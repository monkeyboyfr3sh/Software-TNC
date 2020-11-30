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
FreqCount_CtrTech1.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
FreqCount_CtrTech1.list \

OBJCOPY_BIN += \
FreqCount_CtrTech1.bin \


# All Target
all: FreqCount_CtrTech1.elf secondary-outputs

# Tool invocations
FreqCount_CtrTech1.elf: $(OBJS) $(USER_OBJS) C:\Users\13372\Documents\GitHub\TNCMCU\Software\Code\ Playground\FreqCount_CtrTech1\STM32F446RETX_FLASH.ld
	arm-none-eabi-gcc -o "FreqCount_CtrTech1.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\13372\Documents\GitHub\TNCMCU\Software\Code Playground\FreqCount_CtrTech1\STM32F446RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="FreqCount_CtrTech1.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

FreqCount_CtrTech1.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "FreqCount_CtrTech1.list"
	@echo 'Finished building: $@'
	@echo ' '

FreqCount_CtrTech1.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "FreqCount_CtrTech1.bin"
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
