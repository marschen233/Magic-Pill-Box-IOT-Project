/**
 * @file      main.c
 * @brief     Main application entry point
 * @author    Eduardo Garcia
 * @author    Nick M-G
 * @date      2022-04-14
 ******************************************************************************/

/****
 * Includes
 ******************************************************************************/
#include <errno.h>

#include "CliThread/CliThread.h"
#include "FreeRTOS.h"
#include "I2cDriver\I2cDriver.h"
#include "SerialConsole.h"
#include "WifiHandlerThread/WifiHandler.h"
#include "asf.h"
#include "driver/include/m2m_wifi.h"
#include "main.h"
#include "stdio_serial.h"
#include "OLED/fonts.h"
#include "OLED/SSD1306.h"

// to use the SHTC3, we create a SHTC3 and SHTC3Task, so include these code
#include "SHTC3/SHTC3.h"
#include "SHTC3/SHTC3_TASK.h"

#include "IMU/lsm6dso_reg.h"

/****
 * Defines and Types
 ******************************************************************************/
#define APP_TASK_ID 0 /**< @brief ID for the application task */
#define CLI_TASK_ID 1 /**< @brief ID for the command line interface task */

/****
 * Local Function Declaration
 ******************************************************************************/
void vApplicationIdleHook(void);
//!< Initial task used to initialize HW before other tasks are initialized
static void StartTasks(void);
void vApplicationDaemonTaskStartupHook(void);

void vApplicationStackOverflowHook(void);
void vApplicationMallocFailedHook(void);
void vApplicationTickHook(void);
static void DisplayTask(void *pvParameters);


/****
 * Variables
 ******************************************************************************/
static TaskHandle_t cliTaskHandle = NULL;      //!< CLI task handle
static TaskHandle_t daemonTaskHandle = NULL;   //!< Daemon task handle
static TaskHandle_t wifiTaskHandle = NULL;     //!< Wifi task handle
static TaskHandle_t uiTaskHandle = NULL;       //!< UI task handle
static TaskHandle_t controlTaskHandle = NULL;  //!< Control task handle
static TaskHandle_t SHTC3TaskHandle = NULL;  //!< Control task handle
static TaskHandle_t displayTaskHandle = NULL;

char bufferPrint[64];  ///< Buffer for daemon task

/**
 * @brief Main application function.
 * Application entry point.
 * @return int
 */
int main(void)
{
    /* Initialize the board. */
    system_init();		//we set up user pin and LED right here
    /* Initialize the UART console. */
    InitializeSerialConsole();
    // Initialize trace capabilities
    vTraceEnable(TRC_START);
    // Start FreeRTOS scheduler
    vTaskStartScheduler();

    return 0;  // Will not get here
}

/**
 * function          vApplicationDaemonTaskStartupHook
 * @brief            Initialization code for all subsystems that require FreeRToS
 * @details			This function is called from the FreeRToS timer task. Any code
 *					here will be called before other tasks are initilized.
 * @param[in]        None
 * @return           None
 */
void vApplicationDaemonTaskStartupHook(void)
{
    SerialConsoleWriteString("\r\n\r\n-----ESE516 Main Program-----\r\n");

    // Initialize HW that needs FreeRTOS Initialization
    SerialConsoleWriteString("\r\n\r\nInitialize HW...\r\n");
    if (I2cInitializeDriver() != STATUS_OK) {	//the I2C driver only initialize right here, so if you initialize anything that needs the I2C communication 
        SerialConsoleWriteString("Error initializing I2C Driver!\r\n");
    } else {
        SerialConsoleWriteString("Initialized I2C Driver!\r\n");
    }
  if (SSD1306_Init() != 1) {
	  SerialConsoleWriteString("SSD1306 Initialization failed!\r\n");
	  } else {
	  SerialConsoleWriteString("SSD1306 Successfully Initialized!\r\n");

  }
    StartTasks();

    vTaskSuspend(daemonTaskHandle);
}

