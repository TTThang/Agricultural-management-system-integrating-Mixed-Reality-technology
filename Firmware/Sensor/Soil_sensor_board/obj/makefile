################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include HAL/RS485_SLAVE_HAL/subdir.mk
-include HAL/RS485_HAL/subdir.mk
-include HAL/ONEWIRE_HAL/subdir.mk
-include HAL/DS18B20_HAL/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
Soil_sensor_board.hex \

SECONDARY_LIST += \
Soil_sensor_board.lst \

SECONDARY_SIZE += \
Soil_sensor_board.siz \


# All Target
all: Soil_sensor_board.elf secondary-outputs

# Tool invocations
Soil_sensor_board.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -T "C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Soil_sensor_board\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"Soil_sensor_board.map" --specs=nano.specs --specs=nosys.specs -o "Soil_sensor_board.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lprintf
	@	@
Soil_sensor_board.hex: Soil_sensor_board.elf
	@	riscv-none-embed-objcopy -O ihex "Soil_sensor_board.elf"  "Soil_sensor_board.hex"
	@	@
Soil_sensor_board.lst: Soil_sensor_board.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "Soil_sensor_board.elf" > "Soil_sensor_board.lst"
	@	@
Soil_sensor_board.siz: Soil_sensor_board.elf
	@	riscv-none-embed-size --format=berkeley "Soil_sensor_board.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) Soil_sensor_board.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
