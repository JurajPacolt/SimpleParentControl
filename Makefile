#################################################################
## This Makefile Exported by MinGW Developer Studio
## Copyright (c) 2002-2004 by Parinya Thipchart
#################################################################

ifneq (,$(findstring Release, $(CFG)))
  override CFG = Release
else
  override CFG = Debug
endif

PROJECT = SimpleParentControl
CC = "g++"

ifeq ($(CFG),Debug)
  OBJ_DIR = obj/Debug/src
  OUTPUT_DIR = bin/Debug
  TARGET = SimpleParentControl
  C_INCLUDE_DIRS = -I"include" 
  C_PREPROC = 
  CFLAGS = -pipe  -Wall -g2 -O0 -frtti -fexceptions 
  RC_INCLUDE_DIRS = 
  RC_PREPROC = 
  RCFLAGS = 
  LIB_DIRS = 
  LIBS = 
  LDFLAGS = -pipe 
endif

ifeq ($(CFG),Release)
  OBJ_DIR = obj/Release/src
  OUTPUT_DIR = bin/Release
  TARGET = SimpleParentControl
  C_INCLUDE_DIRS = -I"include" 
  C_PREPROC = 
  CFLAGS = -pipe  -Wall -g0 -O2 -frtti -fexceptions 
  RC_INCLUDE_DIRS = 
  RC_PREPROC = 
  RCFLAGS = 
  LIB_DIRS = 
  LIBS = 
  LDFLAGS = -pipe -s 
endif

ifeq ($(OS),Windows_NT)
  NULL =
else
  NULL = nul
endif

SRC_OBJS = \
  $(OBJ_DIR)/MySimpleCrypto.o	\
  $(OBJ_DIR)/ConfigFile.o	\
  $(OBJ_DIR)/DataFile.o	\
  $(OBJ_DIR)/ParentControl.o	\
  $(OBJ_DIR)/Utils.o

define build_target
@echo Linking...
@$(CC) -o "$(OUTPUT_DIR)/$(TARGET)" $(SRC_OBJS) $(LIB_DIRS) $(LIBS) $(LDFLAGS)
endef

define compile_source
@echo Compiling $<
@$(CC) $(CFLAGS) $(C_PREPROC) $(C_INCLUDE_DIRS) -c "$<" -o "$@"
endef

.PHONY: print_header directories

$(TARGET): print_header directories $(SRC_OBJS)
	$(build_target)

.PHONY: clean cleanall

cleanall:
	@echo Deleting intermediate files for 'ParentControl - $(CFG)'
	-@rm -f $(OBJ_DIR)/*.o
	-@rm -f "$(OUTPUT_DIR)/$(TARGET)"
	-@rm -rf "$(OUTPUT_DIR)"

clean:
	@echo Deleting intermediate files for 'ParentControl - $(CFG)'
	-@rm -f $(OBJ_DIR)/*.o

print_header:
	@echo ----------Configuration: ParentControl - $(CFG)----------

directories:
	-@mkdir -p "$(OUTPUT_DIR)"
	-@mkdir -p "$(OBJ_DIR)"

$(OBJ_DIR)/MySimpleCrypto.o: src/MySimpleCrypto.cpp

	$(compile_source)

$(OBJ_DIR)/ConfigFile.o: src/ConfigFile.cpp

	$(compile_source)

$(OBJ_DIR)/DataFile.o: src/DataFile.cpp

	$(compile_source)

$(OBJ_DIR)/ParentControl.o: src/Main.cpp

	$(compile_source)

$(OBJ_DIR)/Utils.o: src/Utils.cpp

	$(compile_source)

