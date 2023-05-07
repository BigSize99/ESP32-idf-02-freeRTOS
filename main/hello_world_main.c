/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "output_iot.h"
#include "input_iot.h"
#include "freertos/event_groups.h"

#define BIT_EVENT_BUTTON_PRESS	( 1 << 0 )  // Bit event của nút nhấn.     Mỗi bit tương ứng với 1 event
#define BIT_UART_RECV	( 1 << 1 )           // Bit event nhận dữ liệu UART.
// Khai bao so luong timer
TimerHandle_t xTimers[ 2 ];

/* Declare a variable to hold the created event group. */
EventGroupHandle_t xEventGroup;

void vTask1( void * pvParameters)
{
   for(;;)
   {
      // Chờ 1 hoặc nhóm bit event được set trước đó
      EventBits_t uxBits = xEventGroupWaitBits(
                  xEventGroup,   /* The event group being tested. */
                  BIT_EVENT_BUTTON_PRESS| BIT_UART_RECV, /* The bits within the event group to wait for. */
                  pdTRUE,           // Xoá các bit event yêu cầu trước khi hàm trả về. Xét TRUE để chạy 1 lần, xoá đợi tiếp
                  pdFALSE,          // Không cần đợi hết các event mới vượt qua, chỉ cần có event là qua xét được event
                  portMAX_DELAY );  // Đợi max thời gian
      // Hàm trên đứng đợi chờ event nào vượt qua
      // Xét các event vượt qua
      if(uxBits & BIT_EVENT_BUTTON_PRESS)
      {
         printf("BUTTOn được nhấn! \n");
         output_io_toggle(2);
      }
      if(uxBits & BIT_UART_RECV)
      {
         printf("UART nhận được dữ liệu \n");
      }
   }
}


// Ham handle cua timer
void vTimerCallback( TimerHandle_t xTimer )
{
   /* Optionally do something if the pxTimer parameter is NULL. */
   configASSERT( xTimer ); //Ham check loi
   /* The number of times this timer has expired is saved as the
   timer's ID.  Obtain the count. */
   int ulCount = ( uint32_t ) pvTimerGetTimerID( xTimer );  // Ham nay return lai ID
   if(ulCount == 0)
   {
      output_io_toggle(2);
   }else if(ulCount == 1)
   {
      printf("hello, nhay vao timer 2 roi ne! \n");
   }
}

// event interrup
void button_callback(int pin)
{
   if(pin == 0)
   {  
       BaseType_t pxHigherPriorityTaskWoken;
      // /* Set bit 0 and bit 1 in xEventGroup. */ Tringer sự kiện lên báo hiệu có event
      xEventGroupSetBitsFromISR(xEventGroup, BIT_EVENT_BUTTON_PRESS, &pxHigherPriorityTaskWoken);  /* The bits being set. */
   }

}


void app_main(void)
{

      /* Create then start some timers.  Starting the timers before
     the RTOS scheduler has been started means the timers will start
     running immediately that the RTOS scheduler starts. */
   //Khoi tao timer
   xTimers[ 0 ] = xTimerCreate("TimerBlink", pdMS_TO_TICKS( 500 ), pdTRUE,(void *) 0, vTimerCallback);
   xTimers[ 1 ] = xTimerCreate("TimerPrint", pdMS_TO_TICKS( 1000 ), pdTRUE,(void *) 1, vTimerCallback);
    
   output_io_create(2);
   input_io_create(0,HI_TO_LO);
   input_set_callback(button_callback);
   
    /* Attempt to create the event group. */
    xEventGroup = xEventGroupCreate();

   // Timer start
   // xTimerStart(xTimers[ 0 ], 0);
   // xTimerStart(xTimers[ 1 ], 0);
   /* Create the task, storing the handle. */
   xTaskCreate(
                  vTask1,          /* Function that implements the task. */
                  "vTask1",        /* Text name for the task. */
                  1024*2,            /* Stack size in words, not bytes. */
                  NULL,    /* Parameter passed into the task. */
                  4,               /* Priority at which the task is created. */
                  NULL );      /* Used to pass out the created task's handle. */
}
