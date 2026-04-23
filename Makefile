NAME        = woody_woodpacker
CC          = gcc
CFLAGS      = -Wall -Wextra -Werror -g -I $(INC_DIR) -I $(LIB_DIR)
SRC_DIR     = src
INC_DIR     = header
OBJ_DIR     = obj
LIB_DIR     = libft
LIBFT       = $(LIB_DIR)/libft.a
LDFLAGS     = -L$(LIB_DIR) -lft
AS          = nasm
ASFLAGS     = -f elf64
SRCS        = $(SRC_DIR)/main.c $(SRC_DIR)/utils.c $(SRC_DIR)/rc4.c
OBJS        = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
GREEN       = \033[0;32m
RED         = \033[0;31m
RESET       = \033[0m
EXEC		= woody

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(NAME)
	@clang -m64 -o sample resources/sample.c
	@nasm -f bin stub.s -o stub.bin
	@echo -e "$(GREEN)[+] $(NAME) compilé avec succès !$(RESET)"

$(LIBFT):
	@echo "Compiling libft..."
	@make -sC $(LIB_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/woody.h | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiling $<..."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s | $(OBJ_DIR)
	@$(AS) $(ASFLAGS) $< -o $@
	@echo "Assembling $<..."

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)
	@make clean -sC $(LIB_DIR)
	@echo -e "$(RED)[- ] Objets supprimés (woody & libft).$(RESET)"

fclean: clean
	@rm -f $(NAME) $(EXEC) sample stub.bin
	@make fclean -sC $(LIB_DIR)
	@echo -e "$(RED)[- ] Exécutable et libft.a supprimés.$(RESET)"

re: fclean all

.PHONY: all clean fclean re