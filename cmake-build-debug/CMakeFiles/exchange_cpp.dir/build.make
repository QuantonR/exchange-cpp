# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_COMMAND = /cygdrive/c/Users/Riccardo/AppData/Local/JetBrains/CLion2021.2/cygwin_cmake/bin/cmake.exe

# The command to remove a file.
RM = /cygdrive/c/Users/Riccardo/AppData/Local/JetBrains/CLion2021.2/cygwin_cmake/bin/cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /cygdrive/c/exchange-cpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /cygdrive/c/exchange-cpp/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/exchange_cpp.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/exchange_cpp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/exchange_cpp.dir/flags.make

CMakeFiles/exchange_cpp.dir/main.cpp.o: CMakeFiles/exchange_cpp.dir/flags.make
CMakeFiles/exchange_cpp.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/c/exchange-cpp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/exchange_cpp.dir/main.cpp.o"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/exchange_cpp.dir/main.cpp.o -c /cygdrive/c/exchange-cpp/main.cpp

CMakeFiles/exchange_cpp.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exchange_cpp.dir/main.cpp.i"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/c/exchange-cpp/main.cpp > CMakeFiles/exchange_cpp.dir/main.cpp.i

CMakeFiles/exchange_cpp.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exchange_cpp.dir/main.cpp.s"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/c/exchange-cpp/main.cpp -o CMakeFiles/exchange_cpp.dir/main.cpp.s

CMakeFiles/exchange_cpp.dir/src/Book.cpp.o: CMakeFiles/exchange_cpp.dir/flags.make
CMakeFiles/exchange_cpp.dir/src/Book.cpp.o: ../src/Book.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/c/exchange-cpp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/exchange_cpp.dir/src/Book.cpp.o"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/exchange_cpp.dir/src/Book.cpp.o -c /cygdrive/c/exchange-cpp/src/Book.cpp

CMakeFiles/exchange_cpp.dir/src/Book.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exchange_cpp.dir/src/Book.cpp.i"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/c/exchange-cpp/src/Book.cpp > CMakeFiles/exchange_cpp.dir/src/Book.cpp.i

CMakeFiles/exchange_cpp.dir/src/Book.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exchange_cpp.dir/src/Book.cpp.s"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/c/exchange-cpp/src/Book.cpp -o CMakeFiles/exchange_cpp.dir/src/Book.cpp.s

CMakeFiles/exchange_cpp.dir/src/Order.cpp.o: CMakeFiles/exchange_cpp.dir/flags.make
CMakeFiles/exchange_cpp.dir/src/Order.cpp.o: ../src/Order.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/c/exchange-cpp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/exchange_cpp.dir/src/Order.cpp.o"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/exchange_cpp.dir/src/Order.cpp.o -c /cygdrive/c/exchange-cpp/src/Order.cpp

CMakeFiles/exchange_cpp.dir/src/Order.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exchange_cpp.dir/src/Order.cpp.i"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/c/exchange-cpp/src/Order.cpp > CMakeFiles/exchange_cpp.dir/src/Order.cpp.i

CMakeFiles/exchange_cpp.dir/src/Order.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exchange_cpp.dir/src/Order.cpp.s"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/c/exchange-cpp/src/Order.cpp -o CMakeFiles/exchange_cpp.dir/src/Order.cpp.s

CMakeFiles/exchange_cpp.dir/src/Limit.cpp.o: CMakeFiles/exchange_cpp.dir/flags.make
CMakeFiles/exchange_cpp.dir/src/Limit.cpp.o: ../src/Limit.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/c/exchange-cpp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/exchange_cpp.dir/src/Limit.cpp.o"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/exchange_cpp.dir/src/Limit.cpp.o -c /cygdrive/c/exchange-cpp/src/Limit.cpp

CMakeFiles/exchange_cpp.dir/src/Limit.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exchange_cpp.dir/src/Limit.cpp.i"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/c/exchange-cpp/src/Limit.cpp > CMakeFiles/exchange_cpp.dir/src/Limit.cpp.i

CMakeFiles/exchange_cpp.dir/src/Limit.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exchange_cpp.dir/src/Limit.cpp.s"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/c/exchange-cpp/src/Limit.cpp -o CMakeFiles/exchange_cpp.dir/src/Limit.cpp.s

# Object files for target exchange_cpp
exchange_cpp_OBJECTS = \
"CMakeFiles/exchange_cpp.dir/main.cpp.o" \
"CMakeFiles/exchange_cpp.dir/src/Book.cpp.o" \
"CMakeFiles/exchange_cpp.dir/src/Order.cpp.o" \
"CMakeFiles/exchange_cpp.dir/src/Limit.cpp.o"

# External object files for target exchange_cpp
exchange_cpp_EXTERNAL_OBJECTS =

exchange_cpp.exe: CMakeFiles/exchange_cpp.dir/main.cpp.o
exchange_cpp.exe: CMakeFiles/exchange_cpp.dir/src/Book.cpp.o
exchange_cpp.exe: CMakeFiles/exchange_cpp.dir/src/Order.cpp.o
exchange_cpp.exe: CMakeFiles/exchange_cpp.dir/src/Limit.cpp.o
exchange_cpp.exe: CMakeFiles/exchange_cpp.dir/build.make
exchange_cpp.exe: CMakeFiles/exchange_cpp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cygdrive/c/exchange-cpp/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable exchange_cpp.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/exchange_cpp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/exchange_cpp.dir/build: exchange_cpp.exe
.PHONY : CMakeFiles/exchange_cpp.dir/build

CMakeFiles/exchange_cpp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/exchange_cpp.dir/cmake_clean.cmake
.PHONY : CMakeFiles/exchange_cpp.dir/clean

CMakeFiles/exchange_cpp.dir/depend:
	cd /cygdrive/c/exchange-cpp/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cygdrive/c/exchange-cpp /cygdrive/c/exchange-cpp /cygdrive/c/exchange-cpp/cmake-build-debug /cygdrive/c/exchange-cpp/cmake-build-debug /cygdrive/c/exchange-cpp/cmake-build-debug/CMakeFiles/exchange_cpp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/exchange_cpp.dir/depend

