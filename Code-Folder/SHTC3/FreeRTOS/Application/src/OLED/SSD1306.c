/**************************************************************************//**
* @file      SSD1306_Driver.c
* @brief     Driver for the SSD1306 OLED display, utilizing the I2C driver for SAMW25.
* @details   This driver allows basic operations with the SSD1306 display, such as initialization, clearing the display, and updating the screen.
******************************************************************************/
#include "CliThread/CliThread.h"
#include "I2cDriver.h"
#include <string.h> // For memset
#include "fonts.h"
#define ABS(x)   ((x) > 0 ? (x) : -(x))


/******************************************************************************
* Defines and Typedefs
******************************************************************************/
#define SSD1306_I2C_ADDRESS 0x3D  // Adjust if your display has a different address

// Define color types
typedef enum {
SSD1306_COLOR_BLACK = 0,
SSD1306_COLOR_WHITE = 1
} SSD1306_COLOR_t;


typedef struct {
uint16_t CurrentX;
uint16_t CurrentY;
uint8_t Inverted;
uint8_t Initialized;
} SSD1306_t;


// Fundamental Commands


#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64



/******************************************************************************
* Local Function Prototypes
******************************************************************************/
static void SSD1306_WriteCommand(uint8_t cmd);

/******************************************************************************
* Global Variables
******************************************************************************/
/******************************************************************************
* Function Prototypes
******************************************************************************/
static void SSD1306_WriteCommand(uint8_t cmd);
uint8_t SSD1306_Init(void);
void SSD1306_UpdateScreen(void);
void SSD1306_Clear(void);
void SSD1306_Fill(SSD1306_COLOR_t color);
void SSD1306_ON(void);
void SSD1306_OFF(void);
void SSD1306_ToggleInvert(void);
void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color);
void SSD1306_GotoXY(uint16_t x, uint16_t y);
char SSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color);
char SSD1306_Puts(char* str, FontDef_t* Font, SSD1306_COLOR_t color);
void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c);
void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);
void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);
void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color);
void SSD1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color);
void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);
void SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);

/******************************************************************************
* Global Variables
******************************************************************************/
static SSD1306_t SSD1306; // SSD1306 device handle
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

/******************************************************************************
* Functions
******************************************************************************/
static void SSD1306_WriteCommand(uint8_t cmd) {
	I2C_Data i2cData;
	uint8_t* buffer = malloc(2); // Allocate memory for command and control byte
	if (buffer == NULL) {
		// Handle memory allocation failure; e.g., log an error or set an error flag
		SerialConsoleWriteString("Error allocating memory for command");
		return;
	}

	buffer[0] = 0x00; // Co = 0, D/C# = 0 for command mode
	buffer[1] = cmd;  // Command byte

	i2cData.address = SSD1306_I2C_ADDRESS;
	i2cData.msgOut = buffer;
	i2cData.lenOut = 2;

	// Use I2cWriteDataWait to ensure the command is transmitted and handle errors
	int32_t status = I2cWriteDataWait(&i2cData, 10);
	if (status != ERROR_NONE) {
		// Log error or handle it according to your error handling policy
		SerialConsoleWriteString("Error sending I2C command");
	}

	free(buffer); // Free the allocated memory

}


static void SSD1306_WriteData(uint8_t* data, uint16_t count) {
	I2C_Data i2cData;
	uint8_t* buffer = malloc(count + 1); // Allocate memory for data plus control byte
	if (buffer == NULL) {
		// Handle memory allocation failure
		SerialConsoleWriteString("Error allocating memory for data");
		return;
	}

	buffer[0] = 0x40; // Co = 0, D/C# = 1 for data
	memcpy(&buffer[1], data, count); // Copy the data after the control byte

	i2cData.address = SSD1306_I2C_ADDRESS;
	i2cData.msgOut = buffer;
	i2cData.lenOut = count + 1;

	// Use I2cWriteDataWait to ensure the data is transmitted and handle errors
	int32_t status = I2cWriteData(&i2cData);
	if (status != ERROR_NONE) {
		// Log error or handle it according to your error handling policy
		SerialConsoleWriteString("Error sending I2C data");
	}

	free(buffer); // Free the allocated memory
	vTaskDelay(pdMS_TO_TICKS(10)); 
}

