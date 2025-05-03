CXX = g++
CXXFLAGS = -Wall -Wextra -Iinclude -Iinclude/Sockets -Iinclude/APIs -std=c++17
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
PROGRAM_LOG_DIR = $(BIN_DIR)/logs
TARGET = $(BIN_DIR)/main
VCPKG = vcpkg/vcpkg
VCPKG_LIBS = curl sqlite3 nlohmann-json
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

ifeq ($(UNAME_S),Darwin)
  ifeq ($(UNAME_M),arm64)
    VCPKG_TRIPLET = arm64-osx
  else
    VCPKG_TRIPLET = x64-osx
  endif
  FRAMEWORKS = -framework CoreFoundation -framework Security -framework SystemConfiguration
  EXTRA_LIBS = -lz
else ifeq ($(UNAME_S),Linux)
  VCPKG_TRIPLET = x64-linux
  FRAMEWORKS =
  EXTRA_LIBS =
else
  $(error Unsupported system: $(UNAME_S))
endif

VCPKG_INSTALLED_DIR = vcpkg/installed/$(VCPKG_TRIPLET)

CXXFLAGS += -I$(VCPKG_INSTALLED_DIR)/include

define check_installed
  $(shell $(VCPKG) list | grep -q "^$(1)" && echo 1 || echo 0)
endef

dependencies:
	@echo "Verifying dependencies..."
	@missing=0; \
	for pkg in $(VCPKG_LIBS); do \
		if ! $(VCPKG) list | grep -q "$$pkg"; then \
			echo "$$pkg missing"; \
			missing=1; \
		fi; \
	done; \
	if [ $$missing -eq 1 ]; then \
		echo "Installing dependencies..."; \
		$(VCPKG) install $(foreach pkg,$(VCPKG_LIBS),$(pkg):$(VCPKG_TRIPLET)); \
	else \
		echo "All dependencies installed."; \
	fi

SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

LDFLAGS = -L$(VCPKG_INSTALLED_DIR)/lib

LIBS = -lcurl -lsqlite3 $(FRAMEWORKS) $(EXTRA_LIBS)

all: dependencies $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)
	@echo "Compiled successfuly: $(TARGET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "Clean."

run: all
	./$(TARGET)

debug:
	@echo "Include paths: $(CXXFLAGS)"
	@find $(VCPKG_INSTALLED_DIR)/include -name "json.hpp" -o -name "*.json*"
	@echo "Library paths: $(LDFLAGS)"
	@echo "Libraries: $(LIBS)"
	@echo "Frameworks: $(FRAMEWORKS)"

.PHONY: clean all run dependencies debug
