cmake_minimum_required(VERSION 3.10)

# Название проекта
project(cpp_1)

# Установить стандарт C++
set(CMAKE_CXX_STANDARD 20)

# Путь к заголовочным файлам Raylib (укажи свой путь)
include_directories(C:/raylib/w64devkit/x86_64-w64-mingw32/include)

# Путь к библиотекам Raylib (укажи свой путь)
link_directories(C:/raylib/w64devkit/x86_64-w64-mingw32/lib)

# Убедиться, что включаем только нужные исходные файлы (main.cpp, src/*.cpp)
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/main.cpp")

# Создаем исполняемый файл из всех найденных .cpp файлов
add_executable(cpp_1 ${SOURCES})

# Подключаем библиотеку raylib и нужные библиотеки Windows
target_link_libraries(cpp_1 raylib winmm gdi32)

# Указываем, что не нужно использовать временные файлы CMake (например, CompilerIdCXX)
set_target_properties(cpp_1 PROPERTIES LINKER_LANGUAGE CXX)
