TARGET = SDLRaycaster
CC = cc

INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj

# SDL2 flags come from sdl2-config (installed with the SDL2 dev package).
# SDL2_image and SDL2_mixer do not ship a -config tool, so link them directly.
SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LIBS   = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_mixer

CFLAGS  = -Wall -Wextra -O2 -I$(INCLUDE_DIR) $(SDL_CFLAGS)
LDFLAGS = $(SDL_LIBS)

DEBUG_FLAGS = -fsanitize=address -g

SRC = $(shell find $(SRC_DIR) -type f -name "*.c")
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
TOTAL_SRCS = $(words $(SRC))

# Colors
DEF_COLOR   = \033[0;39m
GREEN       = \033[0;92m
BLUE        = \033[0;94m
YELLOW      = \033[0;93m
BAR_COLOR   = \033[42m
RESET_COLOR = \033[0m

all: $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(@D)
	@BAR_WIDTH=30; \
	NUM_OBJS=$$(find $(OBJ_DIR) -type f | wc -l); \
	COMPLETED=$$(((NUM_OBJS + 1) * BAR_WIDTH / $(TOTAL_SRCS))); \
	FILENAME=$$(basename "$<"); \
	printf "\r\033[K$(BAR_COLOR)%-*s$(RESET_COLOR)%-*s $(YELLOW)%s$(DEF_COLOR)" \
		$$COMPLETED "" $$((BAR_WIDTH - COMPLETED)) "" "$$FILENAME"; \
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	@printf "\r\033[K"
	@printf "$(GREEN)Linking $(TARGET)...$(DEF_COLOR)\n"
	@$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)
	@printf "$(BLUE)\n"
	@printf "  SDLRaycaster\n"
	@printf "$(RESET_COLOR)\n"
	@printf "\033[1;36m---------------------------------------\033[0m\n"
	@printf "\033[1;32m CONTROLS:\033[0m\n"
	@printf "\033[1;34m ➤ Look Around:\033[0m ⌨️ Mouse / 🎮 Right stick\n"
	@printf "\033[1;34m ➤ Move:\033[0m ⌨️ WASD / 🎮 Left Stick\n"
	@printf "\033[1;34m ➤ Run:\033[0m ⌨️ Shift / 🎮 L3\n"
	@printf "\033[1;34m ➤ Jump:\033[0m ⌨️ Space / 🎮 Cross\n"
	@printf "\033[1;34m ➤ Crouch:\033[0m ⌨️ C / 🎮 Circle\n"
	@printf "\033[1;34m ➤ Crawl:\033[0m ⌨️ Hold C / 🎮 Hold Circle\n"
	@printf "\033[1;34m ➤ Quit:\033[0m ⌨️ Escape\n"
	@printf "\n"
	@printf "Type \"make run\" to launch the game\n"
	@printf "\033[1;36m---------------------------------------\033[0m\n"

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)$(TARGET) object files cleaned!$(DEF_COLOR)"

fclean: clean
	@rm -rf $(TARGET)
	@echo "$(GREEN)$(TARGET) executable files cleaned!$(DEF_COLOR)"

re: fclean all

debug: CFLAGS += $(DEBUG_FLAGS)
debug: re

run: all
	@./$(TARGET)

.PHONY: all clean fclean re debug run
