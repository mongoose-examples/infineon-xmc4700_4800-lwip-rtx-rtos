PROG = firmware

PROJECT_ROOT_PATH = $(realpath $(CURDIR)/../..)
DOCKER ?= docker run --rm -v $(PROJECT_ROOT_PATH):$(PROJECT_ROOT_PATH) -w $(CURDIR) mdashnet/armgcc

CFLAGS = -MMD -MT -O0 -DXMC4700_F144x2048 -DMG_ARCH=MG_ARCH_CMSIS_RTOS1 -DMG_ENABLE_LWIP=1 -DMAKE_BUILD -ffunction-sections -fdata-sections -fno-common -std=gnu99 -Wno-attributes -mfloat-abi=softfp -pipe -c -fmessage-length=0 -MMD -MP -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -g -gdwarf-2

LINKFLAGS = -T"linker_script.ld" -nostartfiles -Xlinker --gc-sections -specs=nano.specs -specs=nosys.specs -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -lm

SOURCES = $(shell find $(CURDIR) -type f -name '*.c' -not -path "*/Templates/*")
SOURCES_S = $(shell find $(CURDIR) -type f -name '*.S' -not -path "*/Templates/*")

OBJECTS = $(SOURCES:%.c=build/C/%.o)
OBJECTS_S = $(SOURCES_S:%.S=build/S/%.o)

INCLUDES = $(addprefix -I, $(shell find $(CURDIR) -type d -not -name 'build' -not -name 'Debug'))

build: $(PROG).bin

$(PROG).bin: $(PROG).elf
	@$(DOCKER) arm-none-eabi-size $<
	@$(DOCKER) arm-none-eabi-objcopy -v -O binary $< $@

$(PROG).elf: $(OBJECTS) $(OBJECTS_S)
	$(info LD $@)
	@$(DOCKER) arm-none-eabi-gcc $(LINKFLAGS) $(OBJECTS) $(OBJECTS_S) -o $@

build/C/%.o: %.c
	@mkdir -p $(dir $@)
	$(info CC $<)
	@$(DOCKER) arm-none-eabi-gcc $(CFLAGS) $(INCLUDES) -c $< -o $@

build/S/%.o: %.S
	@mkdir -p $(dir $@)
	$(info ASM $<)
	@$(DOCKER) arm-none-eabi-gcc $(CFLAGS) -x assembler-with-cpp -c $< -o $@

clean:
	rm -rf build/ firmware.elf firmware.bin
