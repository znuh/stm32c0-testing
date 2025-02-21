#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

int main(void) {
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_LOW, GPIO8);
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
	while(1) {
		for(uint32_t v=100000;v;v--) {}
		gpio_toggle(GPIOB, GPIO8);
	}
	return 0;
}
