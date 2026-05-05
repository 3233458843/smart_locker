#include "xst.h"
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "lwip/sockets.h"

int g_vofa_client_fd = -1;

// 掌纹进度全局变量（由 main.c 定义，供 UI 实时显示）
extern uint8_t g_xst_palm_progress;
extern bool g_palm_progress_updated;

// ---------------- 核心通信与任务同步资源 ----------------
// 内部使用的命令回复队列 (供业务API阻塞等待结果)
static QueueHandle_t xst_reply_queue = NULL;
// 外部注册的主动通知回调函数
static xst_note_callback_t g_note_callback = NULL;

// 接收缓冲（滑动窗口解析用）
#define PARSE_BUF_SIZE 4096
static uint8_t parse_buf[PARSE_BUF_SIZE];
static uint32_t parse_len = 0;

//解析任务相关的二值信号量和资源锁
static SemaphoreHandle_t xst_parse_sem = NULL; // 二值信号量，用于唤醒解析任务
static SemaphoreHandle_t xst_list_mutex = NULL; // 互斥锁，用于保护待解析数据链表

//解析帧的数据节点链表
typedef struct parse_item{
    uint8_t msg_id;
    uint16_t data_len;
    uint8_t* payload;
    struct parse_item* next;
} parse_item_t;

static parse_item_t* parse_list_head = NULL;
static parse_item_t* parse_list_tail = NULL;

// 定义用于 reply 队列传输的内部结构
typedef struct{
    uint16_t len;
    uint8_t* data; // 指向堆内存，接收方需释放
} queue_item_t;

/*******************************************************************************
 *                               内部辅助与字典翻译函数
 ******************************************************************************/

// 获取 MsgID 对应的字符串
static const char* xst_get_mid_str(uint8_t mid){
    switch (mid){
    case MID_REPLY: return "REPLY (命令回复)";
    case MID_NOTE: return "NOTE (主动通知)";
    case MID_IMAGE: return "IMAGE (图片数据)";
    case MID_LOG: return "LOG (日志)";
    case MID_RESET: return "RESET (复位)";
    case MID_GETSTATUS: return "GET_STATUS (获取状态)";
    case MID_VERIFY: return "VERIFY (验证/识别)";
    case MID_ENROLL: return "ENROLL (录入)";
    case MID_ENROLL_SINGLE: return "ENROLL_SINGLE (单次录入)";
    case MID_ENROLL_PROGRESS : return "ENROLL_PROGRESS (录入进度)";
    case MID_DEL_USER: return "DEL_USER (删除用户)";
    case MID_DEL_ALL: return "DEL_ALL (清空用户)";
    case MID_GET_ALL_USER_ID: return "GET_ALL_USER_ID (获取总数)";
    case MID_GET_USER_INFO: return "GET_USER_INFO (获取信息)";
    case MID_QUIT: return "QUIT (退出)";
    default: return "UNKNOWN_CMD (未知指令)";
    }
}

// 获取 Result 对应的字符串
static const char* xst_get_result_str(uint8_t res){
    switch (res){
    case MR_SUCCESS: return "成功 (SUCCESS)";
    case MR_REJECTED: return "被拒绝 (REJECTED)";
    case MR_ABORTED: return "已终止 (ABORTED)";
    case MR_FAILED4_CAMERA: return "相机故障 (CAMERA_ERR)";
    case MR_FAILED4_UNKNOWN_REASON: return "未知错误 (UNKNOWN_ERR)";
    case MR_FAILED4_INVALID_PARAM: return "参数无效 (INVALID_PARAM)";
    case MR_FAILED4_NO_MEMORY: return "内存不足 (NO_MEMORY)";
    case MR_FAILED4_UNKNOWN_USER: return "未登记用户 (UNKNOWN_USER)";
    case MR_FAILED4_MAX_USER: return "用户已满 (MAX_USER)";
    case MR_FAILED4_ENROLLED: return "用户已存在 (ENROLLED)";
    case MR_FAILED4_LIVENESS_CHECK: return "活体检测失败 (LIVENESS_CHECK)";
    case MR_FAILED4_TIME_OUT: return "超时 (TIME_OUT)";
    case MR_FAILED4_AUTH_FAIL: return "授权失败 (AUTHORIZATION)";
    case MR_FAILED4_READ_FILE: return "读文件失败 (READ_FILE)";
    case MR_FAILED4_WRITE_FILE: return "写文件失败 (WRITE_FILE)";
    default: return "其他错误 (OTHER_ERROR)";
    }
}

