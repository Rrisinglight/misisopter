################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/STM\ Project/Mirror5_8G/F411_HW_SPI_UART/FATFS/App/fatfs.c 

OBJS += \
./Application/User/FATFS/App/fatfs.o 

C_DEPS += \
./Application/User/FATFS/App/fatfs.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/FATFS/App/fatfs.o: D:/STM\ Project/Mirror5_8G/F411_HW_SPI_UART/FATFS/App/fatfs.c Application/User/FATFS/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../../Core/Inc -I../../FATFS/Target -I../../FATFS/App -I../../USB_DEVICE/App -I../../USB_DEVICE/Target -I../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FatFs/src -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Application/User/FATFS/App/fatfs.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-FATFS-2f-App

clean-Application-2f-User-2f-FATFS-2f-App:
	-$(RM) ./Application/User/FATFS/App/fatfs.cyclo ./Application/User/FATFS/App/fatfs.d ./Application/User/FATFS/App/fatfs.o ./Application/User/FATFS/App/fatfs.su

.PHONY: clean-Application-2f-User-2f-FATFS-2f-App

