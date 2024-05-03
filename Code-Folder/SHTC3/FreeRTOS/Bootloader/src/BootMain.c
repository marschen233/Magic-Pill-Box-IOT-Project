/**
 * @file      BootMain.c
 * @brief     Main file for the ESE5160 bootloader. Handles updating the main application
 * @details   Main file for the ESE5160 bootloader. Handles updating the main application
 * @author    Eduardo Garcia
 * @author    Nick M-G
 * @date      2024-03-03
 * @version   2.0
 * @copyright Copyright University of Pennsylvania
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "conf_example.h"
#include "sd_mmc_spi.h"
#include <asf.h>
#include <string.h>

#include "ASF/sam0/drivers/dsu/crc32/crc32.h"
#include "SD Card/SdCard.h"		//include the SD card function
#include "SerialConsole/SerialConsole.h"
#include "Systick/Systick.h"

/******************************************************************************
 * Defines
 ******************************************************************************/
#define APP_START_ADDRESS           ((uint32_t) 0x12000)                    ///< Start of main application. Must be address of start of main application
#define APP_START_RESET_VEC_ADDRESS (APP_START_ADDRESS + (uint32_t) 0x04)   ///< Main application reset vector address

#define PAGE_PER_ROW 4 
#define ROW_SIZE 256
#define BOOTLODER_ROW_NUM 288	//this is because 0x00000 to 0x12000 is 73728 byte. 73728/256 = 288

/******************************************************************************
 * Structures and Enumerations
 ******************************************************************************/

struct usart_module cdc_uart_module;   ///< Structure for UART module connected to EDBG (used for unit test output)

/******************************************************************************
 * Local Function Declaration
 ******************************************************************************/
static void jumpToApplication(void);
static bool StartFilesystemAndTest(void);
static void configure_nvm(void);

static bool DeleteApplication(void);
static bool WriteApplication(char* binary_file_name);

/******************************************************************************
 * Global Variables
 ******************************************************************************/
// INITIALIZE VARIABLES
char test_file_name[] = "0:sd_mmc_test.txt";   ///< Test TEXT File name
char test_bin_file[] = "0:sd_binary.bin";      ///< Test BINARY File name

char testA_file_name[] = "0:FlagA.txt"; //test A file name txt
char testB_file_name[] = "0:FlagB.txt"; //test B file name txt
char testA_bin_file[] = "0:TestA.bin"; //test A file name bin
char testB_bin_file[] = "0:TestB.bin"; //test A file name bin

char app_file_name[] = "0:Application.bin";

bool isTaskA = true; 


Ctrl_status status;                            ///< Holds the status of a system initialization
FRESULT res;                                   // Holds the result of the FATFS functions done on the SD CARD TEST
FRESULT restxt; //Holds the result of the FATFS functions done on the SD CARD TEST
FATFS fs;                                      // Holds the File System of the SD CARD
FIL file_object;                               // FILE OBJECT used on main for the SD Card Test

//INITIALIZE FILE VARIABLES
//char testA_file_name[] = "0:FlagA.txt";
//char testB_file_name[] = "0:FlagB.txt";

//char testA_bin_file[] = "0:TestA.bin";
//char testB_bin_file[] = "0:TestB.bin";

char helpstr[64];
	
bool FlagA = false;

/******************************************************************************
 * Global Functions
 ******************************************************************************/

/**
* @fn		int main(void)
* @brief	Main function for ESE5160 Bootloader Application

* @return	Unused (ANSI-C compatibility).
* @note		Bootloader code initiates here.
*****************************************************************************/

