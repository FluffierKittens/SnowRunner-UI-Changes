﻿cmake_minimum_required (VERSION 3.8)

project (SnowRunner-UI-Changes VERSION 0.1.0 LANGUAGES CXX)

if(NOT MSVC OR NOT ${MSVC_VERSION} VERSION_EQUAL 1935)
  message(FATAL_ERROR "Project has only been tested on Visual Studio 17 (2022), with MSVC 19.35 compiler")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Uncomment below line to compile a version without the debug console
#add_compile_definitions(CM_NO_CONSOLE)

add_subdirectory(DLL)
add_subdirectory(Injector)