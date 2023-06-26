/*
 * Copyright (c) 2022 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/rtc.h> 


//static const struct device *rtc_dev =  DEVICE_DT_GET(DT_NODELABEL(rtc0));
//const struct device *rtc_dev = device_get_binding("rtc0");
#define RTC_DEVICE_NODE DT_NODELABEL(rtc0)
//const struct device *const rtc_dev = DEVICE_DT_GET(RTC_DEVICE_NODE)


/* change this to any other UART peripheral if desired */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

 // static const struct device *const rtc_dev =  DEVICE_DT_GET(DT_NODE_LABEL(rtc0));

#define MSG_SIZE 32

/* queue to store up to 10 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 10, 4);

static const struct device const *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* receive buffer used in UART ISR callback */
static char rx_buf[MSG_SIZE];
static int rx_buf_pos;

/*
 * Read characters from UART until line end is detected. Afterwards push the
 * data to the message queue.
 */
void serial_cb(const struct device *dev, void *user_data)
{
	uint8_t c;

	if (!uart_irq_update(uart_dev)) {
		return;
	}

	if (!uart_irq_rx_ready(uart_dev)) {
		return;
	}

	/* read until FIFO empty */
	while (uart_fifo_read(uart_dev, &c, 1) == 1) {
		if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
			/* terminate string */
			rx_buf[rx_buf_pos] = '\0';

			/* if queue is full, message is silently dropped */
			k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rx_buf_pos = 0;
		} else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
			rx_buf[rx_buf_pos++] = c;
		}
		/* else: characters beyond buffer size are dropped */
	}
}

/*
 * Print a null-terminated string character by character to the UART interface
 */
void print_uart(char *buf)
{
	int msg_len = strlen(buf);

	for (int i = 0; i < msg_len; i++) {
		uart_poll_out(uart_dev, buf[i]);
	}
}

int main(void)
{
	print_uart("Hello, world!");
	const struct device *rtc_dev = device_get_binding("rtc0");

	char tx_buf[MSG_SIZE];

	if (!device_is_ready(uart_dev)) {
		printk("UART device not found!");
		return 0;
	}

	/* configure interrupt and callback to receive data */
	int ret = uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);

	if (ret < 0) {
		if (ret == -ENOTSUP) {
			printk("Interrupt-driven UART API support not enabled\n");
		} else if (ret == -ENOSYS) {
			printk("UART device does not support interrupt-driven API\n");
		} else {
			printk("Error setting UART callback: %d\n", ret);
		}
		return 0;
	}
	uart_irq_rx_enable(uart_dev);

	print_uart("Seting RTC\r\n");
	//rtc_set_calibration(rtc_dev, 100);
	
	
	int32_t calibration = 0;
	// (void) rtc_get_calibration(rtc_dev, &calibration);


	char calibration_buf[30];
	sprintf(calibration_buf, "Calibration: %d \r\n", calibration);
	print_uart(calibration_buf);

	struct rtc_time current_time;

	current_time.tm_sec		=12;
	current_time.tm_min		=34;
	current_time.tm_hour	=5;
	current_time.tm_mday	=6;
	current_time.tm_mon		=7;
	current_time.tm_year	=2008;
	current_time.tm_wday	=1;
	current_time.tm_yday	=10;
	current_time.tm_isdst	=1;
	current_time.tm_nsec	=10;
	
	print_uart("OK\n\r");
	int result = 0;
	// if ( result != rtc_set_time( rtc_dev, &current_time ) )
	// {
	// 	print_uart("Error setting RTC\r\n");
	// }
	print_uart(result);

	print_uart("Entering loop\r\n");
	
	while( 1 )
	{
		print_uart("Loop Entered\r\n");
		

		if( 0 == rtc_get_time(rtc_dev, &current_time) ) 
		{
			print_uart("Time updated\r\n");
		}
		else 
		{
			print_uart("Error updating time!\r\n");
		}

		char time_buf[25];
		sprintf(time_buf, "Day: %d \n", current_time.tm_mday);
		print_uart(time_buf);
		print_uart("Loop done\r\n");
		// printk(time_buf);
		k_msleep(1000);

	}
	return 0;
}
