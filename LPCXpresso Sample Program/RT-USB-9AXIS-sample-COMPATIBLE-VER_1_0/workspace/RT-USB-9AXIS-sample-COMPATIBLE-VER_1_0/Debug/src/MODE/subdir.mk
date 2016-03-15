################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MODE/modeSelect.c \
../src/MODE/mode_calibAccXY.c \
../src/MODE/mode_calibAccZ.c \
../src/MODE/mode_calibMag.c \
../src/MODE/mode_calibOmega.c \
../src/MODE/mode_clearOffset.c \
../src/MODE/mode_viewAllData.c \
../src/MODE/mode_viewEEPROMData.c 

OBJS += \
./src/MODE/modeSelect.o \
./src/MODE/mode_calibAccXY.o \
./src/MODE/mode_calibAccZ.o \
./src/MODE/mode_calibMag.o \
./src/MODE/mode_calibOmega.o \
./src/MODE/mode_clearOffset.o \
./src/MODE/mode_viewAllData.o \
./src/MODE/mode_viewEEPROMData.o 

C_DEPS += \
./src/MODE/modeSelect.d \
./src/MODE/mode_calibAccXY.d \
./src/MODE/mode_calibAccZ.d \
./src/MODE/mode_calibMag.d \
./src/MODE/mode_calibOmega.d \
./src/MODE/mode_clearOffset.d \
./src/MODE/mode_viewAllData.d \
./src/MODE/mode_viewEEPROMData.d 


# Each subdirectory must supply rules for building sources it contributes
src/MODE/%.o: ../src/MODE/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSISv1p30_LPC13xx -D__LPC13XX__ -D__REDLIB__ -I"C:\Users\ryota\Desktop\hoge\LPCXpresso Sample Program\workspace\CMSISv1p30_LPC13xx\inc" -I"C:\Users\ryota\Desktop\hoge\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample-COMPATIBLE-VER_1_0\inc\PERIPHERAL" -I"C:\Users\ryota\Desktop\hoge\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample-COMPATIBLE-VER_1_0\inc\USB" -I"C:\Users\ryota\Desktop\hoge\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample-COMPATIBLE-VER_1_0\inc\OTHER" -I"C:\Users\ryota\Desktop\hoge\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample-COMPATIBLE-VER_1_0\inc\MODE" -I"C:\Users\ryota\Desktop\hoge\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample-COMPATIBLE-VER_1_0\inc\MAIN" -Og -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