// Initialize the SSD1306 display
uint8_t SSD1306_Init(void) {
// Initial command sequence
SSD1306_WriteCommand(0xAE); // Display off
vTaskDelay(pdMS_TO_TICKS(2));  
SSD1306_WriteCommand(0x20); // Memory addressing mode
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0x00); // Horizontal addressing mode
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0xB0); // Set page start address
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0xC8); // COM output scan direction
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0x00); // Set low column address
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0x10); // Set high column address
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0x40); // Set start line address
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0x81); // Set contrast control
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0xFF);
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0xA1); // Segment re-map
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0xA6); // Normal display
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0xA8); // Multiplex ratio
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0x3F);
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0xA4); // Output follows RAM content
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0xD3); // Set display offset
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0x00);
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0xD5); // Set display clock divide ratio
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0xF0);
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0xD9); // Set pre-charge period
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0x22);
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0xDA); // Set COM pins hardware configuration
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0x12);
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0xDB); // Set vcomh
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0x20);
vTaskDelay(pdMS_TO_TICKS(2)); 
SSD1306_WriteCommand(0x8D); // Charge pump setting
vTaskDelay(pdMS_TO_TICKS(10)); 
SSD1306_WriteCommand(0x14);
vTaskDelay(pdMS_TO_TICKS(10)); 
SSD1306_WriteCommand(0xAF); // Turn on SSD1306 panel
vTaskDelay(pdMS_TO_TICKS(10)); 

// Clear screen
memset(SSD1306_Buffer, 0, sizeof(SSD1306_Buffer));
static SSD1306_t SSD1306;

// Set default values for position and flag
SSD1306.CurrentX = 0;
SSD1306.CurrentY = 0;
SSD1306.Inverted = 0;
SSD1306.Initialized = 1;

return 1; // Initialization successful
}

void SSD1306_UpdateScreen(void) {
// Assume SSD1306_Buffer is globally defined and contains the screen data
SSD1306_WriteData(SSD1306_Buffer, SSD1306_WIDTH * SSD1306_HEIGHT / 8);
vTaskDelay(pdMS_TO_TICKS(10)); 
}


