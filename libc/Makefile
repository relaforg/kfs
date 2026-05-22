CC := i686-elf-gcc
INCLUDES := -Iincludes
CFLAGS := -Werror -Wextra -Wall -MD $(INCLUDES) -freestanding
NAME := libc.a
BUILD_DIR := .build

VPATH := srcs/gnl:srcs/is:srcs/lst:srcs/mem:srcs/print:srcs/str:srcs/to:srcs/math
SRCS := ft_isalnum.c\
		ft_isalpha.c\
		ft_isascii.c\
		ft_isdigit.c\
		ft_isprint.c\
		ft_isspace.c\
		ft_isqrt.c\
		ft_bzero.c\
		ft_memmove.c\
		ft_strchr.c\
		ft_striteri.c\
		ft_strlcat.c\
		ft_strlcpy.c\
		ft_strlen.c\
		ft_strncmp.c\
		ft_strnstr.c\
		ft_strrchr.c\
		ft_atoi.c\
		ft_strtoi.c\
		ft_strtol.c\
		ft_toabs.c\
		ft_tolower.c\
		ft_toupper.c

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
