################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../Device_Startup/


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../clocks.c \
../Device_Startup/startup_samd21.c \
../Device_Startup/system_samd21.c \
../globalfunctions.c \
../i2c.c \
../main.c \
../nvmctrl.c \
../usart.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
clocks.o \
Device_Startup/startup_samd21.o \
Device_Startup/system_samd21.o \
globalfunctions.o \
i2c.o \
main.o \
nvmctrl.o \
usart.o

OBJS_AS_ARGS +=  \
clocks.o \
Device_Startup/startup_samd21.o \
Device_Startup/system_samd21.o \
globalfunctions.o \
i2c.o \
main.o \
nvmctrl.o \
usart.o

C_DEPS +=  \
clocks.d \
Device_Startup/startup_samd21.d \
Device_Startup/system_samd21.d \
globalfunctions.d \
i2c.d \
main.d \
nvmctrl.d \
usart.d

C_DEPS_AS_ARGS +=  \
clocks.d \
Device_Startup/startup_samd21.d \
Device_Startup/system_samd21.d \
globalfunctions.d \
i2c.d \
main.d \
nvmctrl.d \
usart.d

OUTPUT_FILE_PATH +=SamD21J18A-MNT.elf

OUTPUT_FILE_PATH_AS_ARGS +=SamD21J18A-MNT.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+=  \
../Device_Startup/samd21e18a_flash.ld \
../Device_Startup/samd21e18a_sram.ld


# AVR32/GNU C Compiler

















./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 5.3.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-gcc.exe$(QUOTE)  -x c -mthumb -D__SAMD21E18A__ -DDEBUG  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAMD21_DFP\1.0.231\samd21a\include" -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\arm\cmsis\4.2.0\CMSIS\Include"  -O1 -ffunction-sections -mlong-calls -g3 -Wall -mcpu=cortex-m0plus -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

Device_Startup/%.o: ../Device_Startup/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 5.3.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-gcc.exe$(QUOTE)  -x c -mthumb -D__SAMD21E18A__ -DDEBUG  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAMD21_DFP\1.0.231\samd21a\include" -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\arm\cmsis\4.2.0\CMSIS\Include"  -O1 -ffunction-sections -mlong-calls -g3 -Wall -mcpu=cortex-m0plus -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: ARM/GNU Linker : 5.3.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -mthumb -Wl,-Map="SamD21J18A-MNT.map" -Wl,--start-group -lm  -Wl,--end-group -L"..\\Device_Startup"  -Wl,--gc-sections -mcpu=cortex-m0plus -Tsamd21e18a_flash.ld  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -O binary "SamD21J18A-MNT.elf" "SamD21J18A-MNT.bin"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "SamD21J18A-MNT.elf" "SamD21J18A-MNT.hex"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O binary "SamD21J18A-MNT.elf" "SamD21J18A-MNT.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objdump.exe" -h -S "SamD21J18A-MNT.elf" > "SamD21J18A-MNT.lss"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature  "SamD21J18A-MNT.elf" "SamD21J18A-MNT.srec"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\arm\arm-gnu-toolchain\bin\arm-none-eabi-size.exe" "SamD21J18A-MNT.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "SamD21J18A-MNT.elf" "SamD21J18A-MNT.a" "SamD21J18A-MNT.hex" "SamD21J18A-MNT.bin" "SamD21J18A-MNT.lss" "SamD21J18A-MNT.eep" "SamD21J18A-MNT.map" "SamD21J18A-MNT.srec"
	