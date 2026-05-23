CC := i686-elf-gcc
INCLUDES := -Iincludes -I../includes
CFLAGS := -Werror -Wextra -Wall -MD $(INCLUDES) -ffreestanding
NAME := libc.a
BUILD_DIR := .build

VPATH := srcs/ctype:srcs/stdlib:srcs/string:srcs/math:srcs/stdio
SRCS := isalnum.c\
		isalpha.c\
		isascii.c\
		isdigit.c\
		isprint.c\
		isspace.c\
		isqrt.c\
		bzero.c\
		memmove.c\
		strchr.c\
		striteri.c\
		strlcat.c\
		strlcpy.c\
		strlen.c\
		strncmp.c\
		strnstr.c\
		strrchr.c\
		atoi.c\
		strtoi.c\
		strtol.c\
		toabs.c\
		tolower.c\
		toupper.c\
		printf.c\
		putchar.c\
		puts.c

OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(BUILD_DIR) $(OBJS)
	ar rcs $(NAME) $(OBJS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

-include $(DEPS)
