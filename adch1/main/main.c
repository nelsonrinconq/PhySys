#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/adc.h>

adc1_channel_t potenciometro=ADC1_CHANNEL_6;

void init_hardware(void);

void app_main(void)
{
    init_hardware();
	while (true) {
		int potenciometro1=adc1_get_raw(potenciometro);
		printf("ADC: %d\n",potenciometro1);
		vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

void init_hardware(void){
	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(potenciometro,ADC_ATTEN_DB_11);
}
