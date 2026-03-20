NAME        = woody_woodpacker
CC          = gcc
CFLAGS      = -Wall -Wextra -Werror -I $(INC_DIR)
SRC_DIR     = src
INC_DIR     = header
OBJ_DIR     = obj
AS          = nasm
ASFLAGS     = -f elf64
SRCS        = $(SRC_DIR)/main.c $(SRC_DIR)/utils.c
OBJS        = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
GREEN       = \033[0;32m
RED         = \033[0;31m
RESET       = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)[+] $(NAME) compilé avec succès !$(RESET)"

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
	@echo "$(RED)[-] Objets et dossier $(OBJ_DIR) supprimés.$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)[-] Exécutable $(NAME) supprimé.$(RESET)"

re: fclean all

.PHONY: all clean fclean re