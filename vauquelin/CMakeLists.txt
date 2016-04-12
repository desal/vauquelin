# vauquelin sources.
set(VAUQUELIN_SRCS
  vauquelin_app.cc
  vauquelin_app.h
  vauquelin_handler.cc
  vauquelin_handler.h
  vauquelin_favicon.cc
  vauquelin_favicon.h
  vauquelin_v8.cc
  vauquelin_v8.h
  vauquelin.exe.manifest
  vauquelin.rc
  vauquelin_win.cc
  resource.h
  vauquelin_handler_win.cc
  )
APPEND_PLATFORM_SOURCES(VAUQUELIN_SRCS)
source_group(vauquelin FILES ${VAUQUELIN_SRCS})

set(VAUQUELIN_RES_SRCS_WINDOWS
  res/vauquelin.ico
  res/small.ico
  )
APPEND_PLATFORM_SOURCES(VAUQUELIN_RES_SRCS)
source_group(vauquelin\\\\res FILES ${VAUQUELIN_RES_SRCS})

set(VAUQUELIN_SRCS
  ${VAUQUELIN_SRCS}
  ${VAUQUELIN_RES_SRCS}
  )

APPEND_PLATFORM_SOURCES(VAUQUELIN_HELPER_SRCS)
source_group(vauquelin FILES ${VAUQUELIN_HELPER_SRCS})

set(VAUQUELIN_RESOURCES_SRCS
  )


# Target executable names.
set(CEF_TARGET "vauquelin")

# Logical target used to link the libcef library.
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

# Determine the target output directory.
SET_CEF_TARGET_OUT_DIR()

# Executable target.
add_executable(${CEF_TARGET} WIN32 ${VAUQUELIN_SRCS})
add_dependencies(${CEF_TARGET} cef)
ADD_LOGICAL_TARGET("libcef_dll_wrapper" "${CEF_LIB_WRAPPER_DEBUG}" "${CEF_LIB_WRAPPER_RELEASE}")
target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS} "secur32.lib")
#target_link_libraries(${CEF_TARGET} libcef_lib ${CEF_STANDARD_LIBS} "secur32.lib")

# Add the custom manifest files to the executable.
#ADD_WINDOWS_MANIFEST("${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_TARGET}")

# Copy binary and resource files to the target output directory.
COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")
