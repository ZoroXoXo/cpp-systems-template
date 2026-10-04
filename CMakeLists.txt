cmake_minimum_required(VERSION 3.20)
project(HighPerfTemplate CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fast LLVM LLD linker
add_link_options(-fuse-ld=lld)

# Build Type Flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# Sanitizer Options
option(ENABLE_ASAN "Enable AddressSanitizer & UndefinedBehaviorSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)

if(ENABLE_ASAN)
    message(STATUS "Configuring AddressSanitizer + UBSan")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
elseif(ENABLE_TSAN)
    message(STATUS "Configuring ThreadSanitizer")
    add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
    add_link_options(-fsanitize=thread)
endif()

find_package(benchmark REQUIRED)

include_directories(include)

set(MY_PROJECT_WARNINGS
    -Wall 
    -Wextra 
    -Wpedantic 
    -Wshadow 
    -Wnon-virtual-dtor 
    -Wold-style-cast 
    -Wcast-align 
    -Wunused 
    -Woverloaded-virtual
)

# Executables
add_executable(app src/main.cpp)
target_compile_options(app PRIVATE ${MY_PROJECT_WARNINGS})

add_executable(bench_runner benchmarks/bench_main.cpp)
target_compile_options(bench_runner PRIVATE ${MY_PROJECT_WARNINGS})
target_link_libraries(bench_runner PRIVATE benchmark::benchmark)

add_executable(lab1_runner benchmarks/lab1_false_sharing.cpp)
target_compile_options(lab1_runner PRIVATE ${MY_PROJECT_WARNINGS})
target_link_libraries(lab1_runner PRIVATE benchmark::benchmark)

add_executable(lab2_runner benchmarks/lab2_memory_orders.cpp)
target_compile_options(lab2_runner PRIVATE ${MY_PROJECT_WARNINGS})
target_link_libraries(lab2_runner PRIVATE benchmark::benchmark)

# Phase 3: SPSC Queue Benchmark
add_executable(bench_spsc benchmarks/bench_spsc.cpp)
target_compile_options(bench_spsc PRIVATE ${MY_PROJECT_WARNINGS})
target_link_libraries(bench_spsc PRIVATE benchmark::benchmark)
