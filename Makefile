CC					:= g++-9
LINKER				:= g++-9
CFLAGS				:= -Wall -Werror -std=c++17 -g
COMPILE_OPTIONS 	:= -DIGNORE_DIRECTORIES
LINKER_FLAGS		:= -lpthread
TEST_LINKER_FLAGS	:= -L/usr/local/lib -lgtest -lgtest_main -lpthread

BINARY				:= hotbackup
OBJ_DIR				:= obj

LINUX_SOURCES		:= $(wildcard ./linux/*.cpp)
LINUX_OBJECTS		:= $(LINUX_SOURCES:./linux/%.cpp=./obj/%.o)

COMMON_SOURCES		:= $(wildcard ./*.cpp)
COMMON_OBJECTS		:= $(COMMON_SOURCES:./%.cpp=./obj/%.o)

TEST_OBJ_DIR		:= $(OBJ_DIR)
TEST_SRC_DIR		:= ./unit-tests

TEST_BINARY			:= tests
TEST_SOURCES		:= $(wildcard unit-tests/*.cpp)
TEST_OBJS			:= $(TEST_SOURCES:unit-tests/%.cpp=obj/%.o)

MKDIR_P				:= mkdir -p


all: create_directories build_project

build_project: build_for_linux

build_for_linux: $(COMMON_OBJECTS) $(LINUX_OBJECTS)
	$(LINKER) -o $(BINARY) $^ $(LINKER_FLAGS)

# Build common sources
$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $(COMPILE_OPTIONS) -c -o $@ $^

# Build linux specific sources
$(OBJ_DIR)/%.o: linux/%.cpp
	$(CC) $(CFLAGS) $(COMPILE_OPTIONS) -c -o $@ $^


# Build tests
tests: create_directories build_tests

build_tests: $(TEST_OBJS) $(LINUX_OBJECTS) $(filter-out ./obj/main.o, $(COMMON_OBJECTS))
	$(LINKER) -o $(TEST_BINARY) $^ $(TEST_LINKER_FLAGS)

$(TEST_OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(COMPILE_OPTIONS) -o $@ -c $<


# Create directories
.PHONY: create_directories

create_directories:
	$(MKDIR_P) $(OBJ_DIR)

clean:
	rm -rf $(OBJECTS) $(BINARY) $(TEST_OBJS) $(TEST_BINARY) $(OBJ_DIR)
