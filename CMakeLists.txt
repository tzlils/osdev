cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_C_COMPILER_WORKS 1)

enable_language(ASM_NASM)
project(osdev C ASM_NASM)

set(CMAKE_C_COMPILER clang)
set(CXX clang)
set(CC clang)
set(CMAKE_ASM_COMPILER nasm)

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/src/linker.ld)
set(LINK_DEPENDS ${LINKER_SCRIPT})

string(TIMESTAMP BUILD_DATE '"%Y-%m-%d %H:%M"')
set(LIB_VERSION_STRING '"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}"')

set_source_files_properties(limine PROPERTIES GENERATED true)
add_custom_target(limine
	COMMAND make
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/limine)

file(GLOB C_SRCS src/*.c src/*/*.c)
file(GLOB S_SRCS src/*.nasm src/*/*.nasm)
add_executable(osdev ${C_SRCS} ${S_SRCS})
add_dependencies(osdev limine)
target_include_directories(osdev PRIVATE ${CMAKE_SOURCE_DIR}/include /usr/include)
set_target_properties(osdev PROPERTIES OUTPUT_NAME "osdev.elf")

add_custom_target( ISO ALL
	COMMAND ${CMAKE_SOURCE_DIR}/build-iso.sh &> /dev/null
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS osdev
)

set(CMAKE_LINKER "ld.ldd")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -f elf64 -o <OBJECT> <SOURCE>")
set(CMAKE_C_FLAGS "\
-I. \
--target=x86_64-pc-none-elf \
-march=x86-64 \
-std=gnu99 \
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
-fpie")

target_link_options(osdev PRIVATE "\
SHELL:-T${LINKER_SCRIPT} \
-fuse-ld=lld \
-MMD \
-nostdlib \
-z max-page-size=0x1000 \
-static \
-fpie")