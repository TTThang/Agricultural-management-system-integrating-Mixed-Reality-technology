################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/core_riscv.c 

OBJS += \
./Core/core_riscv.o 

C_DEPS += \
./Core/core_riscv.d 


# Each subdirectory must supply rules for building sources it contributes
Core/%.o: ../Core/%.c
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Soil_sensor_board\Debug" -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Soil_sensor_board\HAL" -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Soil_sensor_board\Core" -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Soil_sensor_board\User" -I"C:\Users\thang\OneDrive\Documents\Project_final\Firmware\Sensor\Soil_sensor_board\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