// 获取 NID (Note ID) 对应的字符串
static const char* xst_get_nid_str(uint8_t nid){
    switch (nid){
    case NID_READY: return "READY (模组已准备好)";
    case NID_FACE_STATE: return "FACE_STATE (人脸状态更新)";
    case NID_UNKNOWNERROR: return "UNKNOWNERROR (硬件异常)";
    case NID_OTA_DONE: return "OTA_DONE (升级完成)";
    case NID_PALM_STATE: return "PALM_STATE (掌静脉状态更新)";
    case NID_AUTHORIZATION: return "AUTHORIZATION (授权相关)";
    default: return "UNKNOWN_NOTE (未知通知)";
    }
}

// 计算校验和
static uint8_t xst_calc_checksum(uint8_t msg_id, uint16_t size, uint8_t* data){
    uint8_t checksum = 0;
    checksum ^= msg_id;
    checksum ^= (uint8_t)(size >> 8);
    checksum ^= (uint8_t)(size & 0xFF);
    if (data != NULL && size > 0){
        for (int i = 0; i < size; i++){
            checksum ^= data[i];
        }
    }
    return checksum;
}

static void xst_copy_user_name(char* dst, size_t dst_size, const uint8_t* src, size_t src_len){
    if (dst == NULL || dst_size == 0){
        return;
    }

    size_t copy_len = src_len;
    if (copy_len >= dst_size){
        copy_len = dst_size - 1;
    }

    if (src != NULL && copy_len > 0){
        memcpy(dst, src, copy_len);
    }
    dst[copy_len] = '\0';

    for (size_t i = 0; i < copy_len; ++i){
        if ((unsigned char)dst[i] == '\0'){
            break;
        }
    }
}

/*******************************************************************************
 *                               发送底层封装 (带打印功能)
 ******************************************************************************/
