CC := i686-elf-gcc
INCLUDES := -Iincludes -I../includes
CFLAGS := -Werror -Wextra -Wall -MD $(INCLUDES) -ffreestanding
NAME := libc.a
BUILD_DIR := .build

SRCS = $(shell find srcs -name "*.c")

OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(BUILD_DIR) $(OBJS)
	ar rcs $(NAME) $(OBJS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

-include $(DEPS)
