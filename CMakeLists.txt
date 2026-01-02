cmake_minimum_required (VERSION 3.28)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project ("RenderSoft" LANGUAGES CXX)

# --------------------------------------- #
# ----- Fetch External Dependencies ----- #
# --------------------------------------- #
include(FetchContent)

FetchContent_Declare(
	Assimp
	GIT_REPOSITORY https://github.com/assimp/assimp.git
	GIT_TAG v6.0.2
	EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(Assimp)

FetchContent_Declare(
	GadgetCore
	GIT_REPOSITORY https://github.com/ShikenNuggets/GadgetCore.git
	GIT_TAG main
)
FetchContent_MakeAvailable(GadgetCore)

# --------------------------------------- #
# ------------ Setup Tooling ------------ #
# --------------------------------------- #

# Detect compiler
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(EXCEPTION_FLAG "/EHsc")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(EXCEPTION_FLAG "-fexceptions")
else()
	set(EXCEPTION_FLAG "")
endif()

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if (CLANG_TIDY_EXE)
	message(STATUS "clang-tidy checks enabled")
	set(CMAKE_CXX_CLANG_TIDY clang-tidy "-extra-arg=${EXCEPTION_FLAG}")
else()
	message(STATUS "clang-tidy not found, will not run checks")
endif()

# --------------------------------------- #
# ------------ Setup Target ------------- #
# --------------------------------------- #
file (GLOB_RECURSE SRC_FILES
	"src/*.cpp"
	"src/*.h"
	"src/*.hpp"
)

file (GLOB_RECURSE INC_FILES
	"include/*.h"
	"include/*.hpp"
)

add_executable (RenderSoft ${SRC_FILES} ${INC_FILES})

set_target_properties(RenderSoft PROPERTIES
	CXX_STANDARD 23
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)

target_compile_features(RenderSoft PUBLIC cxx_std_23)

target_include_directories(RenderSoft PUBLIC
	include
	${assimp_SOURCE_DIR}/include
)

target_link_libraries(RenderSoft
	PUBLIC
		SDL3::SDL3
		GadgetCore
)

# --------------------------------------- #
# ------ Platform Specific Config ------- #
# --------------------------------------- #
if (WIN32)
	target_compile_definitions(RenderSoft PUBLIC RS_PLATFORM_WIN32 WIN32_LEAN_AND_MEAN NOMINMAX UNICODE _UNICODE)

	# Link Windows system libraries required for your headers
	target_link_libraries(RenderSoft PUBLIC
		ole32
		oleaut32
		uuid
		shell32
		dwmapi
		user32
		gdi32
		advapi32
	)

elseif (APPLE)
	target_compile_definitions(RenderSoft PUBLIC RS_PLATFORM_MACOS)

elseif (UNIX AND NOT APPLE)
	target_compile_definitions(RenderSoft PUBLIC RS_PLATFORM_LINUX)

else()
	target_compile_definitions(RenderSoft PUBLIC RS_PLATFORM_UNKNOWN)

endif()

# --------------------------------------- #
# ----- Build Type Specific Config ------ #
# --------------------------------------- #
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(RenderSoft PUBLIC RS_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	target_compile_definitions(RenderSoft PUBLIC RS_RELEASE)
endif()
