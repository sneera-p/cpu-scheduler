CC = gcc
CSTD = gnu23
FLAGS = -Wall -Wextra -Wpedantic -MMD -MP -O2 -flto -march=native

BIN_DIR = bin
TMP_DIR = .tmp

$(TMP_DIR) $(BIN_DIR):
	mkdir -p $@

INCL_DIR = include
SRC_DIR = src
OBJS = $(patsubst $(SRC_DIR)/%.c, $(TMP_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))
DEPS := $(OBJS:.o=.d)
TARGET = $(BIN_DIR)/exe



$(TMP_DIR)/%.o: $(SRC_DIR)/%.c | $(TMP_DIR)
	$(CC) -std=$(CSTD) $(FLAGS) -I./ -I./lib -I./$(INCL_DIR) -c $< -o $@

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) -std=$(CSTD) $(FLAGS) $(OBJS) -o $@
  

build: $(TARGET)

run: build
	./$(TARGET) $(ARGS)

clean:
	rm -rv $(TMP_DIR) $(BIN_DIR)


-include $(DEPS)

.PHONY: run build clean