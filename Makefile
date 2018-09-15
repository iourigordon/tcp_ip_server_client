#/bin/bash

TARGET_SRV = tcp_ip_server
TARGET_CLT = tcp_ip_client

SERVER_DIR = server
CLIENT_DIR = client

SERVER_HDR = $(wildcard $(SERVER_DIR)/*.h)
SERVER_SRC = $(wildcard $(SERVER_DIR)/*.cc)
CLIENT_SRC = $(wildcard $(CLIENT_DIR)/*.cc)

OBJ_DIR = obj
SERVER_OBJ_FILES = $(patsubst %.cc, $(OBJ_DIR)/%.o, $(notdir $(SERVER_SRC)))
CLIENT_OBJ_FILES = $(patsubst %.cc, $(OBJ_DIR)/%.o, $(notdir $(CLIENT_SRC)))

#CXXFLAGS += -g

all: $(TARGET_SRV) $(TARGET_CLT)

$(TARGET_SRV): $(OBJ_DIR) $(SERVER_OBJ_FILES)
	@echo "Linking " $@
	@$(CXX) $(CXXFLAGS) $(SERVER_OBJ_FILES) -o $@

$(TARGET_CLT): $(OBJ_DIR) $(CLIENT_OBJ_FILES)
	@echo "Linking " $@
	@$(CXX) $(CXXFLAGS) $(CLIENT_OBJ_FILES) -o $@

$(OBJ_DIR)/%.o: $(SERVER_DIR)/%.cc 
	@echo "Compiling " $^
	@$(CXX) $(CXXFLAGS) -c $^ -o $@

$(OBJ_DIR)/%.o: $(CLIENT_DIR)/%.cc
	@echo "Compiling " $^
	@$(CXX) $(CXXFLAGS) -c $^ -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

.PHONY: clean

clean:
	@rm -rf obj
	@rm -f $(TARGET_SRV)
	@rm -f $(TARGET_CLT)
