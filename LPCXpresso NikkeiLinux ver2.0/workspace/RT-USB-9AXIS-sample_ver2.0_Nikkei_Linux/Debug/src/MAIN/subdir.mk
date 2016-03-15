################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MAIN/EEPROM_24LC16BT_I.c \
../src/MAIN/MainFunction.c \
../src/MAIN/UserInterface.c \
../src/MAIN/imu.c \
../src/MAIN/main.c \
../src/MAIN/mpu9250.c \
../src/MAIN/tinyMathFunctions.c 

OBJS += \
./src/MAIN/EEPROM_24LC16BT_I.o \
./src/MAIN/MainFunction.o \
./src/MAIN/UserInterface.o \
./src/MAIN/imu.o \
./src/MAIN/main.o \
./src/MAIN/mpu9250.o \
./src/MAIN/tinyMathFunctions.o 

C_DEPS += \
./src/MAIN/EEPROM_24LC16BT_I.d \
./src/MAIN/MainFunction.d \
./src/MAIN/UserInterface.d \
./src/MAIN/imu.d \
./src/MAIN/main.d \
./src/MAIN/mpu9250.d \
./src/MAIN/tinyMathFunctions.d 


# Each subdirectory must supply rules for building sources it contributes
src/MAIN/%.o: ../src/MAIN/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSISv1p30_LPC13xx -D__LPC13XX__ -D__REDLIB__ -I"C:\Users\ryota\OneDrive\RT working\github\RT-USB-9AXIS-00\LPCXpresso Sample Program\workspace\CMSISv1p30_LPC13xx\inc" -I"C:\Users\ryota\OneDrive\RT working\github\RT-USB-9AXIS-00\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux\inc\PERIPHERAL" -I"C:\Users\ryota\OneDrive\RT working\github\RT-USB-9AXIS-00\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux\inc\USB" -I"C:\Users\ryota\OneDrive\RT working\github\RT-USB-9AXIS-00\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux\inc\OTHER" -I"C:\Users\ryota\OneDrive\RT working\github\RT-USB-9AXIS-00\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux\inc\MODE" -I"C:\Users\ryota\OneDrive\RT working\github\RT-USB-9AXIS-00\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux\inc\MAIN" -Og -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


