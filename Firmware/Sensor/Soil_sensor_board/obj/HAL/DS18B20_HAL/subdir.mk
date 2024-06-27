################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/DS18B20_HAL/DS18B20_HAL.c 

OBJS += \
./HAL/DS18B20_HAL/DS18B20_HAL.o 

C_DEPS += \
./HAL/DS18B20_HAL/DS18B20_HAL.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/DS18B20_HAL/%.o: ../HAL/DS18B20_HAL/%.c
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Soil_sensor_board\Debug" -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Soil_sensor_board\HAL" -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Soil_sensor_board\Core" -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Soil_sensor_board\User" -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Soil_sensor_board\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