int main(void) {

    /*1.) INIT SYSTEM PERIPHERALS INITIALIZATION*/
    system_init();
    delay_init();
    InitializeSerialConsole();
    system_interrupt_enable_global();

    /* Initialize SD MMC stack */
    sd_mmc_init();

    // Initialize the NVM driver
    configure_nvm();

    irq_initialize_vectors();
    cpu_irq_enable();

    // Configure CRC32
    dsu_crc32_init();

    SerialConsoleWriteString("ESE5160 - ENTER BOOTLOADER");   // Order to add string to TX Buffer

    /*END SYSTEM PERIPHERALS INITIALIZATION*/

    /*2.) STARTS SIMPLE SD CARD MOUNTING AND TEST!*/

    // EXAMPLE CODE ON MOUNTING THE SD CARD AND WRITING TO A FILE
    // See function inside to see how to open a file
    SerialConsoleWriteString("\x0C\n\r-- SD/MMC Card Example on FatFs --\n\r");

    if (StartFilesystemAndTest() == false) {
        SerialConsoleWriteString("SD CARD failed! Check your connections. System will restart in 5 seconds...");
        delay_cycles_ms(5000);
        system_reset();
    } else {
        SerialConsoleWriteString("SD CARD mount success! Filesystem also mounted. \r\n");
    }

    /*END SIMPLE SD CARD MOUNTING AND TEST!*/
	
    /*3.) STARTS BOOTLOADER HERE!*/
	// Students - this is your mission!
	
	//I will have to save the A.bin and B.bin into SD card, and also create a FlagA.txt into the SDcard
	// in the A.bin, it will automatically create a FlagB.txt, and for B.bin, it will create a FalgA.txt
	
	//See if the file has any flag
	
	//the appliction will create a FlagA.txt automatically
	
	FlagA = false;
	res = f_open(&file_object, (char const*)app_file_name, FA_READ);	//read if application.bin is exist
		
	
	if(res == FR_OK)		//if Application.bin exist, 
	{
		DeleteApplication();	//then delete the whole application space
		WriteApplication(app_file_name);	// write inside the new applicaiton file
		f_unlink(app_file_name);		//delete the file

	}
	
	f_close(&file_object);

    /* END BOOTLOADER HERE!*/

    // 4.) DEINITIALIZE HW AND JUMP TO MAIN APPLICATION!
    SerialConsoleWriteString("ESE5160 - EXIT BOOTLOADER");   // Order to add string to TX Buffer
    delay_cycles_ms(100);                                    // Delay to allow print

    // Deinitialize HW - deinitialize started HW here!
    DeinitializeSerialConsole();   // Deinitializes UART
    sd_mmc_deinit();               // Deinitialize SD CARD

    // Jump to application
    jumpToApplication();

    // Should not reach here! The device should have jumped to the main FW.
}

/******************************************************************************
 * Static Functions
 ******************************************************************************/

/**
 * function      static void StartFilesystemAndTest()
 * @brief        Starts the filesystem and tests it. Sets the filesystem to the global variable fs
 * @details      Jumps to the main application. Please turn off ALL PERIPHERALS that were turned on by the bootloader
 *				before performing the jump!
 * @return       Returns true is SD card and file system test passed. False otherwise.
 ******************************************************************************/
