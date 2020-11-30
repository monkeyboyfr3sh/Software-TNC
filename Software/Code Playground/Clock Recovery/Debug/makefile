################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Clock\ Recovery.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Clock\ Recovery.list \

OBJCOPY_BIN += \
Clock\ Recovery.bin \


# All Target
all: Clock\ Recovery.elf secondary-outputs

# Tool invocations
Clock\ Recovery.elf: $(OBJS) $(USER_OBJS) C:\GitHub\TNCMCU\Software\Code\ Playground\Clock\ Recovery\STM32F446RETX_FLASH.ld
	arm-none-eabi-gcc -o "Clock Recovery.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\GitHub\TNCMCU\Software\Code Playground\Clock Recovery\STM32F446RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Clock Recovery.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Clock\ Recovery.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Clock Recovery.list"
	@echo 'Finished building: $@'
	@echo ' '

Clock\ Recovery.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Clock Recovery.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