/**
 * function          StartTasks
 * @brief            Initialize application tasks
 * @details
 * @param[in]        None
 * @return           None
 */
static void StartTasks(void)
{
    snprintf(bufferPrint, 64, "Heap before starting tasks: %d\r\n", xPortGetFreeHeapSize());
    SerialConsoleWriteString(bufferPrint);

    // Initialize Tasks here
	
	port_pin_set_output_level(PIN_PA11, false);		//buzzer stop buzz

//    if (xTaskCreate(vCommandConsoleTask, "CLI_TASK", CLI_TASK_SIZE, NULL, CLI_PRIORITY, &cliTaskHandle) != pdPASS) {
//       SerialConsoleWriteString("ERR: CLI task could not be initialized!\r\n");
//    }

   snprintf(bufferPrint, 64, "Heap after starting CLI: %d\r\n", xPortGetFreeHeapSize());
    SerialConsoleWriteString(bufferPrint);

    if (xTaskCreate(vWifiTask, "WIFI_TASK", WIFI_TASK_SIZE, NULL, WIFI_PRIORITY, &wifiTaskHandle) != pdPASS) {
       SerialConsoleWriteString("ERR: WIFI task could not be initialized!\r\n");
    }
    snprintf(bufferPrint, 64, "Heap after starting WIFI: %d\r\n", xPortGetFreeHeapSize());
    SerialConsoleWriteString(bufferPrint);
	
	//create I2c task, this is the IMU task, we are not using IMU, so comment this code
//	if (xTaskCreate(vI2cTask, "vI2cTask", vI2C_TASK_SIZE, NULL, vI2C_PRIORITY, NULL) != pdPASS) {
//	    SerialConsoleWriteString("ERR: I2c task could not be initialized!\r\n");
//	}
	snprintf(bufferPrint, 64, "Heap after starting I2C: %d\r\n", xPortGetFreeHeapSize());
	SerialConsoleWriteString(bufferPrint);
	
	// create a SHTC3 TASK //
	if (xTaskCreate(SHTC3Task, "SHTC3 TASK", SHTC3_TASK_SIZE, NULL, SHTC3_PRIORITY, &SHTC3TaskHandle) != pdPASS) {
		SerialConsoleWriteString("ERR: SHTC3 TASK could not be initialized!\r\n");
	}
	snprintf(bufferPrint, 64, "Heap after starting SHTC3 Task : %d\r\n", xPortGetFreeHeapSize());
	SerialConsoleWriteString(bufferPrint);
	// SHTC3 TASK //
	
	
//	if (xTaskCreate(DisplayTask, "DISPLAY_TASK", 512, NULL, 5, &displayTaskHandle) != pdPASS) {	//DISPLAY_TASK_STACK_SIZE 512
//		SerialConsoleWriteString("ERR: Display task could not be initialized!\r\n");
//	}
	


}

void vApplicationMallocFailedHook(void)
{
    SerialConsoleWriteString("Error on memory allocation on FREERTOS!\r\n");
    while (1)
        ;
}

void vApplicationStackOverflowHook(void)
{
    SerialConsoleWriteString("Error on stack overflow on FREERTOS!\r\n");
    while (1)
        ;
}

#include "MCHP_ATWx.h"
void vApplicationTickHook(void)
{
    SysTick_Handler_MQTT();
}

static void DisplayTask(void *pvParameters) {
	while (1) {
		SerialConsoleWriteString("Updating display...\r\n");
		SSD1306_Fill(SSD1306_COLOR_BLACK);
		SSD1306_GotoXY(10, 25);
		SSD1306_Puts("MagicPB", &Font_11x18, SSD1306_COLOR_WHITE);

		// Update the entire buffer for consistency
		SSD1306_UpdateScreen();

		vTaskDelay(1000);  // Update every second
	}
}