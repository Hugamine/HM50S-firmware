cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME HM50S-firmware)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
    Ex
    lvgl::lvgl
    lvgl_porting
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Lib/GCC/libarm_cortexM7lfsp_math.a
    # usbd
)
#--------------------Float--------------------

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -u _printf_float)

#--------------------FPU--------------------
# set(FPU_FLAGS -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard)

# add_compile_options(${FPU_FLAGS})
# add_link_options(${FPU_FLAGS})
#------------------------USBD-----------------------

# set(usbd_src
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Src/usbd_cdc_acm.c
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_cdc_acm_if.c
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/AUDIO_SPKR/Src/usbd_audio_spkr.c
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_audio_spkr_if.c
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Src/usbd_core.c
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Src/usbd_ctlreq.c
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Src/usbd_ioreq.c
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App/usb_device.c
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_desc.c
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Target/usbd_conf.c
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Src/usbd_composite.c
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/AUDIO_MIC/Src/usbd_audio_mic.c
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_audio_mic_if.c
# )
# set(usbd_inc
#     Middlewares/Third_Party/AL94_USB_Composite
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/AUDIO_MIC/Inc
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/AUDIO_COMMON
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Inc
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Inc
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Target
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/AUDIO_SPKR/Inc
#     Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Inc
# )

# add_library(usbd OBJECT)
# target_sources(usbd PRIVATE ${usbd_src})
# target_link_libraries(usbd PUBLIC stm32cubemx Ex)
# target_include_directories(stm32cubemx INTERFACE ${usbd_inc})

#-------------------Ex---------------------

# A user defined libraries for all external drivers in ./Ex
file(GLOB_RECURSE LIB_SOURCE 
    # ${CMAKE_CURRENT_SOURCE_DIR}/Ex/Src/adau1761.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Ex/Src/adf4351.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Ex/Src/audio.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Ex/Src/button_control.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Ex/Src/tlv320aic3204.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Ex/Src/ui.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Ex/Src/w25qxx.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Ex/Src/xpt2046.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Ex/Src/*.c
)
add_library(Ex ${LIB_SOURCE})
target_include_directories(Ex PUBLIC 
    Ex/Inc
    SStudio/
    Drivers/CMSIS/DSP/Include
)
target_link_libraries(Ex PRIVATE 
    stm32cubemx
    lvgl::lvgl
)

set(LV_BUILD_CONF_DIR ${PROJECT_SOURCE_DIR}/lvgl CACHE STRING "" FORCE)
add_subdirectory(lvgl)

add_library(lvgl_porting ${PROJECT_SOURCE_DIR}/lvgl/lv_port_indev.c)
target_include_directories(lvgl_porting PUBLIC Ex/Inc)
target_link_libraries(lvgl_porting PRIVATE
        stm32cubemx
        lvgl::lvgl
        Ex
)

target_link_libraries(Composite PUBLIC Ex)
