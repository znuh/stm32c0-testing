#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

int main(void) {
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_LOW, GPIO13);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
	while(1) {
		for(uint32_t v=100000;v;v--) {}
		gpio_toggle(GPIOA, GPIO13);	
	}
	return 0;
}
