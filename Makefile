CC := gcc

TARGET := analog_tm

SRC_DIR := src
INC_DIR := inc
BUILD_DIR := build

CFLAGS := -Wall -Wextra -std=c99 -I$(INC_DIR)

LIBS := -lraylib -lm

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    LIBS += -lGL -lpthread -ldl -lrt -lX11
endif

ifeq ($(UNAME_S),Darwin)
    LIBS := -lraylib \
            -framework OpenGL \
            -framework Cocoa \
            -framework IOKit \
            -framework CoreVideo \
            -lm
endif

SRC := $(shell find $(SRC_DIR) -name '*.c')
OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	LIBGL_ALWAYS_SOFTWARE=1 ./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all run clean
