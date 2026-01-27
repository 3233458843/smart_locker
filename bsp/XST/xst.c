#include "xst.h"
#include <string.h>
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"

// 内部使用的命令回复队列
static QueueHandle_t xst_reply_queue = NULL;
// 外部注册的回调函数
static xst_note_callback_t g_note_callback = NULL;

// 串口缓冲区
#define RX_BUF_SIZE 2048 // 增大缓冲区以支持获取所有用户列表或图片
static uint8_t rx_data_buffer[RX_BUF_SIZE];

/*******************************************************************************
 *                               内部辅助函数
 ******************************************************************************/

/**
 * @brief 计算校验和
 * 协议: ParityCheck 为整条协议除去 Sync Word 部分后，其余字节按位做 XOR 运算。
 * 包括 MsgID, Size(2 bytes), Data(N bytes)
 */
static uint8_t xst_calc_checksum(uint8_t msg_id, uint16_t size, uint8_t *data) {
    uint8_t checksum = 0;
    checksum ^= msg_id;
    checksum ^= (uint8_t)(size >> 8);   // Size High
    checksum ^= (uint8_t)(size & 0xFF); // Size Low
    if (data != NULL && size > 0) {
        for (int i = 0; i < size; i++) {
            checksum ^= data[i];
        }
    }
    return checksum;
}

/**
 * @brief 底层发送数据包
 */
static void xst_send_packet(uint8_t msg_id, uint8_t *data, uint16_t len) {
    uint8_t header[5];
    uint8_t checksum;
    
    header[0] = XST_SYNC_WORD_H; 
    header[1] = XST_SYNC_WORD_L; 
    header[2] = msg_id;
    header[3] = (uint8_t)(len >> 8);
    header[4] = (uint8_t)(len & 0xFF);
    
    checksum = xst_calc_checksum(msg_id, len, data);

    // 3. Send UART
    uart_write_bytes(XST_UART_NUM, (const char*)header, 5);
    if (len > 0 && data != NULL) {
        uart_write_bytes(XST_UART_NUM, (const char*)data, len);
    }
    uart_write_bytes(XST_UART_NUM, (const char*)&checksum, 1);
}

/**
 * @brief 发送命令并等待特定回复 (同步阻塞)
 * @param cmd_id 发送的命令ID
 * @param send_data 发送的数据payload
 * @param send_len 发送长度
 * @param out_reply_data 用于接收回复数据的缓冲区指针
 * @param out_reply_len 接收到的数据长度
 * @param timeout_ms 超时时间
 */
static xst_result_t xst_transact(uint8_t cmd_id, uint8_t *send_data, uint16_t send_len, 
                                 uint8_t **out_reply_data, uint16_t *out_reply_len, uint32_t timeout_ms) {
    
    // 1. 清空队列中可能存在的旧数据
    xQueueReset(xst_reply_queue);

    // 2. 发送指令
    xst_send_packet(cmd_id, send_data, send_len);

    // 3. 等待回复
    xst_reply_body_t *reply_packet = NULL;
    if (xQueueReceive(xst_reply_queue, &reply_packet, pdMS_TO_TICKS(timeout_ms)) == pdTRUE) {
        if (reply_packet->mid == cmd_id) {
            xst_result_t res = (xst_result_t)reply_packet->result;          
            if (out_reply_data != NULL) {
                *out_reply_data = reply_packet->payload; // 这里的 payload 指针指向 malloc 块内部
            }
           
            free(reply_packet); // 暂时释放，具体函数逻辑见下文接收任务
            return res;
        } else {
            free(reply_packet);
            ESP_LOGW(XST_TAG, "Reply ID mismatch: expected %02X, got %02X", cmd_id, reply_packet->mid);
            return MR_FAILED4_UNKNOWN_REASON;
        }
    }

    ESP_LOGE(XST_TAG, "Wait reply timeout");
    return MR_FAILED4_TIME_OUT;
}

