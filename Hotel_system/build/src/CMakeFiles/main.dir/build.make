# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/os/桌面/linux_Final_Project/Hotel_system

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/os/桌面/linux_Final_Project/Hotel_system/build

# Include any dependencies generated for this target.
include src/CMakeFiles/main.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/main.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/main.dir/flags.make

src/CMakeFiles/main.dir/main.c.o: src/CMakeFiles/main.dir/flags.make
src/CMakeFiles/main.dir/main.c.o: ../src/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/os/桌面/linux_Final_Project/Hotel_system/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/CMakeFiles/main.dir/main.c.o"
	cd /home/os/桌面/linux_Final_Project/Hotel_system/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/main.dir/main.c.o   -c /home/os/桌面/linux_Final_Project/Hotel_system/src/main.c

src/CMakeFiles/main.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/main.dir/main.c.i"
	cd /home/os/桌面/linux_Final_Project/Hotel_system/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/os/桌面/linux_Final_Project/Hotel_system/src/main.c > CMakeFiles/main.dir/main.c.i

src/CMakeFiles/main.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/main.dir/main.c.s"
	cd /home/os/桌面/linux_Final_Project/Hotel_system/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/os/桌面/linux_Final_Project/Hotel_system/src/main.c -o CMakeFiles/main.dir/main.c.s

src/CMakeFiles/main.dir/systemInit.c.o: src/CMakeFiles/main.dir/flags.make
src/CMakeFiles/main.dir/systemInit.c.o: ../src/systemInit.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/os/桌面/linux_Final_Project/Hotel_system/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object src/CMakeFiles/main.dir/systemInit.c.o"
	cd /home/os/桌面/linux_Final_Project/Hotel_system/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/main.dir/systemInit.c.o   -c /home/os/桌面/linux_Final_Project/Hotel_system/src/systemInit.c

src/CMakeFiles/main.dir/systemInit.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/main.dir/systemInit.c.i"
	cd /home/os/桌面/linux_Final_Project/Hotel_system/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/os/桌面/linux_Final_Project/Hotel_system/src/systemInit.c > CMakeFiles/main.dir/systemInit.c.i

src/CMakeFiles/main.dir/systemInit.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/main.dir/systemInit.c.s"
	cd /home/os/桌面/linux_Final_Project/Hotel_system/build/src && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/os/桌面/linux_Final_Project/Hotel_system/src/systemInit.c -o CMakeFiles/main.dir/systemInit.c.s

# Object files for target main
main_OBJECTS = \
"CMakeFiles/main.dir/main.c.o" \
"CMakeFiles/main.dir/systemInit.c.o"

# External object files for target main
main_EXTERNAL_OBJECTS =

../bin/main: src/CMakeFiles/main.dir/main.c.o
../bin/main: src/CMakeFiles/main.dir/systemInit.c.o
../bin/main: src/CMakeFiles/main.dir/build.make
../bin/main: src/CMakeFiles/main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/os/桌面/linux_Final_Project/Hotel_system/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable ../../bin/main"
	cd /home/os/桌面/linux_Final_Project/Hotel_system/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/main.dir/build: ../bin/main

.PHONY : src/CMakeFiles/main.dir/build

src/CMakeFiles/main.dir/clean:
	cd /home/os/桌面/linux_Final_Project/Hotel_system/build/src && $(CMAKE_COMMAND) -P CMakeFiles/main.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/main.dir/clean

src/CMakeFiles/main.dir/depend:
	cd /home/os/桌面/linux_Final_Project/Hotel_system/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/os/桌面/linux_Final_Project/Hotel_system /home/os/桌面/linux_Final_Project/Hotel_system/src /home/os/桌面/linux_Final_Project/Hotel_system/build /home/os/桌面/linux_Final_Project/Hotel_system/build/src /home/os/桌面/linux_Final_Project/Hotel_system/build/src/CMakeFiles/main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/main.dir/depend

