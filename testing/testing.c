#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

// for DFU bootrom reactivation
#include <libopencm3/stm32/flash.h>
#include <libopencmsis/core_cm3.h>

static void __attribute__( (section(".data#"), long_call, noinline) ) erase0_ram_func(void) {   /* extra # after section name mutes the asm warning m) */
	uint32_t reg = FLASH_CR;
	reg &= ~(FLASH_CR_PNB_MASK << FLASH_CR_PNB_SHIFT);
	reg |= (0 & FLASH_CR_PNB_MASK)  << FLASH_CR_PNB_SHIFT;
	reg |= FLASH_CR_PER;
	reg |= FLASH_CR_STRT;

	FLASH_CR = reg;

	while(FLASH_SR & FLASH_SR_BSY) {} /* busywait */

	FLASH_CR &= ~FLASH_CR_PER;

	SCB_AIRCR = SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ; /* trigger system reset via SCB */
	while(1) {}
}

static void erase_page0(uint32_t safety_key) {
	if(safety_key != 0xAA55) {
		SCB_AIRCR = SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ; /* trigger system reset via SCB */
		while(1) {}
		return;
	}
	cm_disable_interrupts();
	flash_unlock();
	flash_wait_for_last_operation();
	erase0_ram_func();
}

static void usart_init(void) {
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO2);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO2);

	rcc_periph_clock_enable(RCC_USART2);

	usart_set_baudrate(USART2, 3000000);
	usart_set_databits(USART2, 8);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_stopbits(USART2, USART_CR2_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
	usart_enable(USART2);
}

static void u2tx(const char *s) {
	for(;*s;s++)
		usart_send_blocking(USART2, *s);
	usart_send_blocking(USART2, '\r');
	usart_send_blocking(USART2, '\n');
}

int main(void) {
	/* switch from 12MHz (div=4, default) to 48MHz (div=1) */
	rcc_clock_setup(&rcc_clock_config[RCC_CLOCK_CONFIG_HSI_48MHZ]);

	usart_init();
	u2tx("HENLO!11\r\n");

	/* blinkenlight and DFU button */
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO9);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_LOW, GPIO8);
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);

	while(1) {
		for(uint32_t v=1000000;v;v--)
			__asm__("nop");
		gpio_toggle(GPIOB, GPIO8);
		if(gpio_get(GPIOB, GPIO9))
			erase_page0(0xAA55);
	}
	return 0;
}
