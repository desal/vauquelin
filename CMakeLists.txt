
cmake_minimum_required(VERSION 2.8.12.2)
include(ExternalProject)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
project(vauquelin-build)
set_property(GLOBAL PROPERTY OS_FOLDERS ON)
set(OS_WINDOWS 1)
set(CEF_URL https://cefbuilds.s3.amazonaws.com/trunk/win64/48.0.2556.304f01a1/cef_binary_3.2556.1368.g535c4fb_windows64.7z)

ExternalProject_Add(
    cef
    URL ${CEF_URL}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/vendor/cef
    INSTALL_COMMAND ""
)

set(PROJECT_ARCH "x86_64")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
include("macros")

# Source include directory.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Source include CEF3 directory.
ExternalProject_Get_Property(cef SOURCE_DIR)
include_directories(${SOURCE_DIR})


# Magical set of compiler flags that makes building CEF happy
# See cef repo
add_definitions(-D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS)
set(CEF_LIBTYPE                 STATIC)
set(CEF_COMPILER_FLAGS          "/MP /Gy /GR- /Zi /W4 /WX /wd\"4100\" /wd\"4127\" /wd\"4244\" /wd\"4512\" /wd\"4701\" /wd\"4702\" /wd\"4996\"")
set(CEF_COMPILER_FLAGS_DEBUG    "/MTd /RTC1 /Od")
set(CEF_COMPILER_FLAGS_RELEASE  "/MT /O2 /Ob2 /GF /D NDEBUG /D _NDEBUG")
set(CEF_LINKER_FLAGS_DEBUG      "/DEBUG")
set(CEF_EXE_LINKER_FLAGS        "/MANIFEST:NO")

add_definitions(-DWIN32 -D_WIN32 -D_WINDOWS -DUNICODE -D_UNICODE -DWINVER=0x0602
			  -D_WIN32_WINNT=0x602 -DNOMINMAX -DWIN32_LEAN_AND_MEAN -D_HAS_EXCEPTIONS=0)

set(CEF_STANDARD_LIBS "comctl32.lib" "rpcrt4.lib" "shlwapi.lib")

ExternalProject_Get_Property(cef SOURCE_DIR)
set(CEF_RESOURCE_DIR        "${SOURCE_DIR}/Resources")
set(CEF_BINARY_DIR          "${SOURCE_DIR}/$<CONFIGURATION>")
set(CEF_BINARY_DIR_DEBUG    "${SOURCE_DIR}/Debug")
set(CEF_BINARY_DIR_RELEASE  "${SOURCE_DIR}/Release")

set(CEF_LIB_DEBUG   "${CEF_BINARY_DIR_DEBUG}/libcef.lib")
set(CEF_LIB_RELEASE "${CEF_BINARY_DIR_RELEASE}/libcef.lib")

ExternalProject_Get_Property(cef BINARY_DIR)
set(CEF_LIB_WRAPPER_DEBUG   "${BINARY_DIR}/libcef_dll/Debug/libcef_dll_wrapper.lib")
set(CEF_LIB_WRAPPER_RELEASE "${BINARY_DIR}/libcef_dll/Release/libcef_dll_wrapper.lib")

# List of CEF binary files.
set(CEF_BINARY_FILES
d3dcompiler_43.dll
d3dcompiler_47.dll
libcef.dll
libEGL.dll
libGLESv2.dll
widevinecdmadapter.dll
natives_blob.bin
snapshot_blob.bin
)

# List of CEF resource files.
set(CEF_RESOURCE_FILES
cef.pak
cef_100_percent.pak
cef_200_percent.pak
cef_extensions.pak
devtools_resources.pak
icudtl.dat
locales/am.pak
locales/ar.pak
locales/bg.pak
locales/bn.pak
locales/ca.pak
locales/cs.pak
locales/da.pak
locales/de.pak
locales/el.pak
locales/en-GB.pak
locales/en-US.pak
locales/es-419.pak
locales/es.pak
locales/et.pak
locales/fa.pak
locales/fi.pak
locales/fil.pak
locales/fr.pak
locales/gu.pak
locales/he.pak
locales/hi.pak
locales/hr.pak
locales/hu.pak
locales/id.pak
locales/it.pak
locales/ja.pak
locales/kn.pak
locales/ko.pak
locales/lt.pak
locales/lv.pak
locales/ml.pak
locales/mr.pak
locales/ms.pak
locales/nb.pak
locales/nl.pak
locales/pl.pak
locales/pt-BR.pak
locales/pt-PT.pak
locales/ro.pak
locales/ru.pak
locales/sk.pak
locales/sl.pak
locales/sr.pak
locales/sv.pak
locales/sw.pak
locales/ta.pak
locales/te.pak
locales/th.pak
locales/tr.pak
locales/uk.pak
locales/vi.pak
locales/zh-CN.pak
locales/zh-TW.pak
)

set(USE_SANDBOX OFF)
set(USE_ATL OFF)


#
# Post-configuration actions.
#

# Merge compiler/linker flags.
set(CMAKE_C_FLAGS                     "${CEF_COMPILER_FLAGS} ${CEF_C_COMPILER_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG               "${CEF_COMPILER_FLAGS_DEBUG} ${CEF_C_COMPILER_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE             "${CEF_COMPILER_FLAGS_RELEASE} ${CEF_C_COMPILER_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS                   "${CEF_COMPILER_FLAGS} ${CEF_CXX_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG             "${CEF_COMPILER_FLAGS_DEBUG} ${CEF_CXX_COMPILER_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE           "${CEF_COMPILER_FLAGS_RELEASE} ${CEF_CXX_COMPILER_FLAGS_RELEASE}")
set(CMAKE_EXE_LINKER_FLAGS            "${CEF_LINKER_FLAGS} ${CEF_EXE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG      "${CEF_LINKER_FLAGS_DEBUG} ${CEF_EXE_LINKER_FLAGS_DEBUG}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE    "${CEF_LINKER_FLAGS_RELEASE} ${CEF_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS         "${CEF_LINKER_FLAGS} ${CEF_SHARED_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG   "${CEF_LINKER_FLAGS_DEBUG} ${CEF_SHARED_LINKER_FLAGS_DEBUG}")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CEF_LINKER_FLAGS_RELEASE} ${CEF_SHARED_LINKER_FLAGS_RELEASE}")


#
# Include target subdirectories.
#

add_subdirectory(vauquelin)
set(LIBRARIES ${CEF_STANDARD_LIBS})
