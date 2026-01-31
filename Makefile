CC = gcc
CSTD = c23
FLAGS = -Wall -Wextra -Wpedantic -MMD -MP -O2 -flto -mavx2 -mfma -march=native

BIN_DIR = bin
TMP_DIR = .tmp

$(TMP_DIR) $(BIN_DIR):
	mkdir -p $@


### pcg-basic ###

PCG_DIR = lib/pcg
PCG_LIB = $(TMP_DIR)/pcg.a
PCG_EXT = pcg-

PCG_SRCS := $(wildcard $(PCG_DIR)/*.c)
PCG_OBJS := $(patsubst $(PCG_DIR)/%.c, $(TMP_DIR)/$(PCG_EXT)%.o, $(PCG_SRCS))

$(PCG_OBJS): $(TMP_DIR)/$(PCG_EXT)%.o : $(PCG_DIR)/%.c | $(TMP_DIR)
	$(CC) -std=$(CSTD) $(FLAGS) -I./$(PCG_DIR) -c $< -o $@

$(PCG_LIB): $(PCG_OBJS)
	ar rcs $@ $^



### target ###

INCL_DIR = include
SRC_DIR = src

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(TMP_DIR)/%.o, $(SRCS))
LIBS = $(PCG_LIB)

DEPS := $(OBJS:.o=.d)
TARGET = $(BIN_DIR)/exe

$(TMP_DIR)/%.o: $(SRC_DIR)/%.c | $(TMP_DIR)
	$(CC) -std=$(CSTD) $(FLAGS) -I./lib -I./$(INCL_DIR) -c $< -o $@

$(TARGET): $(OBJS) $(LIBS) | $(BIN_DIR)
	$(CC) -std=$(CSTD) $(FLAGS) $(OBJS) $(LIBS) -o $@
  

build: $(TARGET)

run: build
	./$(TARGET) $(ARGS)

clean:
	rm -rv $(TMP_DIR) $(BIN_DIR)


-include $(DEPS)

.PHONY: run build clean