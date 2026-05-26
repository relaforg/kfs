ARCH ?= x86
ARCH_DIR := src/arch/$(ARCH)
SYSROOT := $(PWD)/sysroot
CC := i686-elf-gcc
ASM = i686-elf-as
NAME := kfs
BUILD_DIR := .build
CFLAGS = --sysroot=$(SYSROOT) -std=gnu99 -ffreestanding -O2 \
		 -Wall -Wextra -MD \
		 -isystem $(SYSROOT)/usr/include \
		 -Iincludes
LINKER_FLAGS = -ffreestanding -O2 -nostdlib -lgcc
MKRESCUE_PATH ?= ~/.local/bin/grub-mkrescue
GRUB_PATH ?= ~/.local/lib/grub
QEMU = qemu-system-i386

LIBC = libc/libc.a

CRTI_OBJ = $(BUILD_DIR)/crti.o
CRTBEGIN_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ = $(BUILD_DIR)/crtn.o

vpath %.c src $(ARCH_DIR)
vpath %.s src/asm $(ARCH_DIR)
SRCS := kernel.c terminal.c
ARCH_SRCS := vga.c keyboard.c cursor.c
BOOT_SRC := boot.s

OBJS := $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))
ARCH_OBJS := $(addprefix $(BUILD_DIR)/, $(ARCH_SRCS:.c=.o))
BOOT_OBJ := $(BUILD_DIR)/boot.o
DEPS := $(OBJS:.o=.d)

OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(ARCH_OBJS) $(CRTEND_OBJ) $(CRTN_OBJ)
INTERNAL_OBJS:=$(CRTI_OBJ) $(OBJS) $(CRTN_OBJ)


vpath %.h includes
INCLUDE_DIR = includes

STAMP_HEADERS := $(BUILD_DIR)/.headers_installed


all: $(NAME)

$(NAME): $(BUILD_DIR) $(STAMP_HEADERS) $(LIBC) $(OBJ_LINK_LIST) $(BOOT_OBJ)
	$(CC) -T linker.ld -o $(NAME) $(LINKER_FLAGS) $(BOOT_OBJ) $(OBJ_LINK_LIST) $(LIBC)

$(LIBC):
	$(MAKE) -C libc

run: $(NAME) $(NAME).iso
	 $(QEMU) -cdrom $(NAME).iso

$(NAME).iso: $(NAME)
	mkdir -p $(SYSROOT)/boot/grub
	cp grub.cfg $(SYSROOT)/boot/grub
	cp $(NAME) $(SYSROOT)/boot
	$(MKRESCUE_PATH) -o $(NAME).iso $(SYSROOT) -d $(GRUB_PATH)/i386-pc

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c $(INCLUDES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(ASM) $< -o $@

clean:
	$(MAKE) -C libc clean
	rm -f $(INTERNAL_OBJS)
	rm -drf $(BUILD_DIR)

fclean: clean
	$(MAKE) -C libc fclean
	rm -f $(NAME) $(NAME).iso
	rm -drf isodir
	rm -drf $(SYSROOT)

re: fclean all

$(STAMP_HEADERS): $(wildcard includes/*.h) $(wildcard libc/includes/*.h) | $(BUILD_DIR)
	mkdir -p $(SYSROOT)/usr/include
	cp -R --preserve=timestamps $(INCLUDE_DIR)/. $(SYSROOT)/usr/include/.
	cp -R --preserve=timestamps libc/includes/. $(SYSROOT)/usr/include/
	touch $@

.PHONY: all clean fclean re run install_headers

-include $(DEPS)