/* void SSD1306_UpdateScreen(void) {
	for (uint8_t i = 0; i < 8; i++) {
		SSD1306_WriteCommand(0xB0 + i);
		SSD1306_WriteCommand(0x00);
		SSD1306_WriteCommand(0x10);

		// Continuously write all pixels of a single page (line)
		I2C_Data i2cData;
		i2cData.address = SSD1306_I2C_ADDRESS;
		i2cData.msgOut = &SSD1306_Buffer[SSD1306_WIDTH * i];
		i2cData.lenOut = SSD1306_WIDTH;
		I2cWriteDataWait(&i2cData,200);
	}
	}*/

	void SSD1306_Clear(void) {
	memset(SSD1306_Buffer, 0, sizeof(SSD1306_Buffer));
	}

	/******************************************************************************
	* Your additional functions for drawing text, graphics, etc. go here
	******************************************************************************/


	void SSD1306_Fill(SSD1306_COLOR_t color) {
	// Fill the rest of the buffer
	memset(&SSD1306_Buffer[1], (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(SSD1306_Buffer) - 1);
	}

	void SSD1306_ON(void) {
	SSD1306_WriteCommand(0xAF); // Display on
	}

	void SSD1306_OFF(void) {
	SSD1306_WriteCommand(0xAE); // Display off
	}


	void SSD1306_ToggleInvert(void) {
	SSD1306.Inverted = !SSD1306.Inverted;
	for (uint16_t i = 1; i < sizeof(SSD1306_Buffer); i++) { // start from 1
	SSD1306_Buffer[i] = ~SSD1306_Buffer[i];
	}
	}

	void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color) {
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
	// Error, return
	return;
	}
	
	// Check if pixels are inverted
	if (SSD1306.Inverted) {
	color = (SSD1306_COLOR_t)!color;
	}
	
	// Set color
	if (color == SSD1306_COLOR_WHITE) {
	SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH + 1] |= 1 << (y % 8); // +1 to account for the control byte
	} else {
	SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH + 1] &= ~(1 << (y % 8)); // +1 to account for the control byte
	}
	}
	void SSD1306_GotoXY(uint16_t x, uint16_t y) {
	/* Set write pointers */
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
	}

	char SSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color) {
	uint32_t i, b, j;

	/* Check available space in LCD */
	if (
	SSD1306_WIDTH <= (SSD1306.CurrentX + Font->FontWidth) ||
	SSD1306_HEIGHT <= (SSD1306.CurrentY + Font->FontHeight)
	) {
	/* Error */
	return 0;
	}

	/* Go through font */
	for (i = 0; i < Font->FontHeight; i++) {
	b = Font->data[(ch - 32) * Font->FontHeight + i];
	for (j = 0; j < Font->FontWidth; j++) {
	if ((b << j) & 0x8000) {
	SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t) color);
	} else {
	SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t)!color);
	}
	}
	}

	/* Increase pointer */
	SSD1306.CurrentX += Font->FontWidth;

	/* Return character written */
	return ch;
	}

	char SSD1306_Puts(char* str, FontDef_t* Font, SSD1306_COLOR_t color) {
	/* Write characters */
	while (*str) {
	/* Write character by character */
	if (SSD1306_Putc(*str, Font, color) != *str) {
	/* Return error */
	return *str;
	}

	/* Increase string pointer */
	str++;
	}

	/* Everything OK, zero should be returned */
	return *str;
	}


	void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c) {
	int16_t dx, dy, sx, sy, err, e2, i, tmp;

	/* Check for overflow */
	if (x0 >= SSD1306_WIDTH) {
	x0 = SSD1306_WIDTH - 1;
	}
	if (x1 >= SSD1306_WIDTH) {
	x1 = SSD1306_WIDTH - 1;
	}
	if (y0 >= SSD1306_HEIGHT) {
	y0 = SSD1306_HEIGHT - 1;
	}
	if (y1 >= SSD1306_HEIGHT) {
	y1 = SSD1306_HEIGHT - 1;
	}

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2;

	if (dx == 0) {
	if (y1 < y0) {
	tmp = y1;
	y1 = y0;
	y0 = tmp;
	}

	if (x1 < x0) {
	tmp = x1;
	x1 = x0;
	x0 = tmp;
	}

	/* Vertical line */
	for (i = y0; i <= y1; i++) {
	SSD1306_DrawPixel(x0, i, c);
	}

	/* Return from function */
	return;
	}

	if (dy == 0) {
	if (y1 < y0) {
	tmp = y1;
	y1 = y0;
	y0 = tmp;
	}

	if (x1 < x0) {
	tmp = x1;
	x1 = x0;
	x0 = tmp;
	}

	/* Horizontal line */
	for (i = x0; i <= x1; i++) {
	SSD1306_DrawPixel(i, y0, c);
	}

	/* Return from function */
	return;
	}

	while (1) {
	SSD1306_DrawPixel(x0, y0, c);
	if (x0 == x1 && y0 == y1) {
	break;
	}
	e2 = err;
	if (e2 > -dx) {
	err -= dy;
	x0 += sx;
	}
	if (e2 < dy) {
	err += dx;
	y0 += sy;
	}
	}
	}

	void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) {
	/* Check input parameters */
	if (
	x >= SSD1306_WIDTH ||
	y >= SSD1306_HEIGHT
	) {
	/* Return error */
	return;
	}

	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
	w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
	h = SSD1306_HEIGHT - y;
	}

	/* Draw 4 lines */
	SSD1306_DrawLine(x, y, x + w, y, c);         /* Top line */
	SSD1306_DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
	SSD1306_DrawLine(x, y, x, y + h, c);         /* Left line */
	SSD1306_DrawLine(x + w, y, x + w, y + h, c); /* Right line */
	}

	void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) {
	uint8_t i;

	/* Check input parameters */
	if (
	x >= SSD1306_WIDTH ||
	y >= SSD1306_HEIGHT
	) {
	/* Return error */
	return;
	}

	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
	w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
	h = SSD1306_HEIGHT - y;
	}

	/* Draw lines */
	for (i = 0; i <= h; i++) {
	/* Draw lines */
	SSD1306_DrawLine(x, y + i, x + w, y + i, c);
	}
	}

	void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
	/* Draw lines */
	SSD1306_DrawLine(x1, y1, x2, y2, color);
	SSD1306_DrawLine(x2, y2, x3, y3, color);
	SSD1306_DrawLine(x3, y3, x1, y1, color);
	}


	void SSD1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
	curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
	xinc1 = 1;
	xinc2 = 1;
	} else {
	xinc1 = -1;
	xinc2 = -1;
	}

	if (y2 >= y1) {
	yinc1 = 1;
	yinc2 = 1;
	} else {
	yinc1 = -1;
	yinc2 = -1;
	}

	if (deltax >= deltay){
	xinc1 = 0;
	yinc2 = 0;
	den = deltax;
	num = deltax / 2;
	numadd = deltay;
	numpixels = deltax;
	} else {
	xinc2 = 0;
	yinc1 = 0;
	den = deltay;
	num = deltay / 2;
	numadd = deltax;
	numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
	SSD1306_DrawLine(x, y, x3, y3, color);

	num += numadd;
	if (num >= den) {
	num -= den;
	x += xinc1;
	y += yinc1;
	}
	x += xinc2;
	y += yinc2;
	}
	}

	void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	SSD1306_DrawPixel(x0, y0 + r, c);
	SSD1306_DrawPixel(x0, y0 - r, c);
	SSD1306_DrawPixel(x0 + r, y0, c);
	SSD1306_DrawPixel(x0 - r, y0, c);

	while (x < y) {
	if (f >= 0) {
	y--;
	ddF_y += 2;
	f += ddF_y;
	}
	x++;
	ddF_x += 2;
	f += ddF_x;

	SSD1306_DrawPixel(x0 + x, y0 + y, c);
	SSD1306_DrawPixel(x0 - x, y0 + y, c);
	SSD1306_DrawPixel(x0 + x, y0 - y, c);
	SSD1306_DrawPixel(x0 - x, y0 - y, c);

	SSD1306_DrawPixel(x0 + y, y0 + x, c);
	SSD1306_DrawPixel(x0 - y, y0 + x, c);
	SSD1306_DrawPixel(x0 + y, y0 - x, c);
	SSD1306_DrawPixel(x0 - y, y0 - x, c);
	}
	}

	void SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	SSD1306_DrawPixel(x0, y0 + r, c);
	SSD1306_DrawPixel(x0, y0 - r, c);
	SSD1306_DrawPixel(x0 + r, y0, c);
	SSD1306_DrawPixel(x0 - r, y0, c);
	SSD1306_DrawLine(x0 - r, y0, x0 + r, y0, c);

	while (x < y) {
	if (f >= 0) {
	y--;
	ddF_y += 2;
	f += ddF_y;
	}
	x++;
	ddF_x += 2;
	f += ddF_x;

	SSD1306_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
	SSD1306_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);

	SSD1306_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
	SSD1306_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
	}
	}
