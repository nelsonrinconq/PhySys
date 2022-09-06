#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <driver/ledc.h>
#include <freertos/timers.h>
#include <esp_log.h>
//variables
static const char *tag = "main";
uint8_t led_level = 0;
TimerHandle_t xTimers;
int interval = 500; // aqui son milisegundos pero solo acepta ticks "pulsos" del esp32 para eso usar pdMS_to_tick
int timerId = 1;
int potenciometro_R;
int potenciometro_G;
int potenciometro_B;
int dutyR;
int dutyG;
int dutyB;
//pines de salida
#define ledR 33
#define ledG 25
#define ledB 26
//pines de entrada 34,35,32,  hacia abajo
adc1_channel_t potenciometroB=ADC1_CHANNEL_6;
adc1_channel_t potenciometroG=ADC1_CHANNEL_7;
adc1_channel_t potenciometroR=ADC1_CHANNEL_4;

esp_err_t init_hardware(void);
esp_err_t set_timer(void);
esp_err_t set_pwm(void);
esp_err_t set_pwm_duty(void);
esp_err_t init_led(void);
//funciones repetidas en el tiempo
void vTimerCallback(TimerHandle_t pxTimer)
{
	init_hardware();
	potenciometro_B=adc1_get_raw(potenciometroB);
	potenciometro_G=adc1_get_raw(potenciometroG);
	potenciometro_R=adc1_get_raw(potenciometroR);
	dutyR = potenciometro_R;
	dutyG = potenciometro_G;
	dutyB = potenciometro_B;
	printf("ADC: %d| %d| %d \n",potenciometro_R,potenciometro_G,potenciometro_B);
//    blink_led();
    set_pwm_duty();
}
//

void app_main(void)
{
    init_led();
    set_pwm();
    set_timer();
//	while (true) {
//		int potenciometro_B=adc1_get_raw(potenciometroB);
//		int potenciometro_G=adc1_get_raw(potenciometroG);
//		int potenciometro_R=adc1_get_raw(potenciometroR);
//		printf("ADC: %d| %d| %d \n",potenciometro_R,potenciometro_G,potenciometro_B);
//		vTaskDelay(300 / portTICK_PERIOD_MS); //esta tarea de tiempo ya no hace falta cuando se define timer
//    }
}

esp_err_t init_hardware(void){
	adc1_config_width(ADC_WIDTH_BIT_10);
	adc1_config_channel_atten(potenciometroB,ADC_ATTEN_DB_11);
	adc1_config_channel_atten(potenciometroG,ADC_ATTEN_DB_11);
	adc1_config_channel_atten(potenciometroR,ADC_ATTEN_DB_11);
    return ESP_OK;
}

esp_err_t set_timer(void)
{
    ESP_LOGI(tag, "configuración del timer");
    xTimers = xTimerCreate("Timer",                   // Just a text name, not used by the kernel.
                           (pdMS_TO_TICKS(interval)), // The timer period in ticks.
                           pdTRUE,                    // The timers will auto-reload themselves when they expire.
                           (void *)timerId,           // Assign each timer a unique id equal to its array index.
                           vTimerCallback             // Each timer calls the same callback when it expires.
    );

    if (xTimers == NULL)
    {
        // The timer was not created.
        ESP_LOGE(tag, "The timer was not created.");
    }
    else
    {
        if (xTimerStart(xTimers, 0) != pdPASS)
        {
            // The timer could not be set into the Active state.
            ESP_LOGE(tag, "The timer could not be set into the Active state.");
        }
    }

    return ESP_OK;
}

esp_err_t set_pwm(void)
{
    ledc_channel_config_t channelConfigR = {0};
    channelConfigR.gpio_num = 33;
    channelConfigR.speed_mode = LEDC_HIGH_SPEED_MODE;
    channelConfigR.channel = LEDC_CHANNEL_0;
    channelConfigR.intr_type = LEDC_INTR_DISABLE;
    channelConfigR.timer_sel = LEDC_TIMER_0;
    channelConfigR.duty = 0;

    ledc_channel_config_t channelConfigG = {0};
    channelConfigG.gpio_num = 25;
    channelConfigG.speed_mode = LEDC_HIGH_SPEED_MODE;
    channelConfigG.channel = LEDC_CHANNEL_1;
    channelConfigG.intr_type = LEDC_INTR_DISABLE;
    channelConfigG.timer_sel = LEDC_TIMER_0;
    channelConfigG.duty = 0;

    ledc_channel_config_t channelConfigB = {0};
    channelConfigB.gpio_num = 26;
    channelConfigB.speed_mode = LEDC_HIGH_SPEED_MODE;
    channelConfigB.channel = LEDC_CHANNEL_2;
    channelConfigB.intr_type = LEDC_INTR_DISABLE;
    channelConfigB.timer_sel = LEDC_TIMER_0;
    channelConfigB.duty = 0;

    ledc_channel_config(&channelConfigR);
    ledc_channel_config(&channelConfigG);
    ledc_channel_config(&channelConfigB);

    ledc_timer_config_t timerConfig = {0};
    timerConfig.speed_mode = LEDC_INTR_DISABLE;
    timerConfig.duty_resolution = LEDC_TIMER_10_BIT;
    // timerConfig.bit_num = LEDC_TIMER_10_BIT;
    timerConfig.timer_num = LEDC_TIMER_0;
    timerConfig.freq_hz = 20000;

    ledc_timer_config(&timerConfig);

    return ESP_OK;
}

esp_err_t set_pwm_duty(void)
{
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, dutyR);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, dutyG);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, dutyB);

    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
    return ESP_OK;
}

esp_err_t init_led(void)
{
    gpio_reset_pin(ledR);
    gpio_set_direction(ledR, GPIO_MODE_OUTPUT);
    gpio_reset_pin(ledG);
    gpio_set_direction(ledG, GPIO_MODE_OUTPUT);
    gpio_reset_pin(ledB);
    gpio_set_direction(ledB, GPIO_MODE_OUTPUT);
    return ESP_OK;
}

