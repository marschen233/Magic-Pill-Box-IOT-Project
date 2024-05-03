  /******************************************************************************
  * @file    SHTC3.h
  * @author  Sahil Mangaonkar and Siddhant Mathur
  * @brief   SHTC3 driver file
  * @date    2023-04-20
  ******************************************************************************/
  #ifndef SHTC3_H
  #define SHTC3_H

  #ifdef __cplusplus
  extern "C" {
	  #endif
	  
/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <math.h>

/******************************************************************************
 * Defines
 ******************************************************************************/
#define SHTC3_ADDR 0x70
#define SHTC3_WAKEUP_CMD 0x3517
#define SHTC3_WAKEUP_CMD1 0x35
#define SHTC3_WAKEUP_CMD2 0x17
#define SHTC3_SLEEP_CMD 0xB098
#define SHTC3_SOFT_RESET_CMD 0x805D
#define SHTC3_ID_REG 0xEFC8

#define WAIT_TIME 0xff

#define SHT3_TH_NM_NCS_MEASURE_CMD 0x7866  ///< Command to measure temperature first, then RH, in normal power mode, no clock stretching
#define SHT3_TH_NM_NCS_MEASURE_CMD1 0x78   ///< Command to measure temperature first, then RH, in normal power mode, no clock stretching
#define SHT3_TH_NM_NCS_MEASURE_CMD2 0x66  ///< Command to measure temperature first, then RH, in normal power mode, no clock stretching

#define SHT3_TH_LPM_NCS_MEASURE_CMD 0x609C  ///< Command to measure temperature first, then RH, in low power mode, no clock stretching

#define SHT3_HT_NM_NCS_MEASURE_CMD 0x58E0 ///< Command to measure RH first, then temperature, in normal power mode, no clock stretching
#define SHT3_HT_LPM_NCS_MEASURE_CMD 0x401A  ///< Command to measure RH first, then temperature, in low power mode, no clock stretching

#define SHT3_TH_NM_CS_MEASURE_CMD 0x7CA2  ///< Command to measure temperature first, then RH, in normal power mode, clock stretching
#define SHT3_TH_LPM_CS_MEASURE_CMD 0x6458 ///< Command to measure temperature first, then RH, in low power mode, clock stretching

#define SHT3_HT_NM_CS_MEASURE_CMD 0x5C24 ///< Command to measure RH first, then temperature, in normal power mode,  clock stretching
#define SHT3_HT_LPM_CS_MEASURE_CMD 0x44DE  ///< Command to measure RH first, then temperature, in low power mode, clock stretching

/******************************************************************************
 * Structures and Enumerations
 ******************************************************************************/

/******************************************************************************
 * Global Function Declaration
 ******************************************************************************/
//int SHTC3_Init(void);
//int SHTC3_SendI2cCommand(uint8_t *buf, uint8_t size);
//int SHTC3_Read_Data(uint8_t *buf, uint8_t size);
//int SHTC3_Init(uint8_t *buffer, uint8_t count);
int SHTC3_Init(void);
int32_t SHTC3_ReadTemperatureAndHumidity(uint8_t *buffer, uint8_t count);

#ifdef __cplusplus
}
#endif

#endif /*SHTC3_DRIVER_H */

