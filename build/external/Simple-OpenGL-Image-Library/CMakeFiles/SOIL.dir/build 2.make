# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.24.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.24.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build"

# Include any dependencies generated for this target.
include external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/compiler_depend.make

# Include the progress variables for this target.
include external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/progress.make

# Include the compile flags for this target's objects.
include external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/flags.make

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/SOIL.c.o: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/flags.make
external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/SOIL.c.o: /Users/johnefthymiou/Documents/university/7th\ senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/SOIL.c
external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/SOIL.c.o: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/SOIL.c.o"
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/SOIL.c.o -MF CMakeFiles/SOIL.dir/src/SOIL.c.o.d -o CMakeFiles/SOIL.dir/src/SOIL.c.o -c "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/SOIL.c"

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/SOIL.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SOIL.dir/src/SOIL.c.i"
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/SOIL.c" > CMakeFiles/SOIL.dir/src/SOIL.c.i

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/SOIL.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SOIL.dir/src/SOIL.c.s"
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/SOIL.c" -o CMakeFiles/SOIL.dir/src/SOIL.c.s

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_DXT.c.o: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/flags.make
external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_DXT.c.o: /Users/johnefthymiou/Documents/university/7th\ senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/image_DXT.c
external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_DXT.c.o: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_DXT.c.o"
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_DXT.c.o -MF CMakeFiles/SOIL.dir/src/image_DXT.c.o.d -o CMakeFiles/SOIL.dir/src/image_DXT.c.o -c "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/image_DXT.c"

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_DXT.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SOIL.dir/src/image_DXT.c.i"
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/image_DXT.c" > CMakeFiles/SOIL.dir/src/image_DXT.c.i

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_DXT.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SOIL.dir/src/image_DXT.c.s"
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/image_DXT.c" -o CMakeFiles/SOIL.dir/src/image_DXT.c.s

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_helper.c.o: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/flags.make
external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_helper.c.o: /Users/johnefthymiou/Documents/university/7th\ senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/image_helper.c
external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_helper.c.o: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building C object external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_helper.c.o"
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_helper.c.o -MF CMakeFiles/SOIL.dir/src/image_helper.c.o.d -o CMakeFiles/SOIL.dir/src/image_helper.c.o -c "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/image_helper.c"

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_helper.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SOIL.dir/src/image_helper.c.i"
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/image_helper.c" > CMakeFiles/SOIL.dir/src/image_helper.c.i

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_helper.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SOIL.dir/src/image_helper.c.s"
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/image_helper.c" -o CMakeFiles/SOIL.dir/src/image_helper.c.s

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/stb_image_aug.c.o: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/flags.make
external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/stb_image_aug.c.o: /Users/johnefthymiou/Documents/university/7th\ senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/stb_image_aug.c
external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/stb_image_aug.c.o: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Building C object external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/stb_image_aug.c.o"
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/stb_image_aug.c.o -MF CMakeFiles/SOIL.dir/src/stb_image_aug.c.o.d -o CMakeFiles/SOIL.dir/src/stb_image_aug.c.o -c "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/stb_image_aug.c"

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/stb_image_aug.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SOIL.dir/src/stb_image_aug.c.i"
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/stb_image_aug.c" > CMakeFiles/SOIL.dir/src/stb_image_aug.c.i

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/stb_image_aug.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SOIL.dir/src/stb_image_aug.c.s"
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && /usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library/src/stb_image_aug.c" -o CMakeFiles/SOIL.dir/src/stb_image_aug.c.s

# Object files for target SOIL
SOIL_OBJECTS = \
"CMakeFiles/SOIL.dir/src/SOIL.c.o" \
"CMakeFiles/SOIL.dir/src/image_DXT.c.o" \
"CMakeFiles/SOIL.dir/src/image_helper.c.o" \
"CMakeFiles/SOIL.dir/src/stb_image_aug.c.o"

# External object files for target SOIL
SOIL_EXTERNAL_OBJECTS =

external/Simple-OpenGL-Image-Library/libSOIL_d.a: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/SOIL.c.o
external/Simple-OpenGL-Image-Library/libSOIL_d.a: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_DXT.c.o
external/Simple-OpenGL-Image-Library/libSOIL_d.a: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/image_helper.c.o
external/Simple-OpenGL-Image-Library/libSOIL_d.a: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/src/stb_image_aug.c.o
external/Simple-OpenGL-Image-Library/libSOIL_d.a: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/build.make
external/Simple-OpenGL-Image-Library/libSOIL_d.a: external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_5) "Linking C static library libSOIL_d.a"
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && $(CMAKE_COMMAND) -P CMakeFiles/SOIL.dir/cmake_clean_target.cmake
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SOIL.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/build: external/Simple-OpenGL-Image-Library/libSOIL_d.a
.PHONY : external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/build

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/clean:
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" && $(CMAKE_COMMAND) -P CMakeFiles/SOIL.dir/cmake_clean.cmake
.PHONY : external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/clean

external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/depend:
	cd "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense" "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/external/Simple-OpenGL-Image-Library" "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build" "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library" "/Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/build/external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : external/Simple-OpenGL-Image-Library/CMakeFiles/SOIL.dir/depend
