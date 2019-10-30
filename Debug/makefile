################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Drivers/CMSIS/DSP/Source/SupportFunctions/subdir.mk
-include Drivers/CMSIS/DSP/Source/MatrixFunctions/subdir.mk
-include Drivers/CMSIS/DSP/Source/BasicMathFunctions/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
AI_HandwrittenDigits_Classification.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
AI_HandwrittenDigits_Classification.list \


# All Target
all: AI_HandwrittenDigits_Classification.elf secondary-outputs

# Tool invocations
AI_HandwrittenDigits_Classification.elf: $(OBJS) $(USER_OBJS) C:\Users\Prakashs\STM32CubeIDE\workspace_1.0.2\AI_HandwrittenDigits_Classification\STM32F407IGTX_FLASH.ld
	arm-none-eabi-gcc -o "AI_HandwrittenDigits_Classification.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Prakashs\STM32CubeIDE\workspace_1.0.2\AI_HandwrittenDigits_Classification\STM32F407IGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="AI_HandwrittenDigits_Classification.map" -Wl,--gc-sections -static -L../Middlewares/ST/AI/Lib --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

AI_HandwrittenDigits_Classification.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "AI_HandwrittenDigits_Classification.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
