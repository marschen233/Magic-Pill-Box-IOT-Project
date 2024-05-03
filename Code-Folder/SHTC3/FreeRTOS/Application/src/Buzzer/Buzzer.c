  /******************************************************************************
  * @file	 buzzer.c
  * @author  Jimmy
  * @brief   Buzzer function, including buzzer FreeRTOS task
  * @date    2024-04-25
  ******************************************************************************/
  
  /******************************************************************************
 * Includes
 ******************************************************************************/
 #include "Buzzer.h"

 
 /******************************************************************************
 * Functions
 ******************************************************************************/
 void buzzer_init();
 
 
 /**
 * @fn			buzzer init
 * @brief		set up the buzzer pin right here
 * @details 	PB02 is set for buzzer pin
 * @return		void
 * @note
 */
void buzzer_init()
{	
	struct port_config pin_conf;
	/* Set buzzer as output*/
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PB02, &pin_conf);
	port_pin_set_output_level(PIN_PB02, LED_0_INACTIVE);	//false (0) is open; true (1) is close 
}

 /**
 * @fn			buzzer_case
 * @brief		we can switch different buzzer case right here
 * @details 	use the different buzzer case setting up
 * @return		void
 * @note
 */

uint8_t buzzer_case(int buzzer_case)
{

	switch(buzzer_case)
	{
		case(BUZZER_QUIET):
			port_pin_set_output_level(Buzzer,Buzzer_off);
		break;
		
		case(BUZZER_BUZZING):
			port_pin_set_output_level(Buzzer,Buzzer_on);
		break;
		
	}
	return 0;
}

void buzzer_task()
{
	buzzer_init();
	//SerialConsoleWriteString("Buzzer initialization successful");

}