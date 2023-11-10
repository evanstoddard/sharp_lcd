/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file sharp_lcd.c
 * @author Evan Stoddard
 * @brief Driver for Sharp Memory LCD
 */

#include "sharp_lcd.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/



/**
 * @brief Returns frame buffer index given x y coordinate
 * 
 * @param x X coordinate
 * @param y Y coordinate
 */
#define SHARP_LCD_GET_X_Y_BUFFER_INDEX(x, y) (y * instance->width / 8) + (x / 8)

/**
 * @brief Given line number, get buffer array offset
 * 
 * @param line Line offset
 */
#define SHARP_LCD_GET_LINE_OFFSET(line) (instance->width / 8) * line

/**
 * @brief Macro to get line size in bytes
 * 
 */
#define SHARP_LCD_LINE_SIZE_BYTES (instance->width / 8)

#define SHARP_LCD_COMMAND_WRITE 0x01
#define SHARP_LCD_COMMAND_CLEAR 0x04
#define SHARP_LCD_COMMAND_VCOM  0x02

#define SHARP_LCD_VCOM_OFFSET 1

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/
typedef union sharp_lcd_command_t {
    struct {
        uint8_t reserved:4;
        bool unused_bit:1;
        bool clear_bit:1;
        bool vcom_bit:1;
        bool write_bit:1;
    } bits;
    uint8_t byte;
} sharp_lcd_command_t;

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

void sharp_lcd_init(sharp_lcd_t *instance, uint16_t width, uint16_t height, uint8_t *frame_buffer) {
    instance->width = width;
    instance->height = height;
    instance->vcom = false;
    instance->frame_buffer = frame_buffer;
}

void sharp_lcd_set_spi_interface(sharp_lcd_t *instance, sharp_lcd_spi_interface_t spi_interface) {
    instance->spi_interface = spi_interface;
}

void sharp_lcd_set_pixel(sharp_lcd_t *instance, uint16_t x, uint16_t y, bool active) {
    if (active) {
        instance->frame_buffer[SHARP_LCD_GET_X_Y_BUFFER_INDEX(x,y)] |= (1 << x % 8);
    } else {
        uint8_t mask = 0xFF ^ (1 << (x % 8));
        instance->frame_buffer[SHARP_LCD_GET_X_Y_BUFFER_INDEX(x,y)] &= (1 << x % 8);
    }
}

void sharp_lcd_clear_display(sharp_lcd_t *instance) {
    // Create payload
    sharp_lcd_command_t command;
    command.byte = SHARP_LCD_COMMAND_CLEAR;
    command.bits.vcom_bit = instance->vcom;
    uint8_t payload[] = {command.byte, 0};
    
    // Write command to SPI
    instance->spi_interface.cs_function(true);
    instance->spi_interface.write_function(payload, sizeof(payload));
    instance->spi_interface.cs_function(false);

    memset(instance->frame_buffer, 0xFF, SHARP_LCD_CALC_BUFFER_SIZE(instance->width, instance->height));
}

void sharp_lcd_toggle_vcom(sharp_lcd_t *instance) {
    instance->vcom = !instance->vcom;
    
    // Create payload
    sharp_lcd_command_t command;
    command.byte = SHARP_LCD_COMMAND_VCOM;
    command.bits.vcom_bit = instance->vcom;
    uint8_t payload[] = {command.byte, 0};
    
    // Write command to SPI
    instance->spi_interface.cs_function(true);
    instance->spi_interface.write_function(payload, sizeof(payload));
    instance->spi_interface.cs_function(false);
}

void sharp_lcd_draw(sharp_lcd_t *instance) {
    
	uint8_t trailer = 0;

	// Write header
	sharp_lcd_command_t command;
	command.byte = SHARP_LCD_COMMAND_WRITE;
	instance->spi_interface.cs_function(true);
	instance->spi_interface.write_function((uint8_t *)&command, sizeof(sharp_lcd_command_t));

    // Iterate through frame buffer lines and write
    for (uint8_t i = 0; i < instance->height; i++) {

    	uint8_t line_number = i + 1;
    	// Write line number to SPI
		instance->spi_interface.write_function(&line_number, 1);

        // Write frame line
        instance->spi_interface.write_function(
            &instance->frame_buffer[SHARP_LCD_GET_LINE_OFFSET(i)],
            SHARP_LCD_LINE_SIZE_BYTES
        );
        
        // Write trailer
        instance->spi_interface.write_function(&trailer, 1);
    }
    
    // Write final trailer and deassert chip select
	instance->spi_interface.write_function(&trailer, 1);
	instance->spi_interface.cs_function(false);


}
