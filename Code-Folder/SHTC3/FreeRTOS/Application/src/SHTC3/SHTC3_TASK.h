/*
 * SHTC3_TASK.h
 *               
 * Created: 5/3/2023 4:32:36 PM
 *  Author: siddm
 */ 


#ifndef SHTC3_TASK_H_
#define SHTC3_TASK_H_


#include "I2cDriver/I2cDriver.h"
#include "WifiHandlerThread/WifiHandler.h"	//if this is not in the same level, can we do this?
#include "SHTC3/SHTC3.h"
//#include "Pulse rate/pulse.h"

#define SHTC3_TASK_SIZE	300
#define SHTC3_PRIORITY (configMAX_PRIORITIES -1)  //2
#define TEMP_TOPIC "Mqtttemp"
#define MOIST_TOPIC "Mqttmoist"

void SHTC3Task(void *pvParameters);


#endif /* SHTC3_TASK_H_ */