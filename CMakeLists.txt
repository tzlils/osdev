cmake_minimum_required(VERSION 3.0.0)

project(osdev
LANGUAGES C ASM_NASM CXX
VERSION 1.0.0)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang)
set(CMAKE_ASM_NASM_COMPILER nasm)
# set(CMAKE_LINKER "ld")

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/src/linker.ld)

string(TIMESTAMP BUILD_DATE '"%Y-%m-%d %H:%M"')
set(LIB_VERSION_STRING '"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}"')

set_source_files_properties(limine PROPERTIES GENERATED true)
add_custom_target(limine
	COMMAND make CC="${CMAKE_C_COMPILER}" TOOLCHAIN="llvm" TOOLCHAIN_CC="${CMAKE_C_COMPILER}" TOOLCHAIN_LD="${CMAKE_LINKER}"
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/limine
	BYPRODUCTS ${CMAKE_SOURCE_DIR}/limine/limine-install
)

file(GLOB C_SRCS src/*.c src/*/*.c)
file(GLOB CXX_SRCS src/*.cpp src/*/*.cpp)
file(GLOB S_SRCS src/*.nasm src/*/*.nasm)
add_executable(osdev ${C_SRCS} ${S_SRCS} ${CXX_SRCS})
add_dependencies(osdev limine)
target_include_directories(osdev PRIVATE /usr/include ${CMAKE_SOURCE_DIR}/include)
set_target_properties(osdev PROPERTIES OUTPUT_NAME "osdev.elf")
set_target_properties(osdev PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

add_custom_target( ISO ALL
	COMMAND ${CMAKE_SOURCE_DIR}/build-iso.sh $<TARGET_FILE:osdev> ${CMAKE_BINARY_DIR}/image.iso &> /dev/null
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS osdev
	BYPRODUCTS ${CMAKE_BINARY_DIR}/image.iso
)

set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -f elf64 -o <OBJECT> <SOURCE>")
set(SHARED_CXX_FLAGS "\
-DBUILD_DATE=${BUILD_DATE} \
-DBUILD_VERSION='\"${CMAKE_PROJECT_VERSION}\"' \
--target=x86_64-pc-none-elf \
-march=x86-64 \
-mno-80387 \
-mno-mmx \
-mno-3dnow \
-mno-sse \
-mno-sse2 \
-Wall \
-Wextra \
-O2 \
-pipe \
-ffreestanding \
-fno-stack-protector \
-fpie \
-Xclang \
-Wno-varargs \
-fcolor-diagnostics")

set(CMAKE_C_FLAGS " \
${SHARED_CXX_FLAGS} \
-std=gnu99 \
")

set(CMAKE_CXX_FLAGS " \
${SHARED_CXX_FLAGS} \
-std=c++20 \
-fmodules \
-fmodules-ts")

# needed to remove -rdynamic from link list
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "\
-T${LINKER_SCRIPT} \
-nostdlib \
-z max-page-size=0x1000 \
-static \
-pie")