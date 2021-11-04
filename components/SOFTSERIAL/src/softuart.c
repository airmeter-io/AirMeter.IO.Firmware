/*
 * Softuart
 *
 * Copyright (C) 2020 Ivan Belokobylskiy <belokobylskij@gmail.com>
 * Copyright (C) 2017 Ruslan V. Uss <unclerus@gmail.com>
 * Copyright (C) 2016 Bernhard Guillon <Bernhard.Guillon@web.de>
 *
 * The code is slightly modified to use ESP8266_RTOS SDK
 * from Esspressif. The original code [2] is based on Softuart
 * from here [1] and reworked to fit into esp-open-rtos.
 *
 * it fits my needs to read the GY-GPS6MV2 module with 9600 8n1
 *
 * Original Copyright:
 * Copyright (c) 2015 plieningerweb
 *
 * MIT Licensed as described in the file LICENSE
 *
 * 1 https://github.com/plieningerweb/esp8266-software-uart
 * 2 https://github.com/SuperHouse/esp-open-rtos/tree/master/extras/softuart
 */

#include "softuart.h"
#include <stdint.h>
#include "driver/gpio.h"
#include <stdio.h>

#include "esp_idf_version.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
#define esp_get_time esp_timer_get_time
#endif

#include "rom/ets_sys.h"

#define SOFTUART_DEBUG

#ifdef SOFTUART_DEBUG
#define debug(fmt, ...) printf("%s: " fmt "\n", "SOFTUART", ## __VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

extern uint32_t esp_get_time(void);

typedef struct
{
    char receive_buffer[SOFTUART_MAX_RX_BUFF];
    uint8_t receive_buffer_tail;
    uint8_t receive_buffer_head;
    uint8_t buffer_overflow;
} softuart_buffer_t;

typedef struct
{
    uint32_t rx_pin, tx_pin;
    uint32_t baudrate;
    volatile softuart_buffer_t buffer;
    uint16_t bit_time;
} softuart_t;

static softuart_t uarts[SOFTUART_MAX_UARTS] = { { 0 } };

inline static int8_t find_uart_by_rx(uint8_t rx_pin)
{
    for (uint8_t i = 0; i < SOFTUART_MAX_UARTS; i++)
        if (uarts[i].baudrate && uarts[i].rx_pin == rx_pin) return i;

    return -1;
}

// GPIO interrupt handler
static void handle_rx(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    // find uart
    int8_t uart_no = find_uart_by_rx(gpio_num);
    if (uart_no < 0) return;

    softuart_t *uart = uarts + uart_no;

    // Disable interrupt
    gpio_set_intr_type(gpio_num, GPIO_INTR_DISABLE);
    // gpio_isr_handler_remove(gpio_num);

    // Wait till start bit is half over so we can sample the next one in the center
    ets_delay_us(uart->bit_time / 2);

    // Now sample bits
    uint8_t d = 0;
    uint32_t start_time = 0x7FFFFFFF & esp_get_time();

    for (uint8_t i = 0; i < 8; i++)
    {
        while ((0x7FFFFFFF & esp_get_time()) < (start_time + (uart->bit_time * (i + 1))))
        {
            // If system timer overflow, escape from while loop
            if ((0x7FFFFFFF & esp_get_time()) < start_time)
                break;
        }
        // Shift d to the right
        d >>= 1;

        // Read bit
        if (gpio_get_level(uart->rx_pin))
        {
            // If high, set msb of 8bit to 1
            d |= 0x80;
        }
    }

    // Store byte in buffer
    // If buffer full, set the overflow flag and return
    uint8_t next = (uart->buffer.receive_buffer_tail + 1) % SOFTUART_MAX_RX_BUFF;
    if (next != uart->buffer.receive_buffer_head)
    {
        // save new data in buffer: tail points to where byte goes
        uart->buffer.receive_buffer[uart->buffer.receive_buffer_tail] = d; // save new byte
        uart->buffer.receive_buffer_tail = next;
    }
    else
    {
        uart->buffer.buffer_overflow = 1;
    }

    // Wait for stop bit
    ets_delay_us(uart->bit_time);

    // Done, reenable interrupt
    gpio_set_intr_type(uart->rx_pin, GPIO_INTR_NEGEDGE);
    // gpio_isr_handler_add(uart->rx_pin, handle_rx, (void *)(uart->rx_pin));
}

static bool check_uart_no(uint8_t uart_no)
{
    if (uart_no >= SOFTUART_MAX_UARTS)
    {
        debug("Invalid uart number %d, %d max", uart_no, SOFTUART_MAX_UARTS);
        return false;
    }

    return true;
}

