################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
C:/stm32ws/_rep/cmsis407/startup_stm32f407xx.asm 

C_SRCS += \
C:/stm32ws/_rep/cmsis407/syscalls.c 

OBJS += \
./cmsis407/startup_stm32f407xx.o \
./cmsis407/syscalls.o 

ASM_DEPS += \
./cmsis407/startup_stm32f407xx.d 

C_DEPS += \
./cmsis407/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
cmsis407/startup_stm32f407xx.o: C:/stm32ws/_rep/cmsis407/startup_stm32f407xx.asm
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -DUSE_HAL_DRIVER -DSTM32F407xx -DUSE_USB_FS -DHAL_I2S_MODULE_ENABLED -I"C:\stm32ws/_rep/cmsis407" -I"C:/stm32ws/407homeautomation/inc" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Drivers\BSP\STM32F4-Discovery" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Middlewares\ST\STM32_USB_Device_Library\Core\Inc" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Drivers\STM32F4xx_HAL_Driver\Inc" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cmsis407/syscalls.o: C:/stm32ws/_rep/cmsis407/syscalls.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DUSE_HAL_DRIVER -DSTM32F407xx -DUSE_USB_FS -DHAL_I2S_MODULE_ENABLED -I"C:\stm32ws/_rep/cmsis407" -I"C:/stm32ws/407homeautomation/inc" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Drivers\BSP\STM32F4-Discovery" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Middlewares\ST\STM32_USB_Device_Library\Core\Inc" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Drivers\STM32F4xx_HAL_Driver\Inc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


