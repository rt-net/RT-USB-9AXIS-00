################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/USB/subdir.mk
-include src/PERIPHERAL/subdir.mk
-include src/OTHER/subdir.mk
-include src/MODE/subdir.mk
-include src/MAIN/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux.axf

# Tool invocations
RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\ryota\OneDrive\RT working\github\RT-USB-9AXIS-00\LPCXpresso Sample Program\workspace\CMSISv1p30_LPC13xx\Debug" -Xlinker -Map="RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "RT-USB-9AXIS-sample_ver2_0_Nikkei_Linux_Debug.ld" -o "RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux.axf" ; arm-none-eabi-objcopy -v -O binary "RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux.axf" "RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux.bin" ; checksum -p LPC1343 -d "RT-USB-9AXIS-sample_ver2.0_Nikkei_Linux.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
