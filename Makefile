PROJ_NAME = watch-firmware

###################################################
# Set toolchain
TC = D:/Compillers/GNU Tools ARM Embedded/5.3 2016q1/bin/arm-none-eabi

# Set Tools
CC      = $(TC)-gcc
AR      = $(TC)-ar
OBJCOPY = $(TC)-objcopy
OBJDUMP = $(TC)-objdump
SIZE    = $(TC)-size
RM      = D:/Compillers/WinAVR-20100110/utils/bin/rm
MKDIR   = D:/Compillers/WinAVR-20100110/utils/bin/mkdir
PR      = "C:/Program Files (x86)/STMicroelectronics/STM32 ST-LINK Utility/ST-LINK Utility/ST-LINK_CLI"

###################################################
# Set paths
BIN_PATH = Bin
OBJ_PATH = Obj

# Set Sources
SRCS = $(wildcard mx-gen/Drivers/CMSIS/Device/ST/STM32L1xx/Source/Templates/*.c) \
       $(wildcard mx-gen/Drivers/STM32L1xx_HAL_Driver/Src/*.c) \
       $(wildcard mx-gen/Src/*.c) \
	   $(wildcard *.c)

# Set Objects
OBJS = $(patsubst %,$(OBJ_PATH)/%,$(SRCS:.c=.o)) \
       $(OBJ_PATH)/mx-gen/Drivers/CMSIS/Device/ST/STM32L1xx/Source/Templates/gcc/startup_stm32l151xb.o

# Set Include Paths
INCLUDES = -Imx-gen/Drivers/CMSIS/Device/ST/STM32L1xx/Include/ \
           -Imx-gen/Drivers/CMSIS/Include/ \
           -Imx-gen/Drivers/STM32L1xx_HAL_Driver/Inc/ \
           -Imx-gen/Inc/

# Set Libraries
LIBS  = -lm -lc

# Set Binaries
ELF = $(BIN_PATH)/$(PROJ_NAME).elf
MAP = $(BIN_PATH)/$(PROJ_NAME).map
BIN = $(BIN_PATH)/$(PROJ_NAME).bin
LST = $(BIN_PATH)/$(PROJ_NAME).lst
HEX = $(BIN_PATH)/$(PROJ_NAME).hex

###################################################
# Set Board
MCU      = -mthumb -mcpu=cortex-m3
FPU      = -mfloat-abi=soft
DEFINES  = -DSTM32L151xB -DUSE_HAL_DRIVER

LDFILE   = -Tmx-gen/TrueSTUDIO/mx-gen/STM32L151CB_FLASH.ld

# Set Compilation and Linking Flags
# -Wall						: All warnings
# -Wextra					: Extra warnings
# -std=gnu90				: GNU90 standard
# -ffunction-sections		: Place functions into it's own section
# -fdata-sections			: Place data into it's own section
# -Os						: Optimize for size
# -O0						: Don't optimize
# -g						: Produce debugging information
CFLAGS = $(MCU) $(FPU) $(DEFINES) $(INCLUDES) -Wall -std=gnu90 \
         -ffunction-sections -fdata-sections -Os

# -Wa,--warn 				: Don't suppress warning messages
# -x assembler-with-cpp		: Language select - assembly language
# -g						: Produce debugging information
ASFLAGS = $(MCU) $(FPU) -Wa,--warn -x assembler-with-cpp

# -Tx.ld 					: Use custom linker script x.ld
# -Xlinker --gc-sections	: Dead code elimination, only for sectioned objects
# -Wl,-Map=m.map			: Use custom map file m.map
# -g 						: Produce debugging information
# -gdwarf-2					: Using DWARF-2 debug format
LDFLAGS = $(MCU) $(FPU) $(LDFILE) -Xlinker --gc-sections -Wl,-Map=$(MAP) $(LIBS)

###################################################
all: $(BIN) info

dirs:
	$(MKDIR) -p $(BIN_PATH)
	$(MKDIR) -p $(OBJ_PATH)
	$(MKDIR) -p $(OBJ_PATH)/mx-gen/Drivers/CMSIS/Device/ST/STM32L1xx/Source/Templates
	$(MKDIR) -p $(OBJ_PATH)/mx-gen/Drivers/CMSIS/Device/ST/STM32L1xx/Source/Templates/gcc
	$(MKDIR) -p $(OBJ_PATH)/mx-gen/Drivers/STM32L1xx_HAL_Driver/Src
	$(MKDIR) -p $(OBJ_PATH)/mx-gen/Src

$(OBJ_PATH)/%.o : %.c Makefile
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo $@

$(OBJ_PATH)/%.o : %.s Makefile
	@$(CC) $(ASFLAGS) -c $< -o $@
	@echo $@

$(ELF): $(OBJS)
	@$(CC) $(LDFLAGS) $^ -o $@
	@echo $@

$(BIN): $(ELF)
	@$(OBJCOPY) -O binary $^ $@
	@echo $@

$(HEX): $(ELF)
	@$(OBJCOPY) -O ihex $^ $@
	@echo $@

$(LST): $(ELF)
	@$(OBJDUMP) -h -S $^ > $@
	@echo $@

info: $(ELF)
	@$(SIZE) --format=berkeley $^

cleanall: clean

clean:
	$(RM) -f $(OBJS)
	$(RM) -f $(ELF)
	$(RM) -f $(BIN)
	$(RM) -f $(MAP)

flash: $(BIN)
# -c		: Connect to first target
# -Rst		: Reset
# -HardRst	: Hardware reset
# -Run		: Start program from 0x08000000 address
# -Halt		: Halt the core
# -Step		: Step instruction
# -SetBP	: Set software or hardware breakpoint to address 0x08000000
# -ClrBP	: Clear all hardware breakpoints
# -CoreReg	: Read core registers
# -SCore	: Get core status
# -ME		: Full chip erase
# -P		: Load binary into device to address 0x00000000
# -V		: Verify while programming
# -TVolt	: Get voltage
	$(PR) -P $(BIN) 0x08000000 -V -Rst
