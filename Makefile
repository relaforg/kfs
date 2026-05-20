CC := i686-elf-gcc
ASM = i686-elf-as
NAME := kfs
BUILD_DIR := .build
INCLUDES :=
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -MD $(INCLUDES)
LINKER_FLAGS = -ffreestanding -O2 -nostdlib -lgcc
MKRESCUE_PATH = ~/.local/bin/grub-mkrescue
GRUB_PATH = ~/.local/lib/grub
QEMU = qemu-system-i386

CRTI_OBJ = $(BUILD_DIR)/crti.o
CRTBEGIN_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ = $(BUILD_DIR)/crtn.o

SRCS := kernel.c
OBJS := $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))
DEPS := $(OBJS:.o=.d)

OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(CRTEND_OBJ) $(CRTN_OBJ)
INTERNAL_OBJS:=$(CRTI_OBJ) $(OBJS) $(CRTN_OBJ)

BOOT_SRC := boot.s
BOOT_OBJ := $(BUILD_DIR)/boot.o


all: $(NAME)

$(NAME): $(BUILD_DIR) $(OBJ_LINK_LIST) $(BOOT_OBJ)
	$(CC) -T linker.ld -o $(NAME) $(LINKER_FLAGS) $(BOOT_OBJ) $(OBJ_LINK_LIST)

run: $(NAME)
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub
	cp $(NAME) isodir/boot
	$(MKRESCUE_PATH) -o $(NAME).iso isodir -d $(GRUB_PATH)/i386-pc
	 $(QEMU) -cdrom $(NAME).iso

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(ASM) $< -o $@

clean:
	rm -f $(INTERNAL_OBJS)
	rm -drf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME) $(NAME).iso
	rm -drf isodir

re: fclean all

.PHONY: all clean fclean re run

-include $(DEPS)
