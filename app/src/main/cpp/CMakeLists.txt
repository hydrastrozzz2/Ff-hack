cmake_minimum_required(VERSION 3.18.1)
project("ffhack")

# Dobby സോഴ്സ് കോഡ് ഇവിടെ ചേർക്കുന്നു
add_subdirectory(dobby)

add_library(native-lib SHARED native-lib.cpp)

target_include_directories(native-lib PRIVATE 
    ${CMAKE_SOURCE_DIR}/imgui 
    ${CMAKE_SOURCE_DIR}/dobby/include
)

file(GLOB IMGUI_SOURCES ${CMAKE_SOURCE_DIR}/imgui/*.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})

# Dobby ലൈബ്രറിയിലേക്ക് ലിങ്ക് ചെയ്യുന്നു
target_link_libraries(native-lib 
    imgui 
    android 
    log 
    EGL 
    GLESv3 
    dobby
)
