//librerias autogeneradas por espressif
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
//mis librerias incorporadas
//#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
//contenido
//definiciones de puertos en operación
#define led1 2
#define led2 16
//definiciones de variables
uint8_t led_level =0;
uint8_t count=0;
static const char* tag="main";

//funciones
esp_err_t init_led(void);
esp_err_t blink_led(void);

void app_main(void)
{
    init_led();
    while (1)
    {
        vTaskDelay(1000/portTICK_PERIOD_MS);
        blink_led();
        count +=1;
        if (count>30)
        {
            count=0;
        }

        if (count<10)
        {
            ESP_LOGI(tag,"value: %u.",count);
        }
        if (count >=10 && count < 20)
        {
            ESP_LOGW(tag,"value: %u.",count);
        }
        if (count >= 20)
        {
            ESP_LOGE(tag,"value: %u.",count);
        }

        printf("led_level: %u\n",led_level);

    }

}

esp_err_t init_led(void)
{
    gpio_reset_pin(led1);
    gpio_set_direction(led1,GPIO_MODE_OUTPUT);
    gpio_reset_pin(led2);
    gpio_set_direction(led2,GPIO_MODE_OUTPUT);
    return ESP_OK;
}
esp_err_t blink_led(void)
{
    led_level=!led_level;
    gpio_set_level(led1,led_level);
    gpio_set_level(led2,led_level);
    return ESP_OK;
}
