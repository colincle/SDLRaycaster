TARGET = SDLRaycaster
CC = gcc
CFLAGS =	-Wall -Wextra -Wno-deprecated-declarations -arch x86_64 -Ofast -march=native -flto -ffast-math \
			-funroll-loops -fno-exceptions -fomit-frame-pointer
			
LDFLAGS = -arch x86_64 -L$(LIBS)/lib \
          -force_load $(LIBS)/lib/libSDL2.a \
          -force_load $(LIBS)/lib/libSDL2_image.a \
          -force_load $(LIBS)/lib/libSDL2_ttf.a \
          -force_load $(LIBS)/lib/libSDL2_mixer.a \
          -lpng \
          -framework Cocoa -framework IOKit -framework CoreVideo \
          -framework CoreFoundation -framework AudioToolbox -framework ForceFeedback \
          -framework GameController -framework Metal -framework CoreHaptics \
          -framework CoreAudio -framework Carbon




LIBS = libs

INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj
SRC = $(shell find $(SRC_DIR) -type f -name "*.c")
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

all: $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR) -I$(LIBS)/include

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

fclean: clean

run:
	export DYLD_LIBRARY_PATH=./libs/lib:$$DYLD_LIBRARY_PATH; ./$(TARGET)
