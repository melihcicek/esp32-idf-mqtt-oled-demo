#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "ssd1306.h"
#include "i2c_drv.h"
#include "fonts.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "main.h"
#include "esp_adc_cal.h"


extern const uint8_t iot_eclipse_org_pem_start[] asm("_binary_iot_eclipse_org_pem_start");
extern const uint8_t iot_eclipse_org_pem_end[]   asm("_binary_iot_eclipse_org_pem_end");


#define CONFIG_WIFI_SSID "YOURSSID"
#define CONFIG_WIFI_PASSWORD "YOURPASSWORD"
static const char *TAG = "MQTTS_EXAMPLE";


static SemaphoreHandle_t oled_mutex = NULL;

static QueueHandle_t lcd_que;
unsigned char sig, send_sig;

static EventGroupHandle_t wifi_event_group;
const static int CONNECTED_BIT = BIT0;
const uint8_t str_Topic[] = "HOME APP";
const uint8_t str_EspTest[] = "ESP32 Module";
const uint8_t str_Hello[] = "HELLO WORLD (:";
const uint8_t str_Connected[] =  "Connected";
const uint8_t str_Disconnected[] =  "Disconnected";
const uint8_t str_ON[] = "ON";
const uint8_t str_OFF[] = "OFF";
const uint8_t str_Clean_Row[] = "                ";
const uint8_t str_Clean_2col[] = "  ";

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;
esp_mqtt_client_handle_t client;
uint8_t mqtt_connected=0;
static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}

static void wifi_init(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_LOGI(TAG, "start the WIFI SSID:[%s]", CONFIG_WIFI_SSID);
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Waiting for wifi");
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
}

static void Gpio_Init()
{
	gpio_config_t io_conf;
	//disable interrupt
	io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
	//set as output mode
	io_conf.mode = GPIO_MODE_OUTPUT;
	//bit mask of the pins that you want to set,e.g.GPIO18/19
	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	//disable pull-down mode
	io_conf.pull_down_en = 0;
	//enable pull-up mode
	io_conf.pull_up_en = 1;
	//configure GPIO with the given settings
	gpio_config(&io_conf);
}
static void Adc_Init()
{
	 adc1_config_width(ADC_WIDTH_BIT_12);
	 adc1_config_channel_atten(channel, atten);

	 //Characterize ADC
	 adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
	 esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
	// print_char_val_type(val_type);
}


static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
	//esp_mqtt_client_handle_t
	client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, "/yourtopic/top1", 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        	send_sig = 3;
        	xQueueSend(lcd_que, &send_sig, 0); /* Send Message */
           // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
           // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

           // msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
           // ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        	send_sig = 4;
        	xQueueSend(lcd_que, &send_sig, 0); /* Send Message */
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/yourtopic/top0", "data", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            esp_mqtt_client_publish(client, "/yourtopic/top1", event->data, 1, 1, 0);
            if(event->data[0] == '1') {
            	gpio_set_level(GPIO_OUTPUT_IO_1, 1);
            	send_sig = 1;
            	xQueueSend(lcd_que, &send_sig, 0); /* Send Message */
            }
            else if(event->data[0] == '0') {
            	gpio_set_level(GPIO_OUTPUT_IO_1, 0);
            	send_sig = 2;
            	xQueueSend(lcd_que, &send_sig, 0); /* Send Message */
            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}

static void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtts://iot.eclipse.org:8883",
        .event_handle = mqtt_event_handler,
        .cert_pem = (const char *)iot_eclipse_org_pem_start,
    };

    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_start(client);

}
static void i2c_test_task(void *arg)
{

	  while(1){
		OLED_LOCK_MUTEX();
	   	ssd1306_display_string(0, 0, str_Topic, 16, 1);
	   	ssd1306_refresh_gram();

	   	ssd1306_scrol_text(6, 7, SSD1306_RIGHT_HORIZONTAL_SCROLL);
	    ssd1306_scrol_text_start();
	    vTaskDelay(1400/portTICK_PERIOD_MS);
	    ssd1306_scrol_text_stop();
	   	OLED_UNLOCK_MUTEX();
	   	vTaskDelay(100/portTICK_PERIOD_MS);
	   	vTaskDelete(NULL);
	  }
}
static void i2c_test_task_second(void *arg)
{
	int second=0;
	  while(1){
		  OLED_LOCK_MUTEX();
		 // ESP_LOGI(TAG, "I2C START");

		//  ssd1306_display_num(0, 32, (second++)%64, 1, 12);
	   //	ssd1306_display_string(0, 16, Yazi_2, 16, 1);
	  // 	ssd1306_refresh_gram();

	   	OLED_UNLOCK_MUTEX();
	   	vTaskDelay(200/portTICK_PERIOD_MS);
	  }
}


