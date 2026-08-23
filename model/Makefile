CC := gcc
INCLUDE := -I./utils
CFLAGS := -O2 -Wall -Wextra -fPIC
DEBUG := -O0 -g3 -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer

SRCS := $(shell find ./ -path './general_training' -prune -o -name "*.c" -print)
OUT := build/libmodel.dylib
OUT_LINUX := build/libmodel.so

.PHONY: all clean all_linux clean_linux

all: clean build

build: $(OUT)

$(OUT):
	$(CC) $(CFLAGS) $(INCLUDE) -dynamiclib $(SRCS) -o $(OUT) 

debug: clean
	$(CC) $(DEBUG) $(INCLUDE) -dynamiclib $(SRCS) -o $(OUT) 

clean:
	rm -f $(OUT)

all_linux: clean_linux build_linux

build_linux: $(OUT_LINUX)

$(OUT_LINUX):
	$(CC) $(CFLAGS) $(INCLUDE) -shared $(SRCS) -o $(OUT_LINUX) 

debug_linux: clean_linux
	$(CC) $(DEBUG) $(INCLUDE) -shared $(SRCS) -o $(OUT_LINUX) 

clean_linux:
	rm -f $(OUT_LINUX)
