SYSROOT := $(PWD)/sysroot
CC := i686-elf-gcc
ASM = i686-elf-as
NAME := kfs
BUILD_DIR := .build
CFLAGS = --sysroot=$(SYSROOT) -std=gnu99 -ffreestanding -O2 \
		 -Wall -Wextra -MD \
		 -isystem $(SYSROOT)/usr/include
LINKER_FLAGS = -ffreestanding -O2 -nostdlib -lgcc
MKRESCUE_PATH ?= ~/.local/bin/grub-mkrescue
GRUB_PATH ?= ~/.local/lib/grub
QEMU = qemu-system-i386

LIBC = libc/libc.a

CRTI_OBJ = $(BUILD_DIR)/crti.o
CRTBEGIN_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ = $(BUILD_DIR)/crtn.o

vpath %.c src
vpath %.s src/asm
SRCS := kernel.c
OBJS := $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))
DEPS := $(OBJS:.o=.d)

OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(CRTEND_OBJ) $(CRTN_OBJ)
INTERNAL_OBJS:=$(CRTI_OBJ) $(OBJS) $(CRTN_OBJ)

BOOT_SRC := boot.s
BOOT_OBJ := $(BUILD_DIR)/boot.o

vpath %.h includes
INCLUDE_DIR = includes
HEADERS = test.h

STAMP_HEADERS := $(BUILD_DIR)/.headers_installed


all: $(NAME)

$(NAME): $(BUILD_DIR) $(STAMP_HEADERS) $(LIBC) $(OBJ_LINK_LIST) $(BOOT_OBJ)
	$(CC) -T linker.ld -o $(NAME) $(LINKER_FLAGS) $(BOOT_OBJ) $(OBJ_LINK_LIST) $(LIBC)

$(LIBC):
	$(MAKE) -C libc

run: $(NAME) $(NAME).iso
	 $(QEMU) -cdrom $(NAME).iso

$(NAME).iso:
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

$(STAMP_HEADERS): $(HEADERS) libc/includes/libft.h | $(BUILD_DIR)
	mkdir -p $(SYSROOT)/usr/include
	cp -R --preserve=timestamps $(INCLUDE_DIR)/. $(SYSROOT)/usr/include/.
	cp libc/includes/libft.h $(SYSROOT)/usr/include/
	touch $@

.PHONY: all clean fclean re run install_headers

-include $(DEPS)
