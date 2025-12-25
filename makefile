CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -fPIC -fstack-protector-strong

OBJDIR  := OBJ
LIBDIR  := LIB

# Find all .c files in src
SOURCES := $(wildcard src/*.*) $(wildcard src/*/*.*) $(wildcard src/*/*/*.*) $(wildcard src/*/*/*/*.*)
# Convert src/file.c to OBJ/file.o
OBJECTS := $(patsubst src/%.*, $(OBJDIR)/%.o, $(SOURCES))

DLL     := $(LIBDIR)/libzutil.dll
STATIC  := $(LIBDIR)/libzutil.a
MSVC_LIB := $(LIBDIR)/libzutil.lib
SHARED  := $(LIBDIR)/libzutil.so

OS := $(shell uname 2>/dev/null)

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

# Rule to compile each .c file into its own .o file
$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Create shared library (.so)
$(SHARED): $(OBJECTS)
	$(CC) -shared -o $(SHARED) $(OBJECTS)

$(MSVC_LIB): $(OBJECTS)
	ar rcs $@ $(OBJECTS)

# Create Windows DLL (MinGW)
$(DLL): $(OBJECTS)
	$(CC) -shared -o $(DLL) $(OBJECTS) -Wl,--out-implib,$(LIBDIR)/libzutil.dll.a

# Create static library using ALL object files
$(STATIC): $(OBJECTS)
	@$(call MKDIR, $(LIBDIR))
	ar rcs $(STATIC) $(OBJECTS)
	$(shell ranlib $(STATIC))

clean:
	rm -rf $(OBJDIR) $(LIBDIR) $(TEST_BIN) logfile.test.log test.log

test: $(STATIC)
	$(CC) test.c $(STATIC) -o $(TEST_BIN) -fstack-protector-strong
	$(CC) yes.c $(STATIC) -o yes -fstack-protector-strong
	./$(TEST_BIN)

dirs:
	mkdir OBJ SRC include LIB
 
.PHONY: all clean test dirs