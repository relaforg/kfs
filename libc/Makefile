CC := i686-elf-gcc
INCLUDES := -Iincludes
CFLAGS := -Werror -Wextra -Wall -MD $(INCLUDES) -ffreestanding
NAME := libc.a
BUILD_DIR := .build

VPATH := srcs/gnl:srcs/is:srcs/lst:srcs/mem:srcs/print:srcs/str:srcs/to:srcs/math
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
		toupper.c

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
