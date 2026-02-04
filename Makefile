CC = gcc
CSTD = c23

DEBUG_FLAGS = \
	-O0 \
	-g3 \
	-DDEBUG \
	-fsanitize=address \
	-fsanitize=undefined \
	-fsanitize=leak \
	-fsanitize=null \
	-fsanitize=bounds \
	-fsanitize=float-divide-by-zero \
	-fsanitize-recover=all \
	-fstack-protector-strong \
	-fno-omit-frame-pointer \
	-fno-inline

RELEASE_FLAGS = \
	-O2 \
	-DNDEBUG \
	-flto \
	-mavx2 \
	-mfma \
	-march=native

FLAGS = \
	-MMD \
	-MP \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wshadow \
	-Wformat=2 \
	-Wsign-conversion \
	-Wcast-align \
	-Wlogical-op \
	-Wnull-dereference \
	-Wfloat-equal \
	-Wredundant-decls
# 	-Wconversion \


ifeq ($(strip $(MODE)),r)
	FLAGS += $(RELEASE_FLAGS)
else
	FLAGS += $(DEBUG_FLAGS)
endif

BIN_DIR = bin
TMP_DIR = .tmp

$(TMP_DIR) $(BIN_DIR):
	mkdir -p $@


### utils ###

UTILS_DIR = lib/utils


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
ifeq ($(strip $(IN)),)
	./$(TARGET) $(LOG)
else
	cat $(IN) | ./$(TARGET) $(LOG)
endif

clean:
	rm -rv $(TMP_DIR) $(BIN_DIR) ./*.txt


-include $(DEPS)

.PHONY: run build clean