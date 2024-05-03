#ifndef SSD1306_H_
#define SSD1306_H_

#include "fonts.h"  // Include custom font definitions needed for displaying text.

#ifdef __cplusplus
extern "C" {
	#endif

	/******************************************************************************
	* Defines and Typedefs
	******************************************************************************/

	// I2C address of the SSD1306 OLED display.
	#define SSD1306_I2C_ADDRESS 0x3D

	// Display width and height in pixels for the SSD1306 model.
	#define SSD1306_WIDTH 128
	#define SSD1306_HEIGHT 64

	// Enumerated type for defining display colors.
	typedef enum {
		SSD1306_COLOR_BLACK = 0, // Represents the color black (pixel off).
		SSD1306_COLOR_WHITE = 1  // Represents the color white (pixel on).
	} SSD1306_COLOR_t;

	// Structure to store the state of the SSD1306 display.
	typedef struct {
		uint16_t CurrentX;        // Current cursor position on the x-axis.
		uint16_t CurrentY;        // Current cursor position on the y-axis.
		uint8_t Inverted;         // Flag indicating if the display colors are inverted.
		uint8_t Initialized;      // Flag to check if the display has been initialized.
	} SSD1306_t;

	/******************************************************************************
	* Public function prototypes
	******************************************************************************/

	// Function to initialize the SSD1306 display.
	uint8_t SSD1306_Init(void);

	// Function to update the entire screen based on the current buffer.
	void SSD1306_UpdateScreen(void);

	// Function to clear the display buffer (set all pixels to off/black).
	void SSD1306_Clear(void);

	// Function to fill the entire display with a single color.
	void SSD1306_Fill(SSD1306_COLOR_t color);

	// Function to turn the display on (wake up).
	void SSD1306_ON(void);

	// Function to turn the display off (sleep mode).
	void SSD1306_OFF(void);

	// Function to toggle the inversion of colors on the display.
	void SSD1306_ToggleInvert(void);

	// Function to draw a pixel at a specified location with a specified color.
	void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color);

	// Function to set the cursor position for subsequent text or pixel operations.
	void SSD1306_GotoXY(uint16_t x, uint16_t y);

	// Function to draw a character at the current cursor location using a specified font and color.
	char SSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color);

	// Function to draw a string of characters starting at the current cursor location.
	char SSD1306_Puts(char* str, FontDef_t* Font, SSD1306_COLOR_t color);

	// Function to draw a line between two points with a specified color.
	void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t color);

	// Function to draw a rectangle with specified top-left corner, width, height, and color.
	void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t color);

	// Function to draw a filled rectangle with specified top-left corner, width, height, and color.
	void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t color);

	// Function to draw a triangle connecting three points with a specified color.
	void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color);

	// Function to draw a filled triangle connecting three points with a specified color.
	void SSD1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color);

	// Function to draw a circle with a specified center, radius, and color.
	void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t color);

	// Function to draw a filled circle with a specified center, radius, and color.
	void SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t color);

	#ifdef __cplusplus
}
#endif

#endif // SSD1306_H_
