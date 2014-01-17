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

PROJECT_NAME = string-library

C_COMPILER = gcc
C_COMPILER_FLAGS = -Wall -std=c11

CPP_COMPILER = g++
CPP_COMPILER_FLAGS = -Wall -std=c++11

MAKE = mingw32-make

BUILD = bin
OBJECTS = obj

DEP = dep
SOURCE = src
TEST = test

STRING_INCLUDE = $(DEP)/StringLibrary/src
CATCH_INCLUDE = $(STRING_INCLUDE)/../Catch/include

STRING_LIB = $(DEP)/StringLibrary/bin

INCLUDES =
LIB_PATH = -L$(BUILD)
LIBS = -l$(PROJECT_NAME)

ERROR_OUTPUT = 2> error.log

C_COMPILE_COMMAND = $(C_COMPILER) $(C_COMPILER_FLAGS)

CPP_COMPILE_COMMAND = $(CPP_COMPILER) $(CPP_COMPILER_FLAGS)


clean:
	rm -rf $(OBJECTS)
	rm -rf $(BUILD)
	mkdir $(OBJECTS)
	mkdir $(BUILD)

string-library:
	cd $(STRING_INCLUDE) && cd .. && $(MAKE) static-library

lexer.o:
	$(C_COMPILE_COMMAND) -I$(STRING_INCLUDE) -c $(SOURCE)/token.c -o $(OBJECTS)/token.o $(ERROR_OUTPUT)
	$(C_COMPILE_COMMAND) -I$(STRING_INCLUDE) -c $(SOURCE)/lexer.c -o $(OBJECTS)/lexer.o $(ERROR_OUTPUT)

keywords.o:
	$(C_COMPILE_COMMAND) -c $(SOURCE)/keywords.c -o $(OBJECTS)/keywords.o $(ERROR_OUTPUT)

parser.o:
	$(C_COMPILE_COMMAND) -I$(STRING_INCLUDE) -c $(SOURCE)/parser.c -o $(OBJECTS)/parser.o $(ERROR_OUTPUT)

main.o: lexer.o
	$(C_COMPILE_COMMAND) -I$(STRING_INCLUDE) -c $(SOURCE)/main.c -o $(OBJECTS)/main.o $(ERROR_OUTPUT)

executable: main.o string-library
	$(C_COMPILER) -L$(STRING_LIB) $(OBJECTS)/main.o $(OBJECTS)/lexer.o -lstring-library -o $(BUILD)/main $(ERROR_OUTPUT)

.PHONY: all
all: clean executable

test-lexer: clean lexer.o string-library
	$(CPP_COMPILE_COMMAND) -I$(CATCH_INCLUDE) -I$(STRING_INCLUDE) -c $(TEST)/token.cpp -o $(OBJECTS)/test-token.o $(ERROR_OUTPUT)
	$(CPP_COMPILER) -L$(STRING_LIB) $(OBJECTS)/test-token.o $(OBJECTS)/lexer.o $(OBJECTS)/token.o -lstring-library -o $(BUILD)/test-token $(ERROR_OUTPUT)
	@echo "--- running test-token ---" && cd $(BUILD) && ./test-token

test-parser: clean parser.o keywords.o lexer.o
	$(CPP_COMPILE_COMMAND) -I$(CATCH_INCLUDE) -I$(STRING_INCLUDE) -c $(TEST)/parser.cpp -o $(OBJECTS)/test-parser.o $(ERROR_OUTPUT)
	$(CPP_COMPILER) -L$(STRING_LIB) $(OBJECTS)/test-parser.o $(OBJECTS)/parser.o $(OBJECTS)/keywords.o $(OBJECTS)/lexer.o $(OBJECTS)/token.o -lstring-library -o $(BUILD)/test-parser $(ERROR_OUTPUT)
	@echo "--- running test-parser ---" && cd $(BUILD) && ./test-parser
