################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/dali_master.c \
../Src/dali_master_hal.c \
../Src/defaultFonts.c \
../Src/dht22multi.c \
../Src/gfx.c \
../Src/homecontrol.c \
../Src/ili9341.c \
../Src/ili9341_hal.c \
../Src/main.c \
../Src/pid.c \
../Src/pwmdma.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_it.c \
../Src/system_stm32f4xx.c \
../Src/terminal.c 

OBJS += \
./Src/dali_master.o \
./Src/dali_master_hal.o \
./Src/defaultFonts.o \
./Src/dht22multi.o \
./Src/gfx.o \
./Src/homecontrol.o \
./Src/ili9341.o \
./Src/ili9341_hal.o \
./Src/main.o \
./Src/pid.o \
./Src/pwmdma.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_it.o \
./Src/system_stm32f4xx.o \
./Src/terminal.o 

C_DEPS += \
./Src/dali_master.d \
./Src/dali_master_hal.d \
./Src/defaultFonts.d \
./Src/dht22multi.d \
./Src/gfx.d \
./Src/homecontrol.d \
./Src/ili9341.d \
./Src/ili9341_hal.d \
./Src/main.d \
./Src/pid.d \
./Src/pwmdma.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_it.d \
./Src/system_stm32f4xx.d \
./Src/terminal.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DUSE_HAL_DRIVER -DSTM32F407xx -DUSE_USB_FS -DHAL_I2S_MODULE_ENABLED -I"C:\stm32ws/_rep/cmsis407" -I"C:/stm32ws/407homeautomation/inc" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Drivers\BSP\STM32F4-Discovery" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Middlewares\ST\STM32_USB_Device_Library\Core\Inc" -I"C:\stm32tc\STM32CubeMX\STM32Cube_FW_F4_V1.3.0\Drivers\STM32F4xx_HAL_Driver\Inc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