static bool StartFilesystemAndTest(void) {
    bool sdCardPass = true;
    uint8_t binbuff[256];

    // Before we begin - fill buffer for binary write test
    // Fill binbuff with values 0x00 - 0xFF
    for (int i = 0; i < 256; i++) {
        binbuff[i] = i;
    }

    // MOUNT SD CARD
    Ctrl_status sdStatus = SdCard_Initiate();
    if (sdStatus == CTRL_GOOD)   // If the SD card is good we continue mounting the system!
    {
        SerialConsoleWriteString("SD Card initiated correctly!\n\r");

        // Attempt to mount a FAT file system on the SD Card using FATFS
        SerialConsoleWriteString("Mount disk (f_mount)...\r\n");
        memset(&fs, 0, sizeof(FATFS));
        res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);   // Order FATFS Mount
        if (FR_INVALID_DRIVE == res) {
            LogMessage(LOG_INFO_LVL, "[FAIL] res %d\r\n", res);
            sdCardPass = false;
            goto main_end_of_test;
        }
        SerialConsoleWriteString("[OK]\r\n");

        // Create and open a file
        SerialConsoleWriteString("Create a file (f_open)...\r\n");

        test_file_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
        res = f_open(&file_object, (char const *) test_file_name, FA_CREATE_ALWAYS | FA_WRITE);

        if (res != FR_OK) {
            LogMessage(LOG_INFO_LVL, "[FAIL] res %d\r\n", res);
            sdCardPass = false;
            goto main_end_of_test;
        }

        SerialConsoleWriteString("[OK]\r\n");

        // Write to a file
        SerialConsoleWriteString("Write to test file (f_puts)...\r\n");

        if (0 == f_puts("Test SD/MMC stack\n", &file_object)) {
            f_close(&file_object);
            LogMessage(LOG_INFO_LVL, "[FAIL]\r\n");
            sdCardPass = false;
            goto main_end_of_test;
        }

        SerialConsoleWriteString("[OK]\r\n");
        f_close(&file_object);   // Close file
        SerialConsoleWriteString("Test is successful.\n\r");

        // Write binary file
        // Read SD Card File
        test_bin_file[0] = LUN_ID_SD_MMC_0_MEM + '0';
        res = f_open(&file_object, (char const *) test_bin_file, FA_WRITE | FA_CREATE_ALWAYS);

        if (res != FR_OK) {
            SerialConsoleWriteString("Could not open binary file!\r\n");
            LogMessage(LOG_INFO_LVL, "[FAIL] res %d\r\n", res);
            sdCardPass = false;
            goto main_end_of_test;
        }

        // Write to a binaryfile
        SerialConsoleWriteString("Write to test file (f_write)...\r\n");
        uint32_t varWrite = 0;
        if (0 != f_write(&file_object, binbuff, 256, &varWrite)) {
            f_close(&file_object);
            LogMessage(LOG_INFO_LVL, "[FAIL]\r\n");
            sdCardPass = false;
            goto main_end_of_test;
        }

        SerialConsoleWriteString("[OK]\r\n");
        f_close(&file_object);   // Close file
        SerialConsoleWriteString("Test is successful.\n\r");

    main_end_of_test:
        SerialConsoleWriteString("End of Test.\n\r");

    } else {
        SerialConsoleWriteString("SD Card failed initiation! Check connections!\n\r");
        sdCardPass = false;
    }

    return sdCardPass;
}
/**
 * function      static void jumpToApplication(void)
 * @brief        Jumps to main application
 * @details      Jumps to the main application. Please turn off ALL PERIPHERALS that were turned on by the bootloader
 *				before performing the jump!
 * @return
 ******************************************************************************/
static void jumpToApplication(void) {
    // Function pointer to application section
    void (*applicationCodeEntry)(void);

    // Rebase stack pointer
    __set_MSP(*(uint32_t *) APP_START_ADDRESS);

    // Rebase vector table
    SCB->VTOR = ((uint32_t) APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);

    // Set pointer to application section
    applicationCodeEntry = (void (*)(void))(unsigned *) (*(unsigned *) (APP_START_RESET_VEC_ADDRESS));

    // Jump to application. By calling applicationCodeEntry() as a function we move the PC to the point in memory pointed by applicationCodeEntry,
    // which should be the start of the main FW.
    applicationCodeEntry();
}
/**
 * function      static void configure_nvm(void)
 * @brief        Configures the NVM driver
 * @details
 * @return
 ******************************************************************************/
static void configure_nvm(void) {
    struct nvm_config config_nvm;
    nvm_get_config_defaults(&config_nvm);
    config_nvm.manual_page_write = false;
    nvm_set_config(&config_nvm);
}

static bool DeleteApplication(void)
{
	struct nvm_parameters parameters;
	nvm_get_parameters(&parameters);	//get the parameters for the whole MCU controller
	snprintf(helpstr, 63,"number of pages %d. Page size: %d byte\r\n", parameters.nvm_number_of_pages,parameters.page_size);
	SerialConsoleWriteString(helpstr);
	// find out how many time for the loop to delete, so we have to find out how many rows(256bytes) to delete
	uint8_t RowsToDelete = (parameters.nvm_number_of_pages * parameters.page_size - 0x12000)/ROW_SIZE;		//the 0x12000 is the bootloader code, do not delete the data before 0x12000
	
	//start to delete the application part
	for(int i=0;i<RowsToDelete; i++){
		enum status_code nvmStatus = nvm_erase_row((APP_START_ADDRESS + i*ROW_SIZE));	//start with the app start address 0x12000
		if (nvmStatus != STATUS_OK)	//check if there is any problem
		{
			SerialConsoleWriteString("ERASE ERROR");
			return false;
		}
	}
	return true; //finish delete
}