static void lcd_task(void *arg)
{
	portBASE_TYPE status;
	int tmp = 0;
	while(1)
	{
		status = xQueueReceive(lcd_que, &sig, 10 ); /* Receive Message */
		if(status == pdTRUE){
			OLED_LOCK_MUTEX();
			switch(sig)
			{
				case 1:
					ssd1306_display_string(0, 48, str_Clean_Row, 16, 1);
	            	ssd1306_display_string(0, 48, str_ON, 16, 1);
	            	break;
				case 2:
					ssd1306_display_string(0, 48, str_Clean_Row, 16, 1);
	            	ssd1306_display_string(0, 48, str_OFF, 16, 1);
	            	break;
				case 3:
					mqtt_connected = 1;
					ssd1306_display_string(0, 16, str_Clean_Row, 16, 1);
					ssd1306_display_string(0, 16, str_Connected, 16, 1);
					ssd1306_draw_bitmap(112, 0, c_chSingal816,16,8);
	            	break;
				case 4:
					mqtt_connected = 0;
					ssd1306_display_string(0, 16, str_Clean_Row, 16, 1);
		            ssd1306_display_string(0, 16, str_Disconnected, 16, 1);
		            ssd1306_display_string(112, 0, str_Clean_2col, 16, 1);
	            	break;
				default:
					break;
			}
			ssd1306_refresh_gram();
			OLED_UNLOCK_MUTEX();
		}
	}
}

static void adc_task(void *arg)
{
    //Continuously sample ADC1
	 uint32_t adc_reading;

	char str_adc[7];
    while (1) {
    	adc_reading = 0;
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            } else {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, ADC_WIDTH_BIT_12, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES;
        //Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
        sprintf(str_adc, "%d", voltage);
        if(client!= NULL && mqtt_connected)
        	esp_mqtt_client_publish(client, "/yourtopic/top3", str_adc, 2, 1, 0);

        str_adc[3] = str_adc[2];
        str_adc[2] = '.';
        str_adc[4] = ' ';
        str_adc[5] = 'C';
        OLED_LOCK_MUTEX();
        ssd1306_scrol_text_stop();
        ssd1306_display_string(0, 32, (const uint8_t *)str_adc, 16, 1);
        ssd1306_refresh_gram();
       // ssd1306_scrol_text(2, 3, SSD1306_RIGHT_HORIZONTAL_SCROLL);
      // ssd1306_scrol_text_start();
        OLED_UNLOCK_MUTEX();

        vTaskDelay(15000/portTICK_PERIOD_MS);
    }
}
void app_main()
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    nvs_flash_init();
    wifi_init();
 // create mutexe
    oled_mutex = xSemaphoreCreateMutex();
    lcd_que = xQueueCreate(5, sizeof(char));
    Gpio_Init();
    Adc_Init();
   // check_efuse();
    i2c_master_init();
    ssd1306_init();
    mqtt_app_start();


    xTaskCreate(i2c_test_task, "i2c_test_task_0", 1024 * 2, (void *)0, 2, NULL);
   // xTaskCreate(i2c_test_task_second, "i2c_test_task_1", 1024, (void *)0, 10, NULL);
    xTaskCreate(lcd_task, "lcd_task", 1024, (void *)0, 2, NULL);
    xTaskCreate(adc_task, "adc_task", 1024 *2, (void *)0, 2, NULL);
}


