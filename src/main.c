#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include <stdio.h>


#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(autosar_os, LOG_LEVEL_DBG);


///////////////////////////////////////////////////////////////////////////////
// Macros
#define OS_TICK_MS	(1) /* ms per tick */
#define LED_ONTIME_MS	(150)
#define LED_GPIO25	DT_ALIAS(led0)



///////////////////////////////////////////////////////////////////////////////
// Globals
static const struct gpio_dt_spec LedStruct = GPIO_DT_SPEC_GET(LED_GPIO25, gpios);
static struct k_timer OsTickTimer;


extern void Lin_Init(void);

///////////////////////////////////////////////////////////////////////////////
// Functions
static void os_ticks(struct k_timer *timer)
{
	static int count = (LED_ONTIME_MS/OS_TICK_MS);

	if (--count == 0) {
		gpio_pin_toggle_dt(&LedStruct);
		count = (LED_ONTIME_MS/OS_TICK_MS);
		Lin_Init();
	}
}


int main(void)
{
	int ret;
	LOG_DBG("Welcome to CAR-OS (Zephyr)!");


	if (!gpio_is_ready_dt(&LedStruct)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&LedStruct, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	k_timer_init(&OsTickTimer, os_ticks, NULL);
	k_timer_start(&OsTickTimer, K_MSEC(OS_TICK_MS), K_MSEC(OS_TICK_MS));

	Lin_Init();

	while (1) {
		k_msleep(5000);
		LOG_DBG("Background thread woke-up!");
	}

	return 0;
}