# Documentation for the targets can be found at
# their respective declarations
.PHONY: all test test-valgrind indent clean
.PHONY: test-typemem demo-webstore demo-default-destructors test-demo-webstore
.PHONY: test-valgrind test-typemem-valgrind demo-webstore-valgrind demo-default-destructors-valgrind test-demo-webstore-valgrind

# C compiler
CC = gcc

# C compiler flags
CFLAGS += -std=gnu17
CFLAGS += -pedantic
#CFLAGS += -Wpedantic
#CFLAGS += -pedantic-errors
CFLAGS += -Wall
#CFLAGS += -Wextra
#CFLAGS += -Wbad-function-cast
#CFLAGS += -Wmissing-declarations
#CFLAGS += -Wmissing-include-dirs
# If a missing prototype warning shows up for a private
# function make sure that it is declared static.
#CFLAGS += -Wmissing-prototypes
#CFLAGS += -Wredundant-decls
#CFLAGS += -Wshadow
#CFLAGS += -Wundef
#CFLAGS += -Wunreachable-code
#CFLAGS += -Wunused-but-set-parameter

CFLAGS += --coverage

# External libraries used
CLIBS += -lcunit

# Names
LIBRARY_NAME = refmem
TEST_NAME = test_refmem
TEST_NAME_PLUGINS_TYPEMEM = test_refmem_plugins_typemem
DEMO_NAME_DEFAULT_DESTRUCTOR = demo_refmem_default_destructors
DEMO_NAME_WEBSTORE = demo_refmem_webstore
DEMO_NAME_WEBSTORE_TEST_LIBIOOPM = demo_refmem_webstore_test_libioopm
DEMO_NAME_WEBSTORE_TEST_LOGIC = demo_refmem_webstore_test_logic

# Directories
BUILD_DIR = build
BUILD_DIR_DEMO = $(BUILD_DIR)/demo
BIN_DIR = $(BUILD_DIR)/bin
SRC_DIR = src
SRC_DIR_TEST = test
SRC_DIR_TEST_TYPEMEM = $(SRC_DIR_TEST)/typemem
SRC_DIR_LIB = $(SRC_DIR)/lib
SRC_DIR_LIBIOOPM = $(SRC_DIR_LIB)/libioopm
SRC_DIR_LIBMETA = $(SRC_DIR_LIB)/libmeta
SRC_DIR_PLUGINS = $(SRC_DIR)/plugins
SRC_DIR_PLUGINS_TYPEMEM = $(SRC_DIR_PLUGINS)/typemem
SRC_DIR_DEMO = demo
SRC_DIR_DEMO_DEFAULT_DESTRUCTORS = $(SRC_DIR_DEMO)/default_destructors
SRC_DIR_DEMO_WEBSTORE = $(SRC_DIR_DEMO)/webstore
SRC_DIR_DEMO_WEBSTORE_LIBWEBSTORE = $(SRC_DIR_DEMO_WEBSTORE)/libwebstore
SRC_DIR_DEMO_WEBSTORE_TEST = $(SRC_DIR_DEMO_WEBSTORE)/test

SRC_DIRS = $(SRC_DIR) \
           $(SRC_DIR_TEST) \
           $(SRC_DIR_TEST_TYPEMEM) \
           $(SRC_DIR_LIB) \
           $(SRC_DIR_LIBIOOPM) \
           $(SRC_DIR_LIBMETA) \
           $(SRC_DIR_PLUGINS) \
           $(SRC_DIR_PLUGINS_TYPEMEM) \
           $(SRC_DIR_DEMO) \
           $(SRC_DIR_DEMO_DEFAULT_DESTRUCTORS) \
           $(SRC_DIR_DEMO_WEBSTORE) \
           $(SRC_DIR_DEMO_WEBSTORE_LIBWEBSTORE) \
           $(SRC_DIR_DEMO_WEBSTORE_TEST)

# Source files
SRC_FILES_REFMEM = $(wildcard $(SRC_DIR)/*.c)
SRC_FILES_LIBIOOPM = $(wildcard $(SRC_DIR_LIBIOOPM)/*.c)
SRC_FILES_LIBMETA = $(wildcard $(SRC_DIR_LIBMETA)/*.c)
SRC_FILES_TEST = $(wildcard $(SRC_DIR_TEST)/*.c)
SRC_FILES_TEST_TYPEMEM = $(wildcard $(SRC_DIR_TEST_TYPEMEM)/*.c)
SRC_FILES_PLUGINS_TYPEMEM = $(wildcard $(SRC_DIR_PLUGINS_TYPEMEM)/*.c)
SRC_FILES_DEMO_DEFAULT_DESTRUCTOR = $(wildcard $(SRC_DIR_DEMO_DEFAULT_DESTRUCTORS)/*.c)
SRC_FILES_DEMO_WEBSTORE = $(wildcard $(SRC_DIR_DEMO_WEBSTORE)/*.c)
SRC_FILES_DEMO_WEBSTORE_LIBWEBSTORE = $(wildcard $(SRC_DIR_DEMO_WEBSTORE_LIBWEBSTORE)/*.c)
SRC_FILES_DEMO_WEBSTORE_TEST_LIBIOOPM = $(SRC_DIR_DEMO_WEBSTORE_TEST)/libioopm_tests.c
SRC_FILES_DEMO_WEBSTORE_TEST_LOGIC = $(SRC_DIR_DEMO_WEBSTORE_TEST)/logic_tests.c

# Object files
OBJ_FILES_REFMEM = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES_REFMEM))
OBJ_FILES_LIBIOOPM = $(patsubst $(SRC_DIR_LIBIOOPM)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES_LIBIOOPM))
OBJ_FILES_LIBMETA = $(patsubst $(SRC_DIR_LIBMETA)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES_LIBMETA))
OBJ_FILES_TEST = $(patsubst $(SRC_DIR_TEST)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES_TEST))
OBJ_FILES_TEST_TYPEMEM = $(patsubst $(SRC_DIR_TEST_TYPEMEM)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES_TEST_TYPEMEM))
OBJ_FILES_PLUGINS_TYPEMEM = $(patsubst $(SRC_DIR_PLUGINS_TYPEMEM)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES_PLUGINS_TYPEMEM))
OBJ_FILES_DEMO_DEFAULT_DESTRUCTOR = $(patsubst $(SRC_DIR_DEMO_DEFAULT_DESTRUCTORS)/%.c, $(BUILD_DIR_DEMO)/%.o, $(SRC_FILES_DEMO_DEFAULT_DESTRUCTOR))
OBJ_FILES_DEMO_WEBSTORE = $(patsubst $(SRC_DIR_DEMO_WEBSTORE)/%.c, $(BUILD_DIR_DEMO)/%.o, $(SRC_FILES_DEMO_WEBSTORE))
OBJ_FILES_DEMO_WEBSTORE_LIBWEBSTORE = $(patsubst $(SRC_DIR_DEMO_WEBSTORE_LIBWEBSTORE)/%.c, $(BUILD_DIR_DEMO)/%.o, $(SRC_FILES_DEMO_WEBSTORE_LIBWEBSTORE))
OBJ_FILES_DEMO_WEBSTORE_TEST_LIBIOOPM = $(patsubst $(SRC_DIR_DEMO_WEBSTORE_TEST)/%.c, $(BUILD_DIR_DEMO)/%.o, $(SRC_FILES_DEMO_WEBSTORE_TEST_LIBIOOPM))
OBJ_FILES_DEMO_WEBSTORE_TEST_LOGIC = $(patsubst $(SRC_DIR_DEMO_WEBSTORE_TEST)/%.c, $(BUILD_DIR_DEMO)/%.o, $(SRC_FILES_DEMO_WEBSTORE_TEST_LOGIC))

# Indentation flags for Apache C Language Style Guide
INDENT_FLAGS = -i4 -npsl -di0 -br -nce -d0 -cli0 -npcs -nfc1 -nut

# Build library
$(LIBRARY_NAME): $(OBJ_FILES_LIBIOOPM) $(OBJ_FILES_LIBMETA) $(OBJ_FILES_PLUGINS_TYPEMEM) $(OBJ_FILES_REFMEM)

# Compile object files for refmem
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -g -c $< -o $@

# Compile object files for libioopm
$(BUILD_DIR)/%.o: $(SRC_DIR_LIBIOOPM)/%.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -g -c $< -o $@

# Compile object files for libmeta
$(BUILD_DIR)/%.o: $(SRC_DIR_LIBMETA)/%.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -g -c $< -o $@

# Compile object files for test
$(BUILD_DIR)/%.o: $(SRC_DIR_TEST)/%.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -g -c $< -o $@

# Compile object files for typemem plugin test
$(BUILD_DIR)/%.o: $(SRC_DIR_TEST_TYPEMEM)/%.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -g -c $< -o $@

# Compile object files for typemem plugin
$(BUILD_DIR)/%.o: $(SRC_DIR_PLUGINS_TYPEMEM)/%.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -g -c $< -o $@

# Compile object files for default destructor demo
$(BUILD_DIR_DEMO)/%.o: $(SRC_DIR_DEMO_DEFAULT_DESTRUCTORS)/%.c
	@mkdir -p $(BUILD_DIR_DEMO)
	@$(CC) $(CFLAGS) -g -c $< -o $@

# Compile object files for webstore demo
$(BUILD_DIR_DEMO)/%.o: $(SRC_DIR_DEMO_WEBSTORE)/%.c
	@mkdir -p $(BUILD_DIR_DEMO)
	@$(CC) $(CFLAGS) -g -c $< -o $@

# Compile object files for webstore demo libwebstore
$(BUILD_DIR_DEMO)/%.o: $(SRC_DIR_DEMO_WEBSTORE_LIBWEBSTORE)/%.c
	@mkdir -p $(BUILD_DIR_DEMO)
	@$(CC) $(CFLAGS) -g -c $< -o $@

# Compile object files for webstore demo test
$(BUILD_DIR_DEMO)/%.o: $(SRC_DIR_DEMO_WEBSTORE_TEST)/%.c
	@mkdir -p $(BUILD_DIR_DEMO)
	@$(CC) $(CFLAGS) -g -c $< -o $@


# Link object files for test into executable
$(TEST_NAME): $(OBJ_FILES_LIBIOOPM) $(OBJ_FILES_LIBMETA) $(OBJ_FILES_REFMEM) $(OBJ_FILES_TEST)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) -g -o $(BIN_DIR)/$@ $^ $(CLIBS)

# Link object files for typemem plugin test into executable
$(TEST_NAME_PLUGINS_TYPEMEM): $(OBJ_FILES_LIBIOOPM) $(OBJ_FILES_LIBMETA) $(OBJ_FILES_REFMEM) $(OBJ_FILES_PLUGINS_TYPEMEM) $(OBJ_FILES_TEST_TYPEMEM)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) -g -o $(BIN_DIR)/$@ $^ $(CLIBS)

# Link object files for default destructor demo into executable
$(DEMO_NAME_DEFAULT_DESTRUCTOR): $(OBJ_FILES_LIBIOOPM) $(OBJ_FILES_LIBMETA) $(OBJ_FILES_REFMEM) $(OBJ_FILES_PLUGINS_TYPEMEM) $(OBJ_FILES_DEMO_DEFAULT_DESTRUCTOR)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) -g -o $(BIN_DIR)/$@ $^ $(CLIBS)

# Link object files for webstore demo into executable
$(DEMO_NAME_WEBSTORE): $(OBJ_FILES_LIBIOOPM) $(OBJ_FILES_LIBMETA) $(OBJ_FILES_REFMEM) $(OBJ_FILES_DEMO_WEBSTORE) $(OBJ_FILES_DEMO_WEBSTORE_LIBWEBSTORE)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) -g -o $(BIN_DIR)/$@ $^ $(CLIBS)

# Link object files for webstore demo libioopm test into executable
$(DEMO_NAME_WEBSTORE_TEST_LIBIOOPM): $(OBJ_FILES_LIBIOOPM) $(OBJ_FILES_LIBMETA) $(OBJ_FILES_REFMEM) $(OBJ_FILES_DEMO_WEBSTORE_LIBWEBSTORE) $(OBJ_FILES_DEMO_WEBSTORE_TEST_LIBIOOPM)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) -g -o $(BIN_DIR)/$@ $^ $(CLIBS)

# Link object files for webstore demo logic test into executable
$(DEMO_NAME_WEBSTORE_TEST_LOGIC): $(OBJ_FILES_LIBIOOPM) $(OBJ_FILES_LIBMETA) $(OBJ_FILES_REFMEM) $(OBJ_FILES_DEMO_WEBSTORE_LIBWEBSTORE) $(OBJ_FILES_DEMO_WEBSTORE_TEST_LOGIC)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) -g -o $(BIN_DIR)/$@ $^ $(CLIBS)


# Create statically linkable object files for library
all: $(LIBRARY_NAME)

# Run the unit tests
test: $(TEST_NAME)
	./$(BIN_DIR)/$(TEST_NAME)

# Run the unit tests for typemem plugin
test-typemem: $(TEST_NAME_PLUGINS_TYPEMEM)
	./$(BIN_DIR)/$(TEST_NAME_PLUGINS_TYPEMEM)

# Run a full memory leak test of the execution of the test
test-coverage: $(TEST_NAME)
	./$(BIN_DIR)/$(TEST_NAME)
	@gcov -abcfu ./build/refmem

# Run the webstore demo
demo-webstore: $(DEMO_NAME_WEBSTORE)
	./$(BIN_DIR)/$(DEMO_NAME_WEBSTORE)

# Run the default destructor demo
demo-default-destructors: $(DEMO_NAME_DEFAULT_DESTRUCTOR)
	./$(BIN_DIR)/$(DEMO_NAME_DEFAULT_DESTRUCTOR)

# Run the unit tests for the webstore demo
test-demo-webstore: $(DEMO_NAME_WEBSTORE_TEST_LIBIOOPM) $(DEMO_NAME_WEBSTORE_TEST_LOGIC)
	@printf "Running webstore demo libioopm tests:\n"
	./$(BIN_DIR)/$(DEMO_NAME_WEBSTORE_TEST_LIBIOOPM)
	@printf "\nRunning webstore demo logic tests:\n"
	./$(BIN_DIR)/$(DEMO_NAME_WEBSTORE_TEST_LOGIC)


# The same but with valgrind below:
test-valgrind: $(TEST_NAME)
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/$(TEST_NAME)

test-typemem-valgrind: $(TEST_NAME_PLUGINS_TYPEMEM)
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/$(TEST_NAME_PLUGINS_TYPEMEM)

demo-webstore-valgrind: $(DEMO_NAME_WEBSTORE)
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/$(DEMO_NAME_WEBSTORE)

demo-default-destructors-valgrind: $(DEMO_NAME_DEFAULT_DESTRUCTOR)
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/$(DEMO_NAME_DEFAULT_DESTRUCTOR)

test-demo-webstore-valgrind: $(DEMO_NAME_WEBSTORE_TEST_LIBIOOPM) $(DEMO_NAME_WEBSTORE_TEST_LOGIC)
	@printf "Running webstore demo libioopm tests:\n"
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/$(DEMO_NAME_WEBSTORE_TEST_LIBIOOPM)
	@printf "\nRunning webstore demo logic tests:\n"
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/$(DEMO_NAME_WEBSTORE_TEST_LOGIC)


# Aliases for compatability with instructions
memtest: test-valgrind
demo: demo-webstore


# Indents all code according to the Apache C Language Style Guide
indent:
	@find $(SRC_DIRS) -name '*.c' -exec indent $(INDENT_FLAGS) {} \;
	@find $(SRC_DIRS) -name '*.h' -exec indent $(INDENT_FLAGS) {} \;

# Remove all files and directories generated by this makefile
clean:
	@rm -rf $(BUILD_DIR) refmem.c.gcov
