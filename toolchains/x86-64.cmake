set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86-64)
set(CMAKE_CROSSCOMPILING TRUE)

set(triple "x86-64")
set(CMAKE_C_COMPILER clang)
#set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_ASM_NASM_COMPILER nasm)
set(CMAKE_LINKER ld.lld)

set(CMAKE_CXX_FLAGS_INIT "\
-mno-red-zone \
-march=x86-64 \
-mno-80387 \
-mno-mmx \
-mno-3dnow \
-mno-sse \
-mno-sse2")
