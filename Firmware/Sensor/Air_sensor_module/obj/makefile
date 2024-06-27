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
-include HAL/SCD4X_HAL/subdir.mk
-include HAL/RS485_SLAVE_HAL/subdir.mk
-include HAL/RS485_HAL/subdir.mk
-include HAL/I2C_HAL/subdir.mk
-include HAL/BH1750_HAL/subdir.mk
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
Air_sensor_module.hex \

SECONDARY_LIST += \
Air_sensor_module.lst \

SECONDARY_SIZE += \
Air_sensor_module.siz \


# All Target
all: Air_sensor_module.elf secondary-outputs

# Tool invocations
Air_sensor_module.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -T "C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Air_sensor_module\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"Air_sensor_module.map" --specs=nano.specs --specs=nosys.specs -o "Air_sensor_module.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lprintf
	@	@
Air_sensor_module.hex: Air_sensor_module.elf
	@	riscv-none-embed-objcopy -O ihex "Air_sensor_module.elf"  "Air_sensor_module.hex"
	@	@
Air_sensor_module.lst: Air_sensor_module.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "Air_sensor_module.elf" > "Air_sensor_module.lst"
	@	@
Air_sensor_module.siz: Air_sensor_module.elf
	@	riscv-none-embed-size --format=berkeley "Air_sensor_module.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) Air_sensor_module.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
