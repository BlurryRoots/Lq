#
# Copyright (c) 2013, BlurryRoots aka Sven Freiberg
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


#
# basic setup
#

PROJECT_NAME = lq

#CC = gcc
CFLAGS = -Wall -std=c11 -g

#CXX = g++
CXXFLAGS = -Wall -std=c++11 -g

MAKE = make

BUILD = bin
OBJ = obj

DEP = dep
SRC = src
TEST = test

STRING = $(DEP)/StringLibrary
STRING_INCLUDE = $(DEP)/StringLibrary/src
CATCH_INCLUDE = $(STRING_INCLUDE)/../Catch/include

STRING_LIB = $(DEP)/StringLibrary/bin

INCLUDES =
LIB_PATH = -L$(BUILD)
LIBS = -l$(PROJECT_NAME)

ERROR_OUTPUT = 2> error.log

CC_CMD = $(CC) $(CFLAGS)

CXX_CMD = $(CXX) $(CXXFLAGS)

.PHONY: all
all: clean run

clean:
	rm -rf $(OBJ)
	rm -rf $(BUILD)
	mkdir $(OBJ)
	mkdir $(BUILD)

dependencies:
	$(MAKE) -C$(STRING) static-library

%.o:
	$(CC_CMD) -Iinc -I$(STRING_INCLUDE) \
		-c $(SRC)/$*.c \
		-o $(OBJ)/$*.o \
		$(ERROR_OUTPUT)

lib: clean dependencies keywords.o token.o lexer.o parser.o
	ar rvs $(BUILD)/lib$(PROJECT_NAME).a $(OBJ)/*.o

executable: lib
	$(CC_CMD) -Iinc -I$(STRING_INCLUDE) -Isrc \
		-c src/main.c \
		-o $(OBJ)/main.o \
		$(ERROR_OUTPUT)
	$(CC) $(CFLAGS) -L$(STRING_LIB) -Lbin \
		$(OBJ)/main.o \
		-lstring-library \
		-llq \
		-o $(BUILD)/lq-cli \
		$(ERROR_OUTPUT)

run: executable
	$(BUILD)/./lq-cli

grind: executable
	valgrind --leak-check=full --show-leak-kinds=all $(BUILD)/./lq-cli

test-lexer: clean dependencies lexer.o token.o keywords.o
	$(CXX_CMD) -I$(CATCH_INCLUDE) -Iinc -I$(STRING_INCLUDE) \
		-c $(TEST)/token.cpp \
		-o $(OBJ)/test-token.o \
		$(ERROR_OUTPUT)
	$(CXX) $(CXXFLAGS) -L$(STRING_LIB) \
		$(OBJ)/test-token.o \
		$(OBJ)/lexer.o \
		$(OBJ)/token.o \
		-lstring-library \
		-o $(BUILD)/test-token \
		$(ERROR_OUTPUT)
	@echo "--- running test-token ---" \
		&& cd $(BUILD) && ./test-token

test-parser: clean dependencies lib
	$(CXX_CMD) -I$(CATCH_INCLUDE) -Iinc -I$(STRING_INCLUDE) \
		-c $(TEST)/parser.cpp \
		-o $(OBJ)/test-parser.o $(ERROR_OUTPUT)
	$(CXX) $(CXXFLAGS) -L$(STRING_LIB) -Lbin \
		$(OBJ)/test-parser.o \
		-lstring-library \
		-llq \
		-o $(BUILD)/test-parser \
		$(ERROR_OUTPUT)
	@echo "--- running test-parser ---" \
		&& cd $(BUILD) && ./test-parser
