#include "stdio.h"
#include "esp_log.h"
#include <driver/gpio.h>
#include <input_iot.h>

input_callback_t input_callback = NULL;

// Ham xu ly callback
static void IRAM_ATTR gpio_input_handler(void* arg)
{
    int gpio_num = (uint32_t) arg;
    input_callback(gpio_num);
}

// Ham nay la ham khoi tao, connect chan bao nhieu, bat suon nao
void input_io_create(gpio_num_t gpio_num, interrupt_type_edle_t type)
{
    gpio_pad_select_gpio(gpio_num);
    gpio_set_direction(gpio_num,GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio_num,GPIO_PULLUP_ONLY);
    gpio_set_intr_type(gpio_num,type);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num,gpio_input_handler,(void*)gpio_num);
}

//get muc logic
int input_io_get_level(gpio_num_t gpio_num)
{
    return gpio_get_level(gpio_num);
}

//set callback de ra ngoai xu ly
// bao ra cai su kien de biet ma xu ly
void input_set_callback(void * cb)
{
    input_callback = cb;
}