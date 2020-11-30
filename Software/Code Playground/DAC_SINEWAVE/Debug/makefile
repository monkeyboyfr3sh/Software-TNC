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
DAC_SINEWAVE.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
DAC_SINEWAVE.list \

OBJCOPY_BIN += \
DAC_SINEWAVE.bin \


# All Target
all: DAC_SINEWAVE.elf secondary-outputs

# Tool invocations
DAC_SINEWAVE.elf: $(OBJS) $(USER_OBJS) C:\GitHub\TNCMCU\Software\Code\ Playground\DAC_SINEWAVE\STM32F446RETX_FLASH.ld
	arm-none-eabi-gcc -o "DAC_SINEWAVE.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\GitHub\TNCMCU\Software\Code Playground\DAC_SINEWAVE\STM32F446RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="DAC_SINEWAVE.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

DAC_SINEWAVE.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "DAC_SINEWAVE.list"
	@echo 'Finished building: $@'
	@echo ' '

DAC_SINEWAVE.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "DAC_SINEWAVE.bin"
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
