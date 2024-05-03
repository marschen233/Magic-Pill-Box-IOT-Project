  /******************************************************************************
  * @file	 buzzer.h
  * @author  Jimmy
  * @brief   Buzzer initialization (active buzzer code)
  * @date    2024-04-25
  ******************************************************************************/


/******************************************************************************
 * Descriptions:
 * This is use to change between different types of the buzzer
 *
 * Passive buzzer (the one we bought)
 * We will have to toggle the pin by ourselves, but we can set different frequency and volumes
 * usually freq 3000 - 5000 will be great volume and large sound
 *
 * Active buzzer
 * simple GPIO toggle pin 
 ******************************************************************************/

/******************************************************************************
 * Goals:
 * I hope to use active buzzer and set up the PWM system to toggle the pin to come out with different frequency
 * so that when different situations can have different sounds 
 *
 * Backup plans:
 * use an active buzzer and toggle it simple using GPIO
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
 #include <compiler.h>
 #include <board.h>
 #include <conf_board.h>
 #include <port.h>
 
/******************************************************************************
 * Defines
 ******************************************************************************/
#define Buzzer PIN_PB02 
#define Buzzer_on false
#define Buzzer_off true

/******************************************************************************
 * Structures and Enumerations
 ******************************************************************************/

enum {
	MEASURE_READY = 0, /* Not ready. */
	BUZZER_QUIET = 1,  /* When the buzzer is turn off*/
	BUZZER_BUZZING = 2 /* When the buzzer is open*/
					   /* Buzzer buzzing 3000 freq*/
					   /* Buzzer buzzing 4000 freq*/
					   /* Buzzer buzzing 5000 freq*/
} buzzer_state;


/******************************************************************************
 * Global Function Declaration
 ******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

void buzzer_init();
uint8_t buzzer_case();
void buzzer_task();

#endif /* BUZZER_H_ */