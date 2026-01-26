#include "xst.h"

uint8_t RX_Buffer[128];

uint8_t note_buffer[128];
uint8_t reply_buffer[128];
uint8_t image_buffer[128];

/*******************************************任务、队列handle*************************************************/
//接受xst接受的数据队列
QueueHandle_t     receive_data_handle = NULL;

//xst协议不同通知的二值信号量
SemaphoreHandle_t Note_Semaphore = NULL ;
SemaphoreHandle_t Reply_Semaphore = NULL ;
SemaphoreHandle_t Image_Semaphore = NULL ;

/********************************************END!!!END!!!****************************************************/
/****************************************xst串口初始化及其收发数据*********************************************/
#define xst_uart_usStackDepth 64
#define xst_uart_uxPriority   32

QueueHandle_t uart_queue;
const uart_port_t uart_num = xst_uart_num;

void xst_receive_data_init(void){
    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(xst_uart_num, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(xst_uart_num, xst_tx_pin, xst_rx_pin, GPIO_NUM_NC, GPIO_NUM_NC));

}

void xst_send_data(uint8_t data) {
    uart_write_bytes(uart_num, (const char*)&data, 1);
}

void xst_send_array(uint8_t *data, size_t length) {
    uart_write_bytes(uart_num, (const char*)data, length);
}

void xst_receive_task(void *param){
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t Semaphore_return = pdFALSE;
    while (1){
        int receive_byte = uart_read_bytes(xst_uart_num , RX_Buffer , sizeof(RX_Buffer) , pdMS_TO_TICKS(5));
        if (receive_byte > 0){
            ESP_LOGI(XST_TAG , "Has ReceivE data");
            if (RX_Buffer[0] == XST_SYNC_WORD_H && RX_Buffer[1] == XST_SYNC_WORD_L){
                ESP_LOGI(XST_TAG , "Has Receive syncword");
                switch (RX_Buffer[2]){
                    case MID_REPLY:
                        memcpy(reply_buffer , RX_Buffer , 128 );
                        Semaphore_return = xSemaphoreGiveFromISR (Reply_Semaphore , &xHigherPriorityTaskWoken);
                    break;

                    case MID_NOTE:
                        memcpy(note_buffer , RX_Buffer , 128 );
                        Semaphore_return = xSemaphoreGiveFromISR (Note_Semaphore , &xHigherPriorityTaskWoken);
                    break;

                    case MID_IMAGE:
                        memcpy(image_buffer , RX_Buffer , 128 );
                        Semaphore_return = xSemaphoreGiveFromISR (Image_Semaphore , &xHigherPriorityTaskWoken);
                    break;

                    default:
                    break;
                }
                // 如果有任务被唤醒，需要进行上下文切换
                if (xHigherPriorityTaskWoken) {
                    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
/**************************************ENDENDEND!!!!!!!***********************************************/
/**************************************XST处理不同类型的数据**********************************************/
#define xst_usStackDepth 64
#define xst_uxPriority   32

void xst_init(void){
    Note_Semaphore = xSemaphoreCreateBinary();
    Reply_Semaphore = xSemaphoreCreateBinary();
    Image_Semaphore = xSemaphoreCreateBinary();
}

/**************************************ENDENDEND!!!!!!!***********************************************/


