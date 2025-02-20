PROJECT = c0-testing
BUILD_DIR = bin

#SHARED_DIR = ../my-common-code
CFILES = testing.c
#CFILES += api.c
#AFILES += api-asm.S

DEVICE=stm32f042k6t6
#DEVICE=stm32c071kbt6
#OOCD_FILE = board/stm32f4discovery.cfg

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

all: $(PROJECT).bin
	$(PREFIX)size $(PROJECT).elf

load: $(PROJECT).bin
	(echo ICANHAZBOOTLOADER > /dev/ttyACM0 && sleep 2) || true
	dfu-util -a 0 -s 0x08000000:leave -D $(PROJECT).bin
