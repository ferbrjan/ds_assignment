# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.14.3/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.14.3/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/dinokfenicky/Desktop/Distributed/assignemt1/assigment1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/dinokfenicky/Desktop/Distributed/assignemt1/assigment1

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/local/Cellar/cmake/3.14.3/bin/cmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/local/Cellar/cmake/3.14.3/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/dinokfenicky/Desktop/Distributed/assignemt1/assigment1/CMakeFiles /Users/dinokfenicky/Desktop/Distributed/assignemt1/assigment1/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/dinokfenicky/Desktop/Distributed/assignemt1/assigment1/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named client

# Build rule for target.
client: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 client
.PHONY : client

# fast build rule for target.
client/fast:
	$(MAKE) -f CMakeFiles/client.dir/build.make CMakeFiles/client.dir/build
.PHONY : client/fast

#=============================================================================
# Target rules for targets named server

# Build rule for target.
server: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 server
.PHONY : server

# fast build rule for target.
server/fast:
	$(MAKE) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/build
.PHONY : server/fast

apps/server.o: apps/server.c.o

.PHONY : apps/server.o

# target to build an object file
apps/server.c.o:
	$(MAKE) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/apps/server.c.o
.PHONY : apps/server.c.o

apps/server.i: apps/server.c.i

.PHONY : apps/server.i

# target to preprocess a source file
apps/server.c.i:
	$(MAKE) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/apps/server.c.i
.PHONY : apps/server.c.i

apps/server.s: apps/server.c.s

.PHONY : apps/server.s

# target to generate assembly for a file
apps/server.c.s:
	$(MAKE) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/apps/server.c.s
.PHONY : apps/server.c.s

src/lines.o: src/lines.c.o

.PHONY : src/lines.o

# target to build an object file
src/lines.c.o:
	$(MAKE) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/src/lines.c.o
.PHONY : src/lines.c.o

src/lines.i: src/lines.c.i

.PHONY : src/lines.i

# target to preprocess a source file
src/lines.c.i:
	$(MAKE) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/src/lines.c.i
.PHONY : src/lines.c.i

src/lines.s: src/lines.c.s

.PHONY : src/lines.s

# target to generate assembly for a file
src/lines.c.s:
	$(MAKE) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/src/lines.c.s
.PHONY : src/lines.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... client"
	@echo "... server"
	@echo "... apps/server.o"
	@echo "... apps/server.i"
	@echo "... apps/server.s"
	@echo "... src/lines.o"
	@echo "... src/lines.i"
	@echo "... src/lines.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system
