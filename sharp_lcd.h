/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file sharp_lcd.h
 * @author Evan Stoddard
 * @brief Driver for Sharp Memory LCD
 */

#ifndef SHARP_LCD_H_
#define SHARP_LCD_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/**
 * @brief Given width and height, returns buffer size in bytes
 * 
 * @param width Width of display in pixels
 * @param height Height of display in pixels
 */
#define SHARP_LCD_CALC_BUFFER_SIZE(width, height) (width / 8) * (height / 8)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @brief Functor typedef for function used to write SPI data
 * 
 */
typedef void (*sharp_lcd_write_functor_t)(uint8_t *frame_buffer, size_t size);

/**
 * @brief Functor typedef for function used to set chip select value
 * 
 */
typedef void (*sharp_lcd_cs_functor_t)(bool active);

/**
 * @brief Struct to handle SPI abstraction
 * 
 */
typedef struct sharp_lcd_spi_interface_t {
    sharp_lcd_write_functor_t write_function;
    sharp_lcd_cs_functor_t cs_function;
} sharp_lcd_spi_interface_t;

/**
 * @brief Typedef struct for sharp lcd driver object
 * 
 */
typedef struct sharp_lcd_t {
    uint16_t width;
    uint16_t height;
    uint8_t *frame_buffer;
    bool vcom;
    sharp_lcd_spi_interface_t spi_interface;
} sharp_lcd_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize driver
 * 
 * @param instance Driver instance
 * @param width Width of display in pixels
 * @param height Height of display in pixels
 * @param frame_buffer Pointer to frame buffer
 */
void sharp_lcd_init(sharp_lcd_t *instance, uint16_t width, uint16_t height, uint8_t *frame_buffer);

/**
 * @brief Set driver SPI interface
 * 
 * @param instance Driver instance
 * @param spi_interface SPI interface struct
 */
void sharp_lcd_set_spi_interface(sharp_lcd_t *instance, sharp_lcd_spi_interface_t spi_interface);

/**
 * @brief Set pixel value
 * 
 * @param instance Driver instance
 * @param x X coordinate
 * @param y Y coordinate
 * @param active Wheter pixel active or not
 */
void sharp_lcd_set_pixel(sharp_lcd_t *instance, uint16_t x, uint16_t y, bool active);

/**
 * @brief Clears display
 * 
 * @param instance Driver instance
 */
void sharp_lcd_clear_display(sharp_lcd_t *instance);

/**
 * @brief Performs periodic VCOM toggling
 * 
 * @param instance Driver instance
 */
void sharp_lcd_toggle_vcom(sharp_lcd_t *instance);

/**
 * @brief Writes entire framebuffer to display
 * 
 * @param instance Driver instance
 */
void sharp_lcd_draw(sharp_lcd_t *instance);

#ifdef __cplusplus
}
#endif
#endif /* SHARP_LCD_H_ */