################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/BH1750_HAL/BH1750_HAL.c 

OBJS += \
./HAL/BH1750_HAL/BH1750_HAL.o 

C_DEPS += \
./HAL/BH1750_HAL/BH1750_HAL.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/BH1750_HAL/%.o: ../HAL/BH1750_HAL/%.c
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Air_sensor_module\Debug" -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Air_sensor_module\HAL" -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Air_sensor_module\Core" -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Air_sensor_module\User" -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Air_sensor_module\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

