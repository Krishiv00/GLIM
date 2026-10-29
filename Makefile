CXX := g++
AR := ar
SRC_DIR := src
BUILD_DIR := build
LIB_DIR := lib
LIB_NAME := libglim.a

INCLUDES := -Iinclude
CXXFLAGS := -std=c++23 -O2

rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

CPP_FILES := $(call rwildcard,$(SRC_DIR)/,*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CPP_FILES))

MAKEFLAGS += -j

all: $(LIB_DIR)/$(LIB_NAME)

$(LIB_DIR)/$(LIB_NAME): $(OBJ_FILES) | $(LIB_DIR)
	$(AR) rcs $@ $(OBJ_FILES)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

release: CXXFLAGS += -DNDEBUG
release: all

clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@mkdir $(BUILD_DIR)
