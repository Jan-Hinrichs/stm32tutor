################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.3.0/Drivers/BSP/STM32F4-Discovery/stm32f4_discovery.c \
C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.3.0/Drivers/BSP/STM32F4-Discovery/stm32f4_discovery_accelerometer.c 

OBJS += \
./STM32F4-Discovery/stm32f4_discovery.o \
./STM32F4-Discovery/stm32f4_discovery_accelerometer.o 

C_DEPS += \
./STM32F4-Discovery/stm32f4_discovery.d \
./STM32F4-Discovery/stm32f4_discovery_accelerometer.d 


# Each subdirectory must supply rules for building sources it contributes
STM32F4-Discovery/stm32f4_discovery.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.3.0/Drivers/BSP/STM32F4-Discovery/stm32f4_discovery.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DUSE_HAL_DRIVER -DSTM32F407xx -DUSE_USB_FS -DHAL_I2S_MODULE_ENABLED -I"C:\stm32ws/_rep/cmsis407" -I"C:/stm32ws/407homeautomation/inc" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Drivers\BSP\STM32F4-Discovery" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Middlewares\ST\STM32_USB_Device_Library\Core\Inc" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Drivers\STM32F4xx_HAL_Driver\Inc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

STM32F4-Discovery/stm32f4_discovery_accelerometer.o: C:/stm32tc/STM32CubeMX/STM32Cube_FW_F4_V1.3.0/Drivers/BSP/STM32F4-Discovery/stm32f4_discovery_accelerometer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DUSE_HAL_DRIVER -DSTM32F407xx -DUSE_USB_FS -DHAL_I2S_MODULE_ENABLED -I"C:\stm32ws/_rep/cmsis407" -I"C:/stm32ws/407homeautomation/inc" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Drivers\BSP\STM32F4-Discovery" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Middlewares\ST\STM32_USB_Device_Library\Core\Inc" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Drivers\STM32F4xx_HAL_Driver\Inc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