static void xst_send_packet(uint8_t msg_id, uint8_t* data, uint16_t len){
    uint32_t total_len = 5 + len + 1; // 5字节头 + 载荷 + 1字节校验
    uint8_t* tx_buf = malloc(total_len);
    if (!tx_buf){
        ESP_LOGE(XST_TAG, "TX Buffer Malloc Failed");
        return;
    }

    // 组装协议头
    tx_buf[0] = XST_SYNC_WORD_H;
    tx_buf[1] = XST_SYNC_WORD_L;
    tx_buf[2] = msg_id;
    tx_buf[3] = (uint8_t)(len >> 8);
    tx_buf[4] = (uint8_t)(len & 0xFF);

    // 组装数据区
    if (len > 0 && data != NULL){
        memcpy(&tx_buf[5], data, len);
    }

    // 计算并写入校验和
    tx_buf[total_len - 1] = xst_calc_checksum(msg_id, len, data);

    if (g_vofa_client_fd != -1){
        send(g_vofa_client_fd, tx_buf, total_len, MSG_DONTWAIT);
    }

    // [功能1] 打印发送的原始 HEX 数据
    ESP_LOGI(XST_TAG, "\n>>>>>>>>>>>>>>>>>>>>>>> [ UART 发送 ] >>>>>>>>>>>>>>>>>>>>>>>");
    ESP_LOGI(XST_TAG, "=> 发送指令: %s (0x%02X), 载荷长: %d, 总长: %d 字节", xst_get_mid_str(msg_id), msg_id, len, (int)total_len);
    esp_log_buffer_hex(XST_TAG "-TX", tx_buf, total_len);
    ESP_LOGI(XST_TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

    uart_write_bytes(XST_UART_NUM, (const char*)tx_buf, total_len);
    free(tx_buf);
}

/*******************************************************************************
 *                               解析与分发业务逻辑
 ******************************************************************************/

// 解析具象化内容并打印
static void xst_print_parsed_frame(uint8_t msg_id, uint16_t data_len, uint8_t* payload){
    ESP_LOGI(XST_TAG, "=> [解析详情] 指令类型: 0x%02X (%s), 载荷长度: %d 字节", msg_id, xst_get_mid_str(msg_id), data_len);

    if (msg_id == MID_REPLY){
        if (data_len >= 2){
            uint8_t reply_mid = payload[0];
            uint8_t result = payload[1];
            ESP_LOGI(XST_TAG, "   [内容] 此帧为响应包 -> 对应的发起命令: 0x%02X (%s)", reply_mid, xst_get_mid_str(reply_mid));
            ESP_LOGI(XST_TAG, "   [内容] 执行结果状态: %d (%s)", result, xst_get_result_str(result));
        }
    }
    else if (msg_id == MID_NOTE){
        if (data_len >= 1){
            uint8_t nid = payload[0];
            ESP_LOGI(XST_TAG, "   [内容] 此帧为主板通知包 -> 发生事件: %d (%s)", nid, xst_get_nid_str(nid));
        }
    }
    else if (msg_id == MID_IMAGE){
        ESP_LOGI(XST_TAG, "[内容] 此帧为大图/流媒体传输...");
    }
    else{
        ESP_LOGI(XST_TAG, "   [内容] 其他上报数据");
    }
}

// 分发给上层应用
static void xst_dispatch_frame(uint8_t msg_id, uint16_t data_len, uint8_t* payload){
    if (msg_id == MID_REPLY){
        if (data_len >= 2){
            uint8_t* body_buf = malloc(data_len);
            if (body_buf != NULL){
                memcpy(body_buf, payload, data_len);
                queue_item_t item;
                item.len = data_len;
                item.data = body_buf; // 移交所有权给业务队列
                if (xQueueSend(xst_reply_queue, &item, 0) != pdTRUE){
                    free(body_buf);
                }
            }
        }
    }
    else if (msg_id == MID_NOTE){
        if (g_note_callback && data_len >= 1){
            uint8_t nid = payload[0];
            g_note_callback(nid, payload + 1, data_len - 1);
        }
    }
}

/*******************************************************************************
 *               任务2：解析任务 (由二值信号量唤醒)
 ******************************************************************************/
static void xst_parse_task(void* param){
    while (1){
        // 等待二值信号量被释放 (阻塞态，不耗CPU)
        if (xSemaphoreTake(xst_parse_sem, portMAX_DELAY) == pdTRUE){
            // 循环取出链表中所有的待解析帧
            while (1){
                parse_item_t* item = NULL;

                // 加锁取出一个节点
                xSemaphoreTake(xst_list_mutex, portMAX_DELAY);
                if (parse_list_head != NULL){
                    item = parse_list_head;
                    parse_list_head = item->next;
                    if (parse_list_head == NULL){
                        parse_list_tail = NULL;
                    }
                }
                xSemaphoreGive(xst_list_mutex);

                // 如果链表空了，退出内层循环，继续等信号量
                if (item == NULL){
                    break;
                }

                // --------- 开始对提取出的一帧进行正式解析 ---------
                ESP_LOGI(XST_TAG, "--- [ 二值信号量唤醒解析任务 ] 提取到 1 帧数据 ---");

                // 打印解析出来的明文
                xst_print_parsed_frame(item->msg_id, item->data_len, item->payload);

                // 转发给应用层业务
                xst_dispatch_frame(item->msg_id, item->data_len, item->payload);

                ESP_LOGI(XST_TAG, "--------------------------------------------------");

                // 清理堆内存
                if (item->payload) free(item->payload);
                free(item);
            }
        }
    }
}

/*******************************************************************************
 *               任务1：串口接收任务 (只负责收数据、切帧、发信号量)
 ******************************************************************************/
static void xst_uart_rx_task(void* param){
    uint8_t temp_buf[256];

    while (1){
        int rx_len = uart_read_bytes(XST_UART_NUM, temp_buf, sizeof(temp_buf), pdMS_TO_TICKS(20));

        if (rx_len > 0){
            if (parse_len + rx_len <= PARSE_BUF_SIZE){
                memcpy(parse_buf + parse_len, temp_buf, rx_len);
                parse_len += rx_len;
            }
            else{
                parse_len = 0; // 防溢出清空
                continue;
            }
        }
        // 滑动窗口切帧
        while (parse_len >= 5){
            int sync_idx = -1;

            // 找包头 0xEF 0xAA
            for (int i = 0; i < parse_len - 1; i++){
                if (parse_buf[i] == XST_SYNC_WORD_H && parse_buf[i + 1] == XST_SYNC_WORD_L){
                    sync_idx = i;
                    break;
                }
            }

            if (sync_idx == -1){
                if (parse_buf[parse_len - 1] == XST_SYNC_WORD_H){
                    parse_buf[0] = XST_SYNC_WORD_H;
                    parse_len = 1;
                }
                else{
                    parse_len = 0;
                }
                break;
            }

            if (sync_idx > 0){
                memmove(parse_buf, parse_buf + sync_idx, parse_len - sync_idx);
                parse_len -= sync_idx;
            }

            if (parse_len < 5) break;

            uint8_t msg_id = parse_buf[2];
            uint16_t data_len = ((uint16_t)parse_buf[3] << 8) | parse_buf[4];
            uint32_t frame_total_len = 5 + data_len + 1;

            if (frame_total_len > PARSE_BUF_SIZE){
                memmove(parse_buf, parse_buf + 2, parse_len - 2);
                parse_len -= 2;
                continue;
            }

            if (parse_len < frame_total_len){
                break; // 帧不完整，继续等
            }

            // ================= 提取出完整帧 =================

            // 打印接收到的原始 HEX 数据
            ESP_LOGI(XST_TAG, "\n<<<<<<<<<<<<<<<<<<<<<<< [ UART 接收 ] <<<<<<<<<<<<<<<<<<<<<<<");
            ESP_LOGI(XST_TAG, "=> 捕获完整帧，总长: %lu 字节", (unsigned long)frame_total_len);
            esp_log_buffer_hex(XST_TAG "-RX", parse_buf, frame_total_len);
            if (g_vofa_client_fd != -1){
                // 使用非阻塞方式发送，防止 WiFi 断开时卡死串口任务
                send(g_vofa_client_fd, parse_buf, frame_total_len, MSG_DONTWAIT);
            }
            ESP_LOGI(XST_TAG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");

            // 校验
            uint8_t calc_sum = xst_calc_checksum(msg_id, data_len, &parse_buf[5]);
            uint8_t rx_sum = parse_buf[frame_total_len - 1];

            if (calc_sum == rx_sum){
                // [功能2] 将合法数据放入链表，并使用“二值信号量”通知解析任务
                parse_item_t* new_item = malloc(sizeof(parse_item_t));
                if (new_item){
                    new_item->msg_id = msg_id;
                    new_item->data_len = data_len;
                    new_item->payload = NULL;
                    new_item->next = NULL;

                    if (data_len > 0){
                        new_item->payload = malloc(data_len);
                        if (new_item->payload){
                            memcpy(new_item->payload, &parse_buf[5], data_len);
                        }
                    }

                    // 加锁入队
                    xSemaphoreTake(xst_list_mutex, portMAX_DELAY);
                    if (parse_list_tail == NULL){
                        parse_list_head = new_item;
                        parse_list_tail = new_item;
                    }
                    else{
                        parse_list_tail->next = new_item;
                        parse_list_tail = new_item;
                    }
                    xSemaphoreGive(xst_list_mutex);

                    // 释放二值信号量，通知解析任务干活
                    xSemaphoreGive(xst_parse_sem);
                }
            }
            else{
                ESP_LOGE(XST_TAG, "RX Checksum Error: calc %02X, got %02X", calc_sum, rx_sum);
            }

            // 将此帧从缓冲区剔除
            if (parse_len > frame_total_len){
                memmove(parse_buf, parse_buf + frame_total_len, parse_len - frame_total_len);
                parse_len -= frame_total_len;
            }
            else{
                parse_len = 0;
            }
        }
    }
}

/*******************************************************************************
 *                               API 实现
 ******************************************************************************/
 
void xst_init(xst_note_callback_t callback){
    g_note_callback = callback;
    //初始化XST模组电源控制引脚
    const gpio_config_t xst_power_pin = {
        .pin_bit_mask = (1ULL << XST_POWER_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    esp_err_t err = gpio_config(&xst_power_pin);
    if (err != ESP_OK){
        ESP_LOGE(XST_TAG, "XST POWER PIN config error!!!");
    }
    //低电平模组给电
    gpio_set_level(XST_POWER_PIN , 0);
    // 初始化串口
    const uart_config_t uart_config = {
        .baud_rate = XST_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(XST_UART_NUM, 4096, 4096, 0, NULL, 0);
    uart_param_config(XST_UART_NUM, &uart_config);
    uart_set_pin(XST_UART_NUM, XST_TX_PIN, XST_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // 创建二值信号量（初始状态为空）
    xst_parse_sem = xSemaphoreCreateBinary();
    // 创建互斥锁保护链表
    xst_list_mutex = xSemaphoreCreateMutex();

    xst_reply_queue = xQueueCreate(3, sizeof(queue_item_t));

    // 创建并行的接收任务和解析任务
    xTaskCreate(xst_uart_rx_task, "xst_rx", 4096, NULL, 15, NULL); // 优先级稍微高一点，保证收包实时性
    xTaskCreate(xst_parse_task, "xst_prs", 4096, NULL, 10, NULL); // 优先级稍微低一点，慢条斯理地解析

    ESP_LOGI(XST_TAG, "XST Driver Initialized with Dual-Task & Binary Semaphore System");
}

// 内部使用的事务处理封装
static xst_result_t xst_exec_cmd(uint8_t cmd, uint8_t* tx_data, uint16_t tx_len,
                                 uint8_t** rx_payload, uint16_t* rx_len, uint32_t timeout){
    if (rx_payload != NULL){
        *rx_payload = NULL;
    }
    if (rx_len != NULL){
        *rx_len = 0;
    }

    xQueueReset(xst_reply_queue);
    xst_send_packet(cmd, tx_data, tx_len);

    TickType_t start_tick = xTaskGetTickCount();
    TickType_t timeout_ticks = pdMS_TO_TICKS(timeout);
    queue_item_t item;

    while (1){
        TickType_t elapsed_ticks = xTaskGetTickCount() - start_tick;
        if (elapsed_ticks >= timeout_ticks){
            break;
        }

        TickType_t remaining_ticks = timeout_ticks - elapsed_ticks;
        if (xQueueReceive(xst_reply_queue, &item, remaining_ticks) != pdTRUE){
            break;
        }

        if (item.data == NULL || item.len < 2){
            ESP_LOGW(XST_TAG, "=> [xst_exec_cmd] 收到非法回复包，长度=%u", item.len);
            if (item.data != NULL){
                free(item.data);
            }
            continue;
        }

        xst_reply_body_t* body = (xst_reply_body_t*)item.data;
        ESP_LOGI(XST_TAG, "=> [xst_exec_cmd] 收到回复, body->mid=0x%02X, 期望cmd=0x%02X", body->mid, cmd);

        if (body->mid != cmd){
            // ENROLL_PROGRESS 回复：提取进度值供 UI 实时显示
            if (body->mid == MID_ENROLL_PROGRESS && item.len >= 3){
                g_xst_palm_progress = body->payload[0];
                g_palm_progress_updated = true;
                ESP_LOGI(XST_TAG, "=> [xst_exec_cmd] 录取进度更新: %d%%", g_xst_palm_progress);
            } else {
                ESP_LOGW(XST_TAG, "=> [xst_exec_cmd] 命令ID不匹配! 丢弃此回复并继续等待目标命令回复");
            }
            free(item.data);
            continue;
        }

        xst_result_t res = (xst_result_t)body->result;
        ESP_LOGI(XST_TAG, "=> [xst_exec_cmd] 命令执行结果: %d (%s)", res, xst_get_result_str(res));

        if (rx_payload && rx_len){
            *rx_len = item.len - 2;
            if (*rx_len > 0){
                *rx_payload = malloc(*rx_len);
                if (*rx_payload != NULL){
                    memcpy(*rx_payload, body->payload, *rx_len);
                }
                else{
                    ESP_LOGE(XST_TAG, "=> [xst_exec_cmd] 申请回复载荷缓存失败");
                    free(item.data);
                    return MR_FAILED4_NO_MEMORY;
                }
            }
        }
        free(item.data);
        return res;
    }

    ESP_LOGW(XST_TAG, "=> [xst_exec_cmd] 等待回复超时 (%lu ms), 命令: 0x%02X", (unsigned long)timeout, cmd);
    return MR_FAILED4_TIME_OUT;
}

// ============== 其余向外暴露的API保持原样不变 ==============

/** XST模组复位
 *
 * @return xst_result_t 错误码
 */
xst_result_t xst_cmd_reset(void){
    gpio_set_level(XST_POWER_PIN , 1 );
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(XST_POWER_PIN , 0);
    return xst_exec_cmd(MID_RESET, NULL, 0, NULL, NULL, 10000);
}

/** 获取XST模组状态
 *
 * @param status 状态码指针，成功时写入状态值
 * @return xst_result_t 错误码
 */
xst_result_t xst_cmd_get_status(uint8_t* status){
    uint8_t* data = NULL;
    uint16_t len = 0;
    xst_result_t res = xst_exec_cmd(MID_GETSTATUS, NULL, 0, &data, &len, 2000);
    if (res == MR_SUCCESS && data != NULL && len >= 1) *status = data[0];
    if (data) free(data);
    return res;
}

/** 添加用户
 *
 * @param name 用户名，最长31字节，内部会自动添加结尾符
 * @param admin 权限，0-255，数值越大权限越高
 * @param timeout 采集超时时间，单位秒，建议20-60秒
 * @param out_user_id 新用户ID指针，成功时写入新用户ID，失败时不修改
 * @return
 */
xst_result_t xst_cmd_enroll_single(const char* name, uint8_t admin, uint8_t timeout, uint16_t* out_user_id){
    xst_enroll_param_t param;
    memset(&param, 0, sizeof(param));
    param.admin = admin;
    param.timeout = timeout;
    strncpy(param.user_name, name, XST_USER_NAME_SIZE - 1);

    uint32_t wait_time = (timeout + 5) * 1000;
    uint8_t* data = NULL;
    uint16_t len = 0;

    xst_result_t res = xst_exec_cmd(MID_ENROLL_SINGLE, (uint8_t*)&param, sizeof(param), &data, &len, wait_time);

    if (res == MR_SUCCESS && data != NULL && len >= sizeof(xst_enroll_reply_t)){
        uint16_t user_id = (uint16_t)(((uint16_t)data[0] << 8) | data[1]);
        uint8_t direction = data[2];
        if (out_user_id != NULL){
            *out_user_id = user_id;
        }
        ESP_LOGI(XST_TAG, "Enroll success: user_id=%u direction=%u",
                 (unsigned int)user_id,
                 (unsigned int)direction);
    }
    if (data) free(data);
    return res;
}


// xst_result_t xst_cmd_enroll_single_id16(){
//
// }

/** 验证用户
 *
 * @param timeout 验证超时时间，单位秒，建议20-60秒
 * @param out_user_id 用户ID指针，成功时写入用户ID，失败时不修改
 * @param out_name 用户名指针，成功时写入用户名，失败时不修改
 * @return xst_result_t 错误码
 */
xst_result_t xst_cmd_verify(uint8_t timeout, uint16_t* out_user_id, char* out_name){
    uint8_t req[2] = {0, timeout};
    uint8_t* data = NULL;
    uint16_t len = 0;
    uint32_t wait_time = (timeout + 5) * 1000;

    xst_result_t res = xst_exec_cmd(MID_VERIFY, req, 2, &data, &len, wait_time);

    if (res == MR_SUCCESS && data != NULL && len >= sizeof(xst_verify_reply_t)){
        uint16_t user_id = (uint16_t)(((uint16_t)data[0] << 8) | data[1]);
        uint8_t admin = data[2 + XST_USER_NAME_SIZE];
        uint8_t unlock_status = data[2 + XST_USER_NAME_SIZE + 1];

        if (out_user_id != NULL){
            *out_user_id = user_id;
        }
        if (out_name != NULL){
            xst_copy_user_name(out_name, XST_USER_NAME_SIZE, &data[2], XST_USER_NAME_SIZE);
        }
        ESP_LOGI(XST_TAG, "Verify success: user_id=%u admin=%u unlock_status=%u",
                 (unsigned int)user_id,
                 (unsigned int)admin,
                 (unsigned int)unlock_status);
    }
    if (data) free(data);
    return res;
}

/** 获取指定用户信息
 *
 * 协议依据：6.18 MID_GET_USER_INFO
 * 请求载荷：user_id 高8位 + 低8位
 * 响应载荷：user_id(2) + user_name(32) + admin(1)
 */
xst_result_t xst_cmd_get_user_info(uint16_t query_id, xst_user_info_t* out_info){
    uint8_t req[2] = {(uint8_t)(query_id >> 8), (uint8_t)(query_id & 0xFF)};
    uint8_t* data = NULL;
    uint16_t len = 0;

    xst_result_t res = xst_exec_cmd(MID_GET_USER_INFO, req, 2, &data, &len, 2000);
    if (res == MR_SUCCESS && data != NULL && len >= sizeof(xst_user_info_t) && out_info != NULL){
        memset(out_info, 0, sizeof(*out_info));
        out_info->id = (uint16_t)(((uint16_t)data[0] << 8) | data[1]);
        memcpy(out_info->name, &data[2], XST_USER_NAME_SIZE);
        out_info->name[XST_USER_NAME_SIZE - 1] = '\0';
        out_info->admin = data[2 + XST_USER_NAME_SIZE];

        ESP_LOGI(XST_TAG, "Get user info success: user_id=%u name=%s admin=%u",
                 (unsigned int)out_info->id,
                 out_info->name,
                 (unsigned int)out_info->admin);
    }

    if (data != NULL){
        free(data);
    }
    return res;
}

/** XST删除指定用户
 *
 * @param user_id 用户ID
 * @return xst_result_t 错误码
 */
xst_result_t xst_cmd_del_user(uint16_t user_id){
    uint8_t req[2] = {(user_id >> 8) & 0xFF, user_id & 0xFF};
    return xst_exec_cmd(MID_DEL_USER, req, 2, NULL, NULL, 10000);
}

/** XST删除所有用户
 *
 * @return xst_result_t 错误码
 */
xst_result_t xst_cmd_del_all(void){
    return xst_exec_cmd(MID_DEL_ALL, NULL, 0, NULL, NULL, 10000);
}

/** XST获取已注册的用户ID列表
 *
 * @param user_ids 用户ID输出数组，可为 NULL（只获取数量）
 * @param max_users user_ids 数组最多可写入的数量
 * @param count 用户数量指针，成功时写入本次已解析出的用户数量
 * @return xst_result_t 错误码
 */
xst_result_t xst_cmd_get_all_user_ids(uint16_t* user_ids, uint16_t max_users, uint16_t* count){
    uint8_t* data = NULL;
    uint16_t len = 0;
    xst_result_t res = xst_exec_cmd(MID_GET_ALL_USER_ID, NULL, 0, &data, &len, 2000);

    if (res == MR_SUCCESS && data != NULL && len >= 1){
        uint16_t reported_count = data[0];
        uint16_t available_count = (len > 1) ? (uint16_t)((len - 1) / 2) : 0;
        uint16_t parsed_count = reported_count;

        if (parsed_count > available_count){
            parsed_count = available_count;
        }
        if (user_ids != NULL && parsed_count > max_users){
            parsed_count = max_users;
        }

        if (user_ids != NULL){
            for (uint16_t i = 0; i < parsed_count; i++){
                uint16_t offset = (uint16_t)(1 + i * 2);
                user_ids[i] = (uint16_t)(((uint16_t)data[offset] << 8) | data[offset + 1]);
            }
        }

        if (count != NULL){
            *count = parsed_count;
        }
        ESP_LOGI(XST_TAG, "Get all user ids success: reported=%u parsed=%u",
                 (unsigned int)reported_count,
                 (unsigned int)parsed_count);
    }
    if (data) free(data);
    return res;
}

/** XST获取已注册的用户
 *
 * @param count 用户数量指针，成功时写入用户数量，失败时不修改
 * @return xst_result_t 错误码
 */
xst_result_t xst_cmd_get_user_count(uint16_t* count){
    return xst_cmd_get_all_user_ids(NULL, 0, count);
}
