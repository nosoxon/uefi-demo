MODULE_NAME = boxer
MODULE_TYPE = UEFI_APPLICATION
MODULE_DIR  = .
EDK2        = /source/efi/edk2

IMAGE_ENTRY_POINT = _ModuleEntryPoint

BUILD  = $(MODULE_DIR)/build
TARGET = $(BUILD)/$(MODULE_NAME).efi
OBJS   = \
	$(BUILD)/main.o \
	$(BUILD)/uefi_interface.o \
	$(BUILD)/printf.o

QEMU_FW = /usr/share/edk2-ovmf/x64/OVMF.fd

term_bold = \e[1m$(1)\e[0m

ifeq ($(strip $(V)),)
    Q = @
    D = -@printf '$(call term_bold,%-10s) %s\n'
else
    Q =
    D = -@true
endif

# shell command macros
CP = cp -f
MV = mv -f
RM = rm -f
MD = mkdir -p
RD = rm -rf

# tools
CC      = gcc
AR      = ar
OBJCOPY = objcopy
GENFW   = $(EDK2)/BaseTools/Source/C/bin/GenFw
QEMU    = uefi-run

# flags
INC = \
	-I$(EDK2)/MdePkg/Include \
	-I$(EDK2)/MdePkg/Include/X64

CFLAGS = \
	-g -Os \
	-Wall -Werror \
	-Wno-address \
	-Wno-array-bounds \
	-fdata-sections \
	-ffunction-sections \
	-flto \
	-fpie \
	-fshort-wchar \
	-fno-asynchronous-unwind-tables \
	-fno-builtin \
	-fno-stack-protector \
	-fno-strict-aliasing \
	-fno-common \
	-m64 \
	-maccumulate-outgoing-args \
	-mcmodel=small \
	-mno-red-zone \
	-DEFIAPI='__attribute__((ms_abi))' \
	-DUSING_LTO

LDFLAGS = -Os \
	-flto \
	-nostdlib \
	-z common-page-size=0x40 \
	-Wl,-melf_x86_64 \
	-Wl,--oformat=elf64-x86-64 \
	-Wl,--nmagic \
	-Wl,--emit-relocs \
	-Wl,--gc-sections \
	-Wl,--pic-executable \
	-Wl,--whole-archive \
	-Wl,--defsym=PECOFF_HEADER_SIZE=0x228 \
	-Wl,--entry,$(IMAGE_ENTRY_POINT) \
	-Wl,--undefined=$(IMAGE_ENTRY_POINT) \
	-Wl,-Map=$(BUILD)/$(MODULE_NAME).map \
	-Wl,--script=$(EDK2)/BaseTools/Scripts/GccBase.lds \
	-Wno-error

OBJCOPYFLAGS = 
OBJCOPYFLAGS_DEBUG = \
	--add-gnu-debuglink=$(BUILD)/$(MODULE_NAME).debug

GENFW_FLAGS =



## rules
all: build

build: info dirs $(TARGET)
	-@printf '\n$(call term_bold,Build complete.)\n\n'

info:
	-@printf 'Building $(call term_bold,$(MODULE_NAME).efi)...\n\n'

dirs:
	-@$(MD) $(BUILD)

$(BUILD)/%.o : $(MODULE_DIR)/%.c
	$(Q)$(CC) $(CFLAGS) -c -o $@ $(INC) $^
	$(D) CC "$@"

$(BUILD)/$(MODULE_NAME).a : $(OBJS)
	$(Q)$(RM) $@
	$(Q)$(AR) cr $@ $^
	$(D) AR "$@"

$(BUILD)/$(MODULE_NAME).elf : $(BUILD)/$(MODULE_NAME).a
	$(Q)$(CC) -o $@ $(LDFLAGS) \
		-Wl,--start-group,$^,--end-group \
		$(CFLAGS)
	$(D) LD "$@"
#	$(Q)$(OBJCOPY) $(OBJCOPYFLAGS) $@
#	$(D) "OBJCOPY" "$@"

$(BUILD)/$(MODULE_NAME).efi : $(BUILD)/$(MODULE_NAME).elf
	$(Q)$(CP) $^ $(BUILD)/$(MODULE_NAME).debug
	$(Q)$(OBJCOPY) --strip-unneeded -R .eh_frame $^
	$(D) OBJCOPY "$^"
	-$(Q)$(OBJCOPY) $(OBJCOPYFLAGS_DEBUG) $^
	$(Q)$(GENFW) -e $(MODULE_TYPE) -o $@ $^ $(GENFW_FLAGS)
	$(D) GENFW  "$@"

clean:
	$(Q)$(RD) build
	$(D) RM build

run: build
	$(Q)$(QEMU) --bios $(QEMU_FW) $(TARGET)
	$(D) RUN "$(TARGET)"
