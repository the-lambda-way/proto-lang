# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.15

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = C:\Dev\Programs\CMake\bin\cmake.exe

# The command to remove a file.
RM = C:\Dev\Programs\CMake\bin\cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Dev\Projects\languages\proto\repo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Dev\Projects\languages\proto\repo\build

# Include any dependencies generated for this target.
include CMakeFiles/proto.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/proto.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/proto.dir/flags.make

CMakeFiles/proto.dir/main.cpp.obj: CMakeFiles/proto.dir/flags.make
CMakeFiles/proto.dir/main.cpp.obj: CMakeFiles/proto.dir/includes_CXX.rsp
CMakeFiles/proto.dir/main.cpp.obj: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Dev\Projects\languages\proto\repo\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/proto.dir/main.cpp.obj"
	C:\Dev\Programs\mingw-w64\mingw64\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\proto.dir\main.cpp.obj -c C:\Dev\Projects\languages\proto\repo\main.cpp

CMakeFiles/proto.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proto.dir/main.cpp.i"
	C:\Dev\Programs\mingw-w64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Dev\Projects\languages\proto\repo\main.cpp > CMakeFiles\proto.dir\main.cpp.i

CMakeFiles/proto.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proto.dir/main.cpp.s"
	C:\Dev\Programs\mingw-w64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Dev\Projects\languages\proto\repo\main.cpp -o CMakeFiles\proto.dir\main.cpp.s

# Object files for target proto
proto_OBJECTS = \
"CMakeFiles/proto.dir/main.cpp.obj"

# External object files for target proto
proto_EXTERNAL_OBJECTS =

proto.exe: CMakeFiles/proto.dir/main.cpp.obj
proto.exe: CMakeFiles/proto.dir/build.make
proto.exe: CMakeFiles/proto.dir/linklibs.rsp
proto.exe: CMakeFiles/proto.dir/objects1.rsp
proto.exe: CMakeFiles/proto.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Dev\Projects\languages\proto\repo\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable proto.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\proto.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/proto.dir/build: proto.exe

.PHONY : CMakeFiles/proto.dir/build

CMakeFiles/proto.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\proto.dir\cmake_clean.cmake
.PHONY : CMakeFiles/proto.dir/clean

CMakeFiles/proto.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Dev\Projects\languages\proto\repo C:\Dev\Projects\languages\proto\repo C:\Dev\Projects\languages\proto\repo\build C:\Dev\Projects\languages\proto\repo\build C:\Dev\Projects\languages\proto\repo\build\CMakeFiles\proto.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/proto.dir/depend