// 定义用于队列传输的内部结构
typedef struct {
    uint16_t len;
    uint8_t *data; // 指向堆内存，接收方需释放
} queue_item_t;

/*******************************************************************************
 *                               接收任务
 ******************************************************************************/

static void xst_uart_task(void *param) {
    xst_header_t header;
    uint8_t checksum_byte;
    uint8_t *body_buf = NULL;

    while (1) {
        // 1. 读取包头 (5 Bytes)
        int read_len = uart_read_bytes(XST_UART_NUM, &header, sizeof(xst_header_t), pdMS_TO_TICKS(20));
        
        if (read_len != sizeof(xst_header_t)) {
            continue; // 继续等待
        }

        uint8_t *p_head = (uint8_t*)&header;
        if (p_head[0] != XST_SYNC_WORD_H || p_head[1] != XST_SYNC_WORD_L) {
        }
        
        // 重新修正同步字检查，直接对比字节
        if (p_head[0] != 0xEF || p_head[1] != 0xAA) {
            // 没对齐，往后读一个字节尝试重新对其 (简单处理: 丢弃)
            uart_flush_input(XST_UART_NUM); 
            continue;
        }
        uint16_t data_len = (uint16_t)p_head[3] << 8 | p_head[4]; // Page 5 table: Size 2 bytes. 
        data_len = (uint16_t)p_head[3] << 8 | p_head[4]; 

        // 2. 读取数据体
        if (data_len > RX_BUF_SIZE) {
            ESP_LOGE(XST_TAG, "Packet too large: %d", data_len);
            uart_flush_input(XST_UART_NUM);
            continue;
        }

        if (data_len > 0) {
            body_buf = malloc(data_len);
            if (body_buf == NULL) {
                 uart_flush_input(XST_UART_NUM);
                 continue;
            }
            int body_read = uart_read_bytes(XST_UART_NUM, body_buf, data_len, pdMS_TO_TICKS(50));
            if (body_read != data_len) {
                free(body_buf);
                continue;
            }
        }

        // 3. 读取校验位
        uart_read_bytes(XST_UART_NUM, &checksum_byte, 1, pdMS_TO_TICKS(10));

        // 4. 验证校验
        uint8_t cal_sum = xst_calc_checksum(header.MsgID, data_len, body_buf);
        if (cal_sum != checksum_byte) {
            ESP_LOGE(XST_TAG, "Checksum failed: calc %02X, recv %02X", cal_sum, checksum_byte);
            if (body_buf) free(body_buf);
            continue;
        }

        // 5. 分发处理
        if (header.MsgID == MID_REPLY) {
            // 回复消息 -> 发送到队列
            // Reply Body 结构: mid(1) + result(1) + data(N-2)
            if (data_len >= 2) {
                // 我们把整个 body 打包发给 API 等待函数
                queue_item_t item;
                item.len = data_len;
                item.data = body_buf; // 移交所有权
                
                if (xQueueSend(xst_reply_queue, &item, 0) != pdTRUE) {
                    free(body_buf); // 队列满，丢弃
                }
                body_buf = NULL; // 防止在此处 free
            } else {
                if(body_buf) free(body_buf);
            }
        } 
        else if (header.MsgID == MID_NOTE) {
            // 通知消息 -> 调用回调
            // Note Body: nid(1) + data(N-1)
            if (g_note_callback && data_len >= 1) {
                uint8_t nid = body_buf[0];
                g_note_callback(nid, body_buf + 1, data_len - 1);
            }
            if(body_buf) free(body_buf);
        }
        else if (header.MsgID == MID_IMAGE) {
            // 图片数据，可能需要单独处理，或者通过 Note 机制
             if(body_buf) free(body_buf);
        }
        else {
             if(body_buf) free(body_buf);
        }
    }
}

/*******************************************************************************
 *                               API 实现
 ******************************************************************************/

