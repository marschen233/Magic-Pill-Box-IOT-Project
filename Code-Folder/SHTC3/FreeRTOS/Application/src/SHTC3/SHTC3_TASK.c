
#include "SHTC3/SHTC3_TASK.h"
#include "I2cDriver/I2cDriver.h"
#include "WifiHandlerThread/WifiHandler.h"
#include "SHTC3/SHTC3.h"
#include "Buzzer/Buzzer.h"

#include "OLED/fonts.h"
#include "OLED/SSD1306.h"

//#include "Pulse rate/pulse.h"
//#include "DRV2605\DRV2605.h"

/* Instance of MQTT service. */
static struct mqtt_module mqtt_inst;

volatile char mqtt_msgg[64] = "{\"d\":{\"temp\":17}}\"";
	
extern xQueueTempBuffer;
extern xQueueMoistBuffer;

int fire_alarm_flg = 0;

/**
 * @fn		    void SHTC3Task(void *pvParameters)
 * @brief       Runs temperature measurement task in parallel
 * @details 	Runs temperature measurement task in parallel

 * @return		Returns nothing.
 * @note
 */
void SHTC3Task(void *pvParameters) {
	uint8_t buffer[5];
	uint8_t logBuffer[64];
	SerialConsoleWriteString("SHTC3 Task Started\r\n");

	int32_t initializationStatus = SHTC3_Init();

	int32_t temperature = 0;
	int32_t humidity = 0;

	while (1) {
		
		//uint8_t buffer[5] = {0};

		int32_t sensorReadStatus = SHTC3_ReadTemperatureAndHumidity(buffer, 5);
		
		if (sensorReadStatus != 0) {
			SerialConsoleWriteString("Error reading SHTC3 sensor data!\r\n");
		} else {
			temperature = (-45 + (((buffer[0] << 8) | buffer[1]) * 175) / 65536);		//temp 68 
			humidity = (100 * ((buffer[3] << 8) | buffer[4])) / 65536;
		
			
//		snprintf((char *)logBuffer, sizeof(logBuffer), "Temperature: %d, Humidity: %d%%\r\n", temperature, humidity);
//		SerialConsoleWriteString(logBuffer);
		
		//PUBLISH this on the mqtt		
		// this is use to print out the temp and moist for, we will have to send the data through mqtt
		
		//set the value of the moist and the temperature to the XQueueBuffer
		xQueueSend(xQueueTempBuffer, &temperature, (TickType_t)10);
		xQueueSend(xQueueMoistBuffer, &humidity, (TickType_t)10);
		
		if(port_pin_get_input_level(BUTTON_0_PIN) == false)
		{
			port_pin_set_output_level(PIN_PA11, true);		//buzzer buzz	
		}

			//now we save the temp and humid value,
			if(temperature > 50)		//fire alarm
			{
				fire_alarm_flg = 1;
				SerialConsoleWriteString("there is a fire!!! run \r\n");
				port_pin_set_output_level(PIN_PA11, true);		//buzzer buzz
			} 
			

				if((temperature > 28 || humidity > 80 ) && fire_alarm_flg == 0  )
				{
					SerialConsoleWriteString("pill damage\r\n");
					port_pin_toggle_output_level(PIN_PA11);
					vTaskDelay(50);
					port_pin_set_output_level(PIN_PA11, false);		//buzzer stop buzz
						
				}

			

			
		//when the box is open, then use the button to get

		if (port_pin_get_input_level(PIN_PA25) == true)		//button is released
		{
			SerialConsoleWriteString("box 1 open\r\n");
			port_pin_set_output_level(PIN_PA11, false);		//buzzer stop buzz
			
		}
		
		if (port_pin_get_input_level(PIN_PA21) == true)		//button is released
		{
			SerialConsoleWriteString("box 2 open\r\n");
			port_pin_set_output_level(PIN_PA11, false);		//buzzer stop buzz
		}
		
		if (port_pin_get_input_level(PIN_PA20) == true)		//button is released
		{
			SerialConsoleWriteString("box 3 open\r\n");
			port_pin_set_output_level(PIN_PA11, false);		//buzzer stop buzz
		}
		
		
			// if the inner button is pressed, then we can stop the pill alarm
			//also turn off the MqttLed

		}
		vTaskDelay(500);
	}
}

