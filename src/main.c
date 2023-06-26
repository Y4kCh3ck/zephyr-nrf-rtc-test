#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h> 

// #define NRFX_RTC_ENABLED 1
// #define NRFX_RTC0_ENABLED 1


// static const struct device *rtc_dev =  DEVICE_DT_GET(DT_NODELABEL(rtc0));
// const struct device *rtc_dev = device_get_binding("rtc0");
// #define RTC_DEVICE_NODE DT_NODELABEL(rtc0)
// const struct device *const rtc_dev = DEVICE_DT_GET(RTC_DEVICE_NODE);
 static const struct device *const rtc_dev = DEVICE_DT_GET_ONE(nordic_nrf_clock);
// static const struct device *const rtc_dev = DEVICE_DT_GET_ONE(nrf_rtc);


int main(void)
{

	printk("Board: %s\n", CONFIG_BOARD);
	// const struct device *rtc_dev = device_get_binding("rtc0");

	int32_t calibration = 0;
	// (void) rtc_get_calibration(rtc_dev, &calibration);
	printk("Calibration: %d \r\n", calibration);


	struct rtc_time current_time;

	// Dummy data
	current_time.tm_sec		=  12;
	current_time.tm_min		=  34;
	current_time.tm_hour	=   5;
	current_time.tm_mday	=   6;
	current_time.tm_mon		=   7;
	current_time.tm_year	=  18;
	current_time.tm_wday	=   1;
	current_time.tm_yday	=  10;
	current_time.tm_isdst	=  -1;
	current_time.tm_nsec	=  10;

	printk("OK\n\r");
	int result = rtc_set_time( rtc_dev, &current_time );
	if ( 0 != result )
	{
		printk("Error setting RTC %d\n\r", result);
	}

	printk("\nEntering loop\r\n");

	while( 1 )
	{
		printk("Loop Entered\r\n");

		result = rtc_get_time( rtc_dev, &current_time );
		if( 0 == result )
		{
			printk("Time updated\r\n");
		}
		else
		{
			printk("Error updating time! %d \n\r ", result);
		}

		char time_buf[25];
		sprintf(time_buf, "Day: %d \n", current_time.tm_mday);
		printk(time_buf);
		printk("Loop done\r\n");
		printk(time_buf);
		k_msleep(1000);

	}
	return 0;
}
