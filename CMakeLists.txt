cmake_minimum_required(VERSION 3.28...3.30)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version})

option(ENABLE_FRONTEND_API "Use obs-frontend-api for UI functionality" ON)
option(ENABLE_QT "Use Qt functionality" ON)

include(compilerconfig)
include(defaults)
include(helpers)

add_library(${CMAKE_PROJECT_NAME} MODULE)

find_package(libobs REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)

if(ENABLE_FRONTEND_API)
  find_package(obs-frontend-api REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::obs-frontend-api Qt6::Widgets)
endif()

if(ENABLE_QT)
  find_package(Qt6 COMPONENTS Widgets Core)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)
  target_compile_options(
    ${CMAKE_PROJECT_NAME}
    PRIVATE $<$<C_COMPILER_ID:Clang,AppleClang>:-Wno-quoted-include-in-framework-header -Wno-comma>
  )
  set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON
  )
endif()

target_sources(${CMAKE_PROJECT_NAME} PRIVATE src/plugin-main.cpp)

set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})

# =====================================================
# Deploy automático a OBS
# =====================================================

# =====================================================
# Deploy automático a OBS
# =====================================================

set(OBS_INSTALL_DIR "C:/Program Files/obs-studio")
set(RUNDIR "${CMAKE_BINARY_DIR}/rundir/$<CONFIG>")

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD

    # Copiar DLL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${RUNDIR}/quick-docks.dll"
        "${OBS_INSTALL_DIR}/obs-plugins/64bit"

    # Copiar carpeta del plugin
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${RUNDIR}/quick-docks"
        "${OBS_INSTALL_DIR}/data/obs-plugins/quick-docks"

    COMMENT "Deploying quick-docks to OBS..."
)
