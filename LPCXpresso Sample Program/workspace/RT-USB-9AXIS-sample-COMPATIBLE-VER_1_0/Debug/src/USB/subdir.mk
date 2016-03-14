################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/USB/cdcuser.c \
../src/USB/usbcore.c \
../src/USB/usbdesc.c \
../src/USB/usbhw.c \
../src/USB/usbuser.c 

OBJS += \
./src/USB/cdcuser.o \
./src/USB/usbcore.o \
./src/USB/usbdesc.o \
./src/USB/usbhw.o \
./src/USB/usbuser.o 

C_DEPS += \
./src/USB/cdcuser.d \
./src/USB/usbcore.d \
./src/USB/usbdesc.d \
./src/USB/usbhw.d \
./src/USB/usbuser.d 


# Each subdirectory must supply rules for building sources it contributes
src/USB/%.o: ../src/USB/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSISv1p30_LPC13xx -D__LPC13XX__ -D__REDLIB__ -I"C:\Users\ryota\OneDrive\RT working\github\RT-USB-9AXIS-00\LPCXpresso Sample Program\workspace\CMSISv1p30_LPC13xx\inc" -I"C:\Users\ryota\OneDrive\RT working\github\RT-USB-9AXIS-00\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample\inc\PERIPHERAL" -I"C:\Users\ryota\OneDrive\RT working\github\RT-USB-9AXIS-00\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample\inc\USB" -I"C:\Users\ryota\OneDrive\RT working\github\RT-USB-9AXIS-00\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample\inc\OTHER" -I"C:\Users\ryota\OneDrive\RT working\github\RT-USB-9AXIS-00\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample\inc\MODE" -I"C:\Users\ryota\OneDrive\RT working\github\RT-USB-9AXIS-00\LPCXpresso Sample Program\workspace\RT-USB-9AXIS-sample\inc\MAIN" -Og -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


