/* Necessary Libraries */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_event.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#define ECHO_TEST_TXD  (GPIO_NUM_10) //for UART1 -> TXD
#define ECHO_TEST_RXD  (GPIO_NUM_9)	 //for UART1 -> RXD
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE) //Unchanged
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE) //Unchanged

#define BUF_SIZE (1024)//Buffer size
uint8_t *data;


static void uart_task(){
  // Configure a temporary buffer for the incoming data
  data = (uint8_t *) malloc(BUF_SIZE);
  // Notifier set to "ok"
  char notifier[3] = "OK";
  int s = 0;
  // Read data from the UART
  while (1) {
    /* Prevents the Watch Dog Timer from tiggering */
    TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed = 1;
    TIMERG0.wdt_wprotect = 0;
    if (s == 0){
      uart_write_bytes(UART_NUM_1, notifier, 10);
    }

    int len = uart_read_bytes(UART_NUM_1, data, 10, 20 / portTICK_RATE_MS);

    if(len > 0){
      s++;
      data[len] = '\0';
      printf("%s\n", data);
      fflush(stdout);
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}

static void uart_init(){
    /* Configure parameters of an UART driver, communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 38400,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, 10, 9, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE, 1024, 1024, NULL, 0);
    uart_task();
}


void app_main(){
    xTaskCreate(uart_init, "uart_init", 1024, NULL, 10, NULL);
}