static bool check_uart_enabled(uint8_t uart_no)
{
    if (!uarts[uart_no].baudrate)
    {
        debug("Uart %d is disabled", uart_no);
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
/// Public
///////////////////////////////////////////////////////////////////////////////

bool softuart_open(uint8_t uart_no, uint32_t baudrate, uint32_t rx_pin, uint32_t tx_pin)
{
    // do some checks
    if (!check_uart_no(uart_no)) return false;
    if (baudrate == 0)
    {
        debug("Invalid baudrate");
        return false;
    }
    for (uint8_t i = 0; i < SOFTUART_MAX_UARTS; i++)
        if (uarts[i].baudrate && i != uart_no
            && (uarts[i].rx_pin == rx_pin || uarts[i].tx_pin == tx_pin || uarts[i].rx_pin == tx_pin || uarts[i].tx_pin == rx_pin))
        {
            debug("Cannot share pins between uarts");
            return false;
        }

    softuart_close(uart_no);

    softuart_t *uart = uarts + uart_no;

    uart->baudrate = baudrate;
    uart->rx_pin = rx_pin;
    uart->tx_pin = tx_pin;

    // Calculate bit_time
    uart->bit_time = (1000000 / baudrate);
    if (((100000000 / baudrate) - (100 * uart->bit_time)) > 50) uart->bit_time++;

    // Setup Rx
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL<<rx_pin;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // gpio_enable(rx_pin, GPIO_MODE_INPUT);
    // gpio_pullup_en(rx_pin);

    // Setup Tx
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL<<tx_pin;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // gpio_enable(tx_pin, GPIO_MODE_OUTPUT);
    // gpio_pullup_en(tx_pin);
    gpio_set_level(tx_pin, 1);

    //install gpio isr service
    gpio_install_isr_service(0);
    // Setup the interrupt handler to get the start bit
    gpio_set_intr_type(rx_pin, GPIO_INTR_NEGEDGE);
    gpio_isr_handler_add(rx_pin, handle_rx, (void *)rx_pin);

    ets_delay_us(1000); // TODO: not sure if it really needed

    return true;
}

bool softuart_close(uint8_t uart_no)
{
    if (!check_uart_no(uart_no)) return false;
    softuart_t *uart = uarts + uart_no;

    if (!uart->baudrate) return true;

    // Remove interrupt
    gpio_set_intr_type(uart->rx_pin, GPIO_INTR_DISABLE);
    // Mark as unused
    uart->baudrate = 0;
    gpio_uninstall_isr_service();

    return true;
}

bool softuart_put(uint8_t uart_no, char c)
{
    if (!check_uart_no(uart_no)) return false;
    if (!check_uart_enabled(uart_no)) return false;
    softuart_t *uart = uarts + uart_no;

    uint32_t start_time = 0x7FFFFFFF & esp_get_time();
    gpio_set_level(uart->tx_pin, 0);

    for (uint8_t i = 0; i <= 8; i++)
    {
        while ((0x7FFFFFFF & esp_get_time()) < (start_time + (uart->bit_time * (i + 1))))
        {
            if ((0x7FFFFFFF & esp_get_time()) < start_time)
                break;
        }
        gpio_set_level(uart->tx_pin, c & (1 << i));
    }

    while ((0x7FFFFFFF & esp_get_time()) < (start_time + (uart->bit_time * 9)))
    {
        if ((0x7FFFFFFF & esp_get_time()) < start_time)
            break;
    }
    gpio_set_level(uart->tx_pin, 1);
    ets_delay_us(uart->bit_time * 6);

    return true;
}

bool softuart_puts(uint8_t uart_no, const char *s)
{
    while (*s)
    {
        if (!softuart_put(uart_no, *s++))
            return false;
    }

    return true;
}

bool softuart_available(uint8_t uart_no)
{
    if (!check_uart_no(uart_no)) return false;
    if (!check_uart_enabled(uart_no)) return false;
    softuart_t *uart = uarts + uart_no;

    return (uart->buffer.receive_buffer_tail + SOFTUART_MAX_RX_BUFF - uart->buffer.receive_buffer_head) % SOFTUART_MAX_RX_BUFF;
}

uint8_t softuart_read(uint8_t uart_no)
{
    if (!check_uart_no(uart_no)) return 0;
    if (!check_uart_enabled(uart_no)) return 0;
    softuart_t *uart = uarts + uart_no;

    // Empty buffer?
    if (uart->buffer.receive_buffer_head == uart->buffer.receive_buffer_tail) return 0;

    // Read from "head"
    uint8_t d = uart->buffer.receive_buffer[uart->buffer.receive_buffer_head]; // grab next byte
    uart->buffer.receive_buffer_head = (uart->buffer.receive_buffer_head + 1) % SOFTUART_MAX_RX_BUFF;
    return d;
}
