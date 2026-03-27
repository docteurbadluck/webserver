CC = cc

CFLAGS = -Wall -Wextra -Werror -I unity -D_GNU_SOURCE

NAME = testeur_webserver

SRC = test.c \
    unity/unity.c \
	helper.c \
	test_get.c \
	test_post.c \
	test_delete.c \
	test_multiple.c \
	test_redirect.c \
	test_connection.c \
	test_cgi.c

BUILD_DIR = build

OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all re clean fclean
 
