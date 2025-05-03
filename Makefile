CXX = g++
CXXFLAGS = -Wall -Wextra -Iinclude -Iinclude/Sockets -Iinclude/APIs -std=c++17
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
RELEASE_DIR = release
PROGRAM_NAME = proxy-server
RELEASE_TARGET = $(RELEASE_DIR)/$(PROGRAM_NAME)
DEBUG_TARGET = $(BIN_DIR)/main
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
else ifeq ($(UNAME_S),Linux)
  VCPKG_TRIPLET = x64-linux
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
			echo "Missing: $$pkg"; \
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

DEBUG_LIBS = -lcurl -lsqlite3 -lz -pthread

RELEASE_LDFLAGS = -static
RELEASE_LIBS = -Wl,-Bstatic -lcurl -lsqlite3 -lnghttp2 -lssl -lcrypto -lz -Wl,-Bdynamic -ldl -pthread

all: dependencies json-check $(DEBUG_TARGET)

json-check:
	@echo "Checking nlohmann/json.hpp header..."
	@if [ -f "$(VCPKG_INSTALLED_DIR)/include/nlohmann/json.hpp" ]; then \
		echo "JSON header found at $(VCPKG_INSTALLED_DIR)/include/nlohmann/json.hpp"; \
	else \
		echo "JSON header not found! Checking alternative locations..."; \
		find $(VCPKG_INSTALLED_DIR)/include -name "json.hpp" | grep .; \
	fi

$(DEBUG_TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(DEBUG_LIBS)
	@echo "Debug build compiled: $(DEBUG_TARGET)"

release: dependencies json-check release-build

release-build: $(OBJS)
	@mkdir -p $(RELEASE_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $(RELEASE_TARGET) $(LDFLAGS) $(RELEASE_LDFLAGS) $(RELEASE_LIBS)
	strip --strip-all $(RELEASE_TARGET)
	@echo "Release build compiled: $(RELEASE_TARGET)"
	
package: release
	@echo "Creating distribution package..."
	@mkdir -p $(RELEASE_DIR)/$(PROGRAM_NAME)
	@cp $(RELEASE_TARGET) $(RELEASE_DIR)/$(PROGRAM_NAME)/
	@if [ -d "config" ]; then cp -r config $(RELEASE_DIR)/$(PROGRAM_NAME)/; fi
	@echo '#!/bin/bash\necho "Installing $(PROGRAM_NAME)..."\nmkdir -p ~/.local/bin\ncp $(PROGRAM_NAME) ~/.local/bin/\necho "Installed to ~/.local/bin/$(PROGRAM_NAME)"' > $(RELEASE_DIR)/$(PROGRAM_NAME)/install.sh
	@chmod +x $(RELEASE_DIR)/$(PROGRAM_NAME)/install.sh
	@cd $(RELEASE_DIR) && tar -czvf $(PROGRAM_NAME)-linux-x64.tar.gz $(PROGRAM_NAME)
	@echo "Package created: $(RELEASE_DIR)/$(PROGRAM_NAME)-linux-x64.tar.gz"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(RELEASE_DIR)
	@echo "Cleaned."

run: all
	./$(DEBUG_TARGET)

debug-info:
	@echo "Include paths: $(CXXFLAGS)"
	@echo "JSON header location check:"
	@find $(VCPKG_INSTALLED_DIR)/include -name "json.hpp" -o -name "*.json*"
	@echo "Library paths: $(LDFLAGS)"
	@echo "Libraries: $(DEBUG_LIBS)"
	@echo "Try compilation with verbose output:"
	@echo '  make clean && make VERBOSE=1'
	@ldd $(DEBUG_TARGET) 2>/dev/null || echo "No dynamic dependencies found or ldd failed"

.PHONY: clean all run dependencies release release-build package debug-info json-check
