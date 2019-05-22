################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Atimer.cpp \
../src/Console.cpp \
../src/Dac.cpp \
../src/Eeprom.cpp \
../src/FramCy15b104q.cpp \
../src/GptRtTimer.cpp \
../src/GptVoidTimer.cpp \
../src/GroupPinInterrupt.cpp \
../src/I2cMaster.cpp \
../src/Ipc.cpp \
../src/IrqController.cpp \
../src/JbControllerM0.cpp \
../src/JbControllerM4.cpp \
../src/NvmParameters.cpp \
../src/PinInterrupt.cpp \
../src/PwmVoidTimer.cpp \
../src/RiVoidTimer.cpp \
../src/SctVoidTimer.cpp \
../src/SgpioSpiMaster.cpp \
../src/Spifi.cpp \
../src/Ssp.cpp \
../src/SysTickVoidTimer.cpp \
../src/Uart.cpp \
../src/Wdt.cpp \
../src/irq_handlers_m0app.cpp \
../src/irq_handlers_m0sub.cpp \
../src/irq_handlers_m4.cpp 

OBJS += \
./src/Atimer.o \
./src/Console.o \
./src/Dac.o \
./src/Eeprom.o \
./src/FramCy15b104q.o \
./src/GptRtTimer.o \
./src/GptVoidTimer.o \
./src/GroupPinInterrupt.o \
./src/I2cMaster.o \
./src/Ipc.o \
./src/IrqController.o \
./src/JbControllerM0.o \
./src/JbControllerM4.o \
./src/NvmParameters.o \
./src/PinInterrupt.o \
./src/PwmVoidTimer.o \
./src/RiVoidTimer.o \
./src/SctVoidTimer.o \
./src/SgpioSpiMaster.o \
./src/Spifi.o \
./src/Ssp.o \
./src/SysTickVoidTimer.o \
./src/Uart.o \
./src/Wdt.o \
./src/irq_handlers_m0app.o \
./src/irq_handlers_m0sub.o \
./src/irq_handlers_m4.o 

CPP_DEPS += \
./src/Atimer.d \
./src/Console.d \
./src/Dac.d \
./src/Eeprom.d \
./src/FramCy15b104q.d \
./src/GptRtTimer.d \
./src/GptVoidTimer.d \
./src/GroupPinInterrupt.d \
./src/I2cMaster.d \
./src/Ipc.d \
./src/IrqController.d \
./src/JbControllerM0.d \
./src/JbControllerM4.d \
./src/NvmParameters.d \
./src/PinInterrupt.d \
./src/PwmVoidTimer.d \
./src/RiVoidTimer.d \
./src/SctVoidTimer.d \
./src/SgpioSpiMaster.d \
./src/Spifi.d \
./src/Ssp.d \
./src/SysTickVoidTimer.d \
./src/Uart.d \
./src/Wdt.d \
./src/irq_handlers_m0app.d \
./src/irq_handlers_m0sub.d \
./src/irq_handlers_m4.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -DDEBUG -D__CODE_RED -D__NEWLIB__ -D__LPC43XX__ -DCORE_M0 -DCORE_M0SUB -I"H:\JB_Lib\LPC43XX\LPC43XX_JB_Drivers\include" -I"H:\JB_Lib\LPC43XX\LPC43XX_Open_chip\include" -I"H:\JB_Lib\Platform_Abstract\JB_Kernel\include" -I"H:\JB_Lib\LPC43XX\MCUXWorkspace\include" -I"H:\JB_Lib\Platform_Abstract\JB_Utilities\include" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fsingle-precision-constant -mcpu=cortex-m0 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


