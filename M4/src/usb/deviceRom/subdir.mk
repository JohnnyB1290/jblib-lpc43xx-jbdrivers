################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/usb/deviceRom/ComUsbDeviceRom.cpp \
../src/usb/deviceRom/RndisUsbDeviceRom.cpp \
../src/usb/deviceRom/UsbDeviceRomController.cpp \
../src/usb/deviceRom/UsbDeviceRomDescriptors.cpp 

OBJS += \
./src/usb/deviceRom/ComUsbDeviceRom.o \
./src/usb/deviceRom/RndisUsbDeviceRom.o \
./src/usb/deviceRom/UsbDeviceRomController.o \
./src/usb/deviceRom/UsbDeviceRomDescriptors.o 

CPP_DEPS += \
./src/usb/deviceRom/ComUsbDeviceRom.d \
./src/usb/deviceRom/RndisUsbDeviceRom.d \
./src/usb/deviceRom/UsbDeviceRomController.d \
./src/usb/deviceRom/UsbDeviceRomDescriptors.d 


# Each subdirectory must supply rules for building sources it contributes
src/usb/deviceRom/%.o: ../src/usb/deviceRom/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -DDEBUG -D__CODE_RED -D__NEWLIB__ -D__LPC43XX__ -DCORE_M4 -I"H:\JB_Lib\LPC43XX\LPC43XX_JB_Drivers\include" -I"H:\JB_Lib\LPC43XX\LPC43XX_Open_chip\include" -I"H:\JB_Lib\Platform_Abstract\JB_Kernel\include" -I"H:\JB_Lib\LPC43XX\MCUXWorkspace\include" -I"H:\JB_Lib\Platform_Abstract\JB_Utilities\include" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