static bool WriteApplication(char* binary_file_name)
{
	//in this part, we have already select which bin file to load, therefore, we will have to open the file and then load it into the MCU flash
	uint32_t resultCrcSd = 0;
	uint32_t resultCrcNvm = 0;
	
	uint8_t readBuffer[ROW_SIZE];
	FIL file_object_bin;
	UINT numBytesRead = ROW_SIZE;
	
	int addrCnt = 0;	//use to count the iteration of the current address 
	
	res = f_open(&file_object_bin,binary_file_name,FA_READ);	//open the file and then read out into file_object_bin
	if (res != STATUS_OK)	//check if there is any problem
	{
		SerialConsoleWriteString("no File ERROR");
		f_close(&file_object_bin);
		return false;
	}
	
	//find the size of the application, write the data based on the times of the size, loop the functions 
	while (numBytesRead == ROW_SIZE)		//when reading the full row
	{
		// in this function, the program will read the data from file.bin and then load the code into readbuffer[0]
		// every time it will read a row size and then the numBytesRead is the actual reading bytes that has been read. 
		res = f_read(&file_object_bin,&readBuffer[0],ROW_SIZE,&numBytesRead);	
		if (res != STATUS_OK)	//check if there is any problem
		{
			SerialConsoleWriteString("read File ERROR");
			f_close(&file_object_bin);
			return false;
		}
		//after reading the data from the file.bin, we will start to put the read data into application code
		
		int i = 0;
		//if it is the last row (when numBytesRead != ROW_SIZE), then write the remain byte and jump out
		if(numBytesRead != ROW_SIZE)
		{
			uint8_t remain_pages =  numBytesRead/64;	//how many pages left for the last run
			uint8_t remain_bytes =  numBytesRead%64;	//how many bytes left for the last run after pages
			for(i = 0;i<remain_pages;i++)
			{
				nvm_write_buffer(APP_START_ADDRESS + (addrCnt*ROW_SIZE) + (i*64),&readBuffer[64*i],64);	//write the remain pages in the application code.
			}
			nvm_write_buffer(APP_START_ADDRESS + (addrCnt*ROW_SIZE) + (i*64),&readBuffer[64*i],remain_bytes);	//write the remain bytes in app code
			
			dsu_crc32_cal(APP_START_ADDRESS + (addrCnt*ROW_SIZE)+1,ROW_SIZE,&resultCrcNvm);
			break;
		}

		//first, calculate the address to write in
		//after getting the address, write the code into the select address
		nvm_write_buffer(APP_START_ADDRESS + (addrCnt*ROW_SIZE),&readBuffer[0],64);		//the write buffer can only write one pages (64 bytes) so the length is 64
		nvm_write_buffer(APP_START_ADDRESS + (addrCnt*ROW_SIZE) + 64,&readBuffer[64],64);
		nvm_write_buffer(APP_START_ADDRESS + (addrCnt*ROW_SIZE) + 128,&readBuffer[128],64);
		nvm_write_buffer(APP_START_ADDRESS + (addrCnt*ROW_SIZE) + 192,&readBuffer[192],64);
		
		//calculate the SD CRC chunk
		
		*((volatile unsigned int*) 0x41007058) &= ~0x30000UL;
		dsu_crc32_cal(readBuffer,ROW_SIZE,&resultCrcSd);
		*((volatile unsigned int*) 0x41007058) |= 0x20000UL;
		
		//calculate the NVM CRC chunk
		dsu_crc32_cal(APP_START_ADDRESS + (addrCnt*ROW_SIZE),ROW_SIZE,&resultCrcNvm);
		
		addrCnt++;																			
	} 
	
	if(resultCrcNvm != resultCrcSd){
		SerialConsoleWriteString("CRC ERROR\r\n");
		f_close(&file_object_bin);
		return false;
	}
	SerialConsoleWriteString("CRC IS CORRECT!!!\r\n");
	return true;
}