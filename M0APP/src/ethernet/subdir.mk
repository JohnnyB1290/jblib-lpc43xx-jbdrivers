################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ethernet/EthernetPhy.cpp 

OBJS += \
./src/ethernet/EthernetPhy.o 

CPP_DEPS += \
./src/ethernet/EthernetPhy.d 


# Each subdirectory must supply rules for building sources it contributes
src/ethernet/%.o: ../src/ethernet/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -DDEBUG -D__CODE_RED -D__NEWLIB__ -D__LPC43XX__ -DCORE_M0 -DCORE_M0APP -I"H:\JB_Lib\LPC43XX\LPC43XX_JB_Drivers\include" -I"H:\JB_Lib\LPC43XX\LPC43XX_Open_chip\include" -I"H:\JB_Lib\Platform_Abstract\JB_Kernel\include" -I"H:\JB_Lib\LPC43XX\MCUXWorkspace\include" -I"H:\JB_Lib\Platform_Abstract\JB_Utilities\include" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fsingle-precision-constant -mcpu=cortex-m0 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


