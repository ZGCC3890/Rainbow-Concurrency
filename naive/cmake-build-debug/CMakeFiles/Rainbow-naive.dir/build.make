# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.28

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\Jetbrain\CLion 2024.1.1\bin\cmake\win\x64\bin\cmake.exe"

# The command to remove a file.
RM = "D:\Jetbrain\CLion 2024.1.1\bin\cmake\win\x64\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = F:\JetBrains\Rainbow-Concurrency\naive

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = F:\JetBrains\Rainbow-Concurrency\naive\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Rainbow-naive.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Rainbow-naive.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Rainbow-naive.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Rainbow-naive.dir/flags.make

CMakeFiles/Rainbow-naive.dir/src/main.cpp.obj: CMakeFiles/Rainbow-naive.dir/flags.make
CMakeFiles/Rainbow-naive.dir/src/main.cpp.obj: F:/JetBrains/Rainbow-Concurrency/naive/src/main.cpp
CMakeFiles/Rainbow-naive.dir/src/main.cpp.obj: CMakeFiles/Rainbow-naive.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=F:\JetBrains\Rainbow-Concurrency\naive\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Rainbow-naive.dir/src/main.cpp.obj"
	"D:\Jetbrain\CLion 2024.1.1\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Rainbow-naive.dir/src/main.cpp.obj -MF CMakeFiles\Rainbow-naive.dir\src\main.cpp.obj.d -o CMakeFiles\Rainbow-naive.dir\src\main.cpp.obj -c F:\JetBrains\Rainbow-Concurrency\naive\src\main.cpp

CMakeFiles/Rainbow-naive.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/Rainbow-naive.dir/src/main.cpp.i"
	"D:\Jetbrain\CLion 2024.1.1\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E F:\JetBrains\Rainbow-Concurrency\naive\src\main.cpp > CMakeFiles\Rainbow-naive.dir\src\main.cpp.i

CMakeFiles/Rainbow-naive.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/Rainbow-naive.dir/src/main.cpp.s"
	"D:\Jetbrain\CLion 2024.1.1\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S F:\JetBrains\Rainbow-Concurrency\naive\src\main.cpp -o CMakeFiles\Rainbow-naive.dir\src\main.cpp.s

CMakeFiles/Rainbow-naive.dir/src/utils.cpp.obj: CMakeFiles/Rainbow-naive.dir/flags.make
CMakeFiles/Rainbow-naive.dir/src/utils.cpp.obj: F:/JetBrains/Rainbow-Concurrency/naive/src/utils.cpp
CMakeFiles/Rainbow-naive.dir/src/utils.cpp.obj: CMakeFiles/Rainbow-naive.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=F:\JetBrains\Rainbow-Concurrency\naive\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Rainbow-naive.dir/src/utils.cpp.obj"
	"D:\Jetbrain\CLion 2024.1.1\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Rainbow-naive.dir/src/utils.cpp.obj -MF CMakeFiles\Rainbow-naive.dir\src\utils.cpp.obj.d -o CMakeFiles\Rainbow-naive.dir\src\utils.cpp.obj -c F:\JetBrains\Rainbow-Concurrency\naive\src\utils.cpp

CMakeFiles/Rainbow-naive.dir/src/utils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/Rainbow-naive.dir/src/utils.cpp.i"
	"D:\Jetbrain\CLion 2024.1.1\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E F:\JetBrains\Rainbow-Concurrency\naive\src\utils.cpp > CMakeFiles\Rainbow-naive.dir\src\utils.cpp.i

CMakeFiles/Rainbow-naive.dir/src/utils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/Rainbow-naive.dir/src/utils.cpp.s"
	"D:\Jetbrain\CLion 2024.1.1\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S F:\JetBrains\Rainbow-Concurrency\naive\src\utils.cpp -o CMakeFiles\Rainbow-naive.dir\src\utils.cpp.s

# Object files for target Rainbow-naive
Rainbow__naive_OBJECTS = \
"CMakeFiles/Rainbow-naive.dir/src/main.cpp.obj" \
"CMakeFiles/Rainbow-naive.dir/src/utils.cpp.obj"

# External object files for target Rainbow-naive
Rainbow__naive_EXTERNAL_OBJECTS =

Rainbow-naive.exe: CMakeFiles/Rainbow-naive.dir/src/main.cpp.obj
Rainbow-naive.exe: CMakeFiles/Rainbow-naive.dir/src/utils.cpp.obj
Rainbow-naive.exe: CMakeFiles/Rainbow-naive.dir/build.make
Rainbow-naive.exe: CMakeFiles/Rainbow-naive.dir/linkLibs.rsp
Rainbow-naive.exe: CMakeFiles/Rainbow-naive.dir/objects1.rsp
Rainbow-naive.exe: CMakeFiles/Rainbow-naive.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=F:\JetBrains\Rainbow-Concurrency\naive\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable Rainbow-naive.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\Rainbow-naive.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Rainbow-naive.dir/build: Rainbow-naive.exe
.PHONY : CMakeFiles/Rainbow-naive.dir/build

CMakeFiles/Rainbow-naive.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\Rainbow-naive.dir\cmake_clean.cmake
.PHONY : CMakeFiles/Rainbow-naive.dir/clean

CMakeFiles/Rainbow-naive.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" F:\JetBrains\Rainbow-Concurrency\naive F:\JetBrains\Rainbow-Concurrency\naive F:\JetBrains\Rainbow-Concurrency\naive\cmake-build-debug F:\JetBrains\Rainbow-Concurrency\naive\cmake-build-debug F:\JetBrains\Rainbow-Concurrency\naive\cmake-build-debug\CMakeFiles\Rainbow-naive.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/Rainbow-naive.dir/depend
