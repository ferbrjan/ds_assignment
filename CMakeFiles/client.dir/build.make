# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.19.6/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.19.6/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/stanislawczembor/Desktop/ds/ds_assignment

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/stanislawczembor/Desktop/ds/ds_assignment

# Utility rule file for client.

# Include the progress variables for this target.
include CMakeFiles/client.dir/progress.make

CMakeFiles/client: client.jar


client.jar: java-getopt-1.0.14.jar
client.jar: CMakeFiles/client.dir/java_class_filelist
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/stanislawczembor/Desktop/ds/ds_assignment/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Creating Java archive client.jar"
	cd /Users/stanislawczembor/Desktop/ds/ds_assignment/CMakeFiles/client.dir && /Library/Java/JavaVirtualMachines/adoptopenjdk-11.jdk/Contents/Home/bin/jar -cfm /Users/stanislawczembor/Desktop/ds/ds_assignment/client.jar /Users/stanislawczembor/Desktop/ds/ds_assignment/manifest.mf @java_class_filelist
	cd /Users/stanislawczembor/Desktop/ds/ds_assignment/CMakeFiles/client.dir && /usr/local/Cellar/cmake/3.19.6/bin/cmake -D_JAVA_TARGET_DIR=/Users/stanislawczembor/Desktop/ds/ds_assignment -D_JAVA_TARGET_OUTPUT_NAME=client.jar -D_JAVA_TARGET_OUTPUT_LINK= -P /usr/local/Cellar/cmake/3.19.6/share/cmake/Modules/UseJavaSymlinks.cmake

CMakeFiles/client.dir/java_class_filelist: CMakeFiles/client.dir/java_compiled_client
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/stanislawczembor/Desktop/ds/ds_assignment/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating CMakeFiles/client.dir/java_class_filelist"
	/usr/local/Cellar/cmake/3.19.6/bin/cmake -DCMAKE_JAVA_CLASS_OUTPUT_PATH=/Users/stanislawczembor/Desktop/ds/ds_assignment/CMakeFiles/client.dir -DCMAKE_JAR_CLASSES_PREFIX= -P /usr/local/Cellar/cmake/3.19.6/share/cmake/Modules/UseJavaClassFilelist.cmake

CMakeFiles/client.dir/java_compiled_client: apps/client.java
CMakeFiles/client.dir/java_compiled_client: java-getopt-1.0.14.jar
CMakeFiles/client.dir/java_compiled_client: CMakeFiles/client.dir/java_sources
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/stanislawczembor/Desktop/ds/ds_assignment/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building Java objects for client.jar"
	/usr/local/Cellar/cmake/3.19.6/bin/cmake -DCMAKE_JAVA_CLASS_OUTPUT_PATH=/Users/stanislawczembor/Desktop/ds/ds_assignment/CMakeFiles/client.dir -DCMAKE_JAR_CLASSES_PREFIX= -P /usr/local/Cellar/cmake/3.19.6/share/cmake/Modules/UseJava/ClearClassFiles.cmake
	/Library/Java/JavaVirtualMachines/adoptopenjdk-11.jdk/Contents/Home/bin/javac -classpath :/Users/stanislawczembor/Desktop/ds/ds_assignment:/Users/stanislawczembor/Desktop/ds/ds_assignment:java-getopt-1.0.14.jar -d /Users/stanislawczembor/Desktop/ds/ds_assignment/CMakeFiles/client.dir @/Users/stanislawczembor/Desktop/ds/ds_assignment/CMakeFiles/client.dir/java_sources
	/usr/local/Cellar/cmake/3.19.6/bin/cmake -E touch /Users/stanislawczembor/Desktop/ds/ds_assignment/CMakeFiles/client.dir/java_compiled_client

client: CMakeFiles/client
client: CMakeFiles/client.dir/java_class_filelist
client: CMakeFiles/client.dir/java_compiled_client
client: client.jar
client: CMakeFiles/client.dir/build.make

.PHONY : client

# Rule to build all files generated by this target.
CMakeFiles/client.dir/build: client

.PHONY : CMakeFiles/client.dir/build

CMakeFiles/client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/client.dir/cmake_clean.cmake
.PHONY : CMakeFiles/client.dir/clean

CMakeFiles/client.dir/depend:
	cd /Users/stanislawczembor/Desktop/ds/ds_assignment && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/stanislawczembor/Desktop/ds/ds_assignment /Users/stanislawczembor/Desktop/ds/ds_assignment /Users/stanislawczembor/Desktop/ds/ds_assignment /Users/stanislawczembor/Desktop/ds/ds_assignment /Users/stanislawczembor/Desktop/ds/ds_assignment/CMakeFiles/client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/client.dir/depend

