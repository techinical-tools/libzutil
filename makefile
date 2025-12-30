OS := $(shell uname 2>/dev/null)

CC := gcc

# Detect OS automatically
IS_MINGW := $(findstring MINGW,$(OS))
IS_LINUX := $(findstring Linux,$(OS))

# Common flags
CFLAGS_COMMON := -Wall -Wextra -O2 -fPIC -fstack-protector-strong

# Platform-specific flags
ifeq ($(IS_MINGW),MINGW)
    LDFLAGS := -lbcrypt -lws2_32
else
    LDFLAGS :=
endif

CFLAGS := $(CFLAGS_COMMON)

OBJDIR := OBJ
LIBDIR := LIB

# Find all .c files in src recursively
SOURCES := $(wildcard src/*.*) $(wildcard src/*/*.*) $(wildcard src/*/*/*.*) $(wildcard src/*/*/*/*.*)
OBJECTS := $(patsubst src/%.c, $(OBJDIR)/%.o, $(SOURCES))

DLL := $(LIBDIR)/libzutil.dll
STATIC := $(LIBDIR)/libzutil.a
SHARED := $(LIBDIR)/libzutil.so
MSVC_LIB := $(LIBDIR)/libzutil.lib

ifeq ($(OS),)
    TEST_BIN := test.exe
else
    TEST_BIN := test
endif

all: $(OBJDIR) $(LIBDIR) $(STATIC) $(SHARED) $(DLL)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

# Compile each .c file into its own .o file
$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Shared library (.so)
$(SHARED): $(OBJECTS)
	$(CC) $(CFLAGS) -shared -o $(SHARED) $(OBJECTS) $(LDFLAGS)

# Windows DLL (MinGW)
$(DLL): $(OBJECTS)
	$(CC) $(CFLAGS) -shared -o $(DLL) $(OBJECTS) -Wl,--out-implib,$(LIBDIR)/libzutil.dll.a $(LDFLAGS)

# Static library
$(STATIC): $(OBJECTS)
	ar rcs $(STATIC) $(OBJECTS)

# Optional MSVC lib
$(MSVC_LIB): $(OBJECTS)
	ar rcs $@ $(OBJECTS)

clean:
	rm -rf $(OBJDIR) $(LIBDIR) $(TEST_BIN) yes logfile.test.log test.log

test: $(STATIC)
	$(CC) test.c $(STATIC) -o $(TEST_BIN) -fstack-protector-strong $(LDFLAGS)
	$(CC) yes.c $(STATIC) -o yes -fstack-protector-strong $(LDFLAGS)

dirs:
	mkdir -p OBJ LIB OBJ/cryptography OBJ/network

.PHONY: all clean test dirs