void xst_init(xst_note_callback_t callback) {
    g_note_callback = callback;
    
    // 初始化串口
    uart_config_t uart_config = {
        .baud_rate = XST_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(XST_UART_NUM, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(XST_UART_NUM, &uart_config);
    uart_set_pin(XST_UART_NUM, XST_TX_PIN, XST_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // 创建回复队列
    xst_reply_queue = xQueueCreate(1, sizeof(queue_item_t));

    // 创建接收任务
    xTaskCreate(xst_uart_task, "xst_rx", 4096, NULL, 10, NULL);
    
    ESP_LOGI(XST_TAG, "Initialized");
}

// 内部使用的事务处理封装
static xst_result_t xst_exec_cmd(uint8_t cmd, uint8_t *tx_data, uint16_t tx_len, 
                                 uint8_t **rx_payload, uint16_t *rx_len, uint32_t timeout) {
    xQueueReset(xst_reply_queue);
    xst_send_packet(cmd, tx_data, tx_len);
    
    queue_item_t item;
    if (xQueueReceive(xst_reply_queue, &item, pdMS_TO_TICKS(timeout)) == pdTRUE) {
        // item.data format: [mid][result][payload...]
        xst_reply_body_t *body = (xst_reply_body_t *)item.data;
        
        if (body->mid != cmd) {
            free(item.data);
            return MR_FAILED4_UNKNOWN_REASON;
        }
        
        xst_result_t res = (xst_result_t)body->result;
        
        if (rx_payload && rx_len) {
            // 用户想要数据
            *rx_len = item.len - 2; // sub mid & result
            if (*rx_len > 0) {
                *rx_payload = malloc(*rx_len);
                if (*rx_payload) {
                    memcpy(*rx_payload, body->payload, *rx_len);
                }
            } else {
                *rx_payload = NULL;
            }
        }
        
        free(item.data); // 释放任务里分配的内存
        return res;
    }
    return MR_FAILED4_TIME_OUT;
}

xst_result_t xst_cmd_reset(void) {
    return xst_exec_cmd(MID_RESET, NULL, 0, NULL, NULL, 2000);
}

xst_result_t xst_cmd_get_status(uint8_t *status) {
    uint8_t *data = NULL;
    uint16_t len = 0;
    xst_result_t res = xst_exec_cmd(MID_GETSTATUS, NULL, 0, &data, &len, 2000);
    
    if (res == MR_SUCCESS && data != NULL && len >= 1) {
        *status = data[0];
    }
    if (data) free(data);
    return res;
}

xst_result_t xst_cmd_enroll_single(const char *name, uint8_t admin, uint8_t timeout, uint16_t *out_user_id) {
    xst_enroll_param_t param;
    memset(&param, 0, sizeof(param));
    param.admin = admin;
    param.timeout = timeout;
    strncpy(param.user_name, name, 31); // 安全拷贝

    // 指令超时时间要比参数里的timeout长
    uint32_t wait_time = (timeout + 5) * 1000;
    
    uint8_t *data = NULL;
    uint16_t len = 0;
    
    // 参数结构体直接作为Payload发送
    xst_result_t res = xst_exec_cmd(MID_ENROLL_SINGLE, (uint8_t*)&param, sizeof(param), &data, &len, wait_time);

    if (res == MR_SUCCESS && data != NULL && len >= 2) {
        // 解析返回的 User ID (Big Endian according to structure usually, but check parsing)
        // PDF Page 18: struct { uint8_t user_id_heb; uint8_t user_id_leb; ... }
        // 所以是 High byte first
        if (out_user_id) {
            *out_user_id = (uint16_t)data[0] << 8 | data[1];
        }
    }
    if (data) free(data);
    return res;
}

xst_result_t xst_cmd_verify(uint8_t timeout, uint16_t *out_user_id, char *out_name) {
    // 构造请求包：pd_rightaway(1) + timeout(1)
    uint8_t req[2] = {0, timeout}; 
    
    uint8_t *data = NULL;
    uint16_t len = 0;
    uint32_t wait_time = (timeout + 5) * 1000;

    xst_result_t res = xst_exec_cmd(MID_VERIFY, req, 2, &data, &len, wait_time);
    
    if (res == MR_SUCCESS && data != NULL && len >= 34) { 
        // Reply struct (PDF Page 16): 
        // id_h(1), id_l(1), name(32), admin(1), unlockStatus(1)
        if (out_user_id) {
            *out_user_id = (uint16_t)data[0] << 8 | data[1];
        }
        if (out_name) {
            memcpy(out_name, &data[2], 32);
            out_name[31] = '\0'; // 确保字符串结束
        }
    }
    if (data) free(data);
    return res;
}

xst_result_t xst_cmd_del_user(uint16_t user_id) {
    // PDF Page 25: input struct { id_heb, id_leb }
    uint8_t req[2];
    req[0] = (user_id >> 8) & 0xFF;
    req[1] = user_id & 0xFF;
    return xst_exec_cmd(MID_DEL_USER, req, 2, NULL, NULL, 5000);
}

xst_result_t xst_cmd_del_all(void) {
    return xst_exec_cmd(MID_DEL_ALL, NULL, 0, NULL, NULL, 5000);
}

xst_result_t xst_cmd_get_user_count(uint16_t *count) {
     uint8_t *data = NULL;
    uint16_t len = 0;
    xst_result_t res = xst_exec_cmd(MID_GET_ALL_USER_ID, NULL, 0, &data, &len, 2000);

    if (res == MR_SUCCESS && data != NULL && len >= 2) {
        *count = (uint16_t)data[0] << 8 | data[1];
    }
    if (data) free(data);
    return res;
}

/*
XST在app_main里的use：
#include "xst.h"
#include "esp_log.h"

// Note 异步消息回调
void my_xst_callback(uint8_t nid, uint8_t *data, uint16_t len) {
    switch (nid) {
        case NID_READY:
            ESP_LOGI("APP", "Module is Ready!");
            break;
        case NID_FACE_STATE:
            ESP_LOGI("APP", "Face detected during interaction");
            break;
        default:
            ESP_LOGI("APP", "Received Note ID: %d", nid);
            break;
    }
}

void app_main(void) {
    // 1. 初始化
    xst_init(my_xst_callback);

    // 2. 复位模组
    ESP_LOGI("APP", "Resetting module...");
    if (xst_cmd_reset() == MR_SUCCESS) {
        ESP_LOGI("APP", "Reset Success");
    }

    // 3. 延时等待模组启动
    vTaskDelay(pdMS_TO_TICKS(1000));

    // 4. 获取状态
    uint8_t status = 0;
    xst_cmd_get_status(&status);
    ESP_LOGI("APP", "Status: %d", status);

    // 5. 录入用户 (阻塞直到录入完成或超时)
    uint16_t new_id = 0;
    ESP_LOGI("APP", "Start Enrollment (Please put palm/face)...");
    xst_result_t res = xst_cmd_enroll_single("StudentA", 0, 15, &new_id); // 15秒超时
    
    if (res == MR_SUCCESS) {
        ESP_LOGI("APP", "Enroll Success! ID: %d", new_id);
    } else {
        ESP_LOGE("APP", "Enroll Failed, code: %d", res);
    }

    // 6. 识别测试
    while (1) {
        char name[32] = {0};
        uint16_t uid = 0;
        ESP_LOGI("APP", "Waiting for verify...");
        
        // 此函数会阻塞等待识别结果
        res = xst_cmd_verify(10, &uid, name); // 10秒超时
        
        if (res == MR_SUCCESS) {
            ESP_LOGI("APP", "Verified! Hello ID: %d, Name: %s", uid, name);
        } else if (res == MR_FAILED4_TIME_OUT) {
            ESP_LOGW("APP", "Verify Timeout");
        } else {
            ESP_LOGE("APP", "Verify Error: %d", res);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
*/
