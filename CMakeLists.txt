cmake_minimum_required(VERSION 3.5)
project(neatpp)

# Enable output of compile commands during generation.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Add the -fPIC flag using the line below
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Use python version that satisfies version constraints
set(Python_FIND_STRATEGY LOCATION)
# Make sure that python libraries such as numpy are found
find_package(Python 3 COMPONENTS Interpreter Development.Module NumPy)
# Use relative paths in MacOS
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# detect virtualenv and set Pip args accordingly
# if(DEFINED ENV{VIRTUAL_ENV} OR DEFINED ENV{CONDA_PREFIX})
#   set(_pip_args)
# else()
#   set(_pip_args "--user")
# endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Logging messages to debug possible Python issues
find_program(PYTHON "python3")
message(status " Python executable is ${PYTHON}")
message(status " Python Development Module found value is ${Python_Development.Module_FOUND}")
message(status " Python version is ${Python_VERSION}")

# Download git submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()


# IF(DEFINED ENV{CI})
#     message(STATUS "CI environment detected. Set compilation flags accordingly (target ivybridge which supports avx).")
#     set(CMAKE_CXX_FLAGS "-O3 -march=ivybridge -mfma -ffp-contract=fast")
# elseif(DEFINED ENV{CONDA_BUILD})
#     message(STATUS "conda build environment detected. Let conda set compilation flags accordingly.")
#     # set(CMAKE_CXX_FLAGS "-O3 -march=ivybridge -mfma -ffp-contract=fast")
# else()
#     message(STATUS "Local build detected. Set compilation flags accordingly (march=native).")
#     set(CMAKE_CXX_FLAGS "-O3 -march=native -mfma -ffp-contract=fast")
# endif()

# find_package(OpenMP)

# Add BOOST
find_package(Boost 1.71.0)
if(Boost_FOUND)
    message(STATUS "Boost version is ${Boost_VERSION_STRING}")
    message(STATUS "Boost include dirs are ${Boost_INCLUDE_DIRS}")
else()
    message(STATUS "Downloading and installing boost.")
    # For some external project macros
    include(ExternalProject)

    # Download boost from git and build the headers
    set( boost_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/boost )
    set( boost_target boost)

    ExternalProject_Add(
        ${boost_target}
        PREFIX ${boost_DIR}
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-1.76.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        GIT_SUBMODULES tools/build tools/boost_install libs/config libs/numeric
            libs/math libs/type_traits libs/predef libs/assert libs/static_assert
            libs/throw_exception libs/core libs/serialization libs/preprocessor libs/mpl
            libs/utility libs/typeof libs/array libs/units libs/integer libs/fusion
            libs/range libs/iterator libs/concept_check libs/detail libs/function_types
            libs/lexical_cast libs/container libs/move libs/smart_ptr libs/multi_array
            libs/functional libs/function libs/type_index libs/container_hash libs/bind
        CONFIGURE_COMMAND ./bootstrap.sh --prefix=<PREFIX>
        BUILD_COMMAND ./b2 headers --prefix=${boost_DIR}
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
    )

    set(Boost_INCLUDE_DIRS ${boost_DIR}/src/${boost_target})
    message(STATUS "Boost include dirs are ${Boost_INCLUDE_DIRS}")
endif()

# Add GSL
find_package(GSL REQUIRED)
  include_directories(${GSL_INCLUDE_DIRS})

# Compile git submodules
if(INSTALL_VMEC)
    message(STATUS "Install VMEC executable")
    set(VMEC_PYTHON_DIR ".")
    add_subdirectory(external/vmec)

    # message(STATUS "Install VMEC python module")
    # file(COPY ${PROJECT_SOURCE_DIR}/external/vmec/python/vmec DESTINATION ${CMAKE_BINARY_DIR}/external/vmec/python)
    # file(COPY_FILE ${PROJECT_SOURCE_DIR}/cmake_config_file.json ${CMAKE_BINARY_DIR}/external/vmec/cmake_config_file.json)
    # file(COPY_FILE ${PROJECT_SOURCE_DIR}/external/vmec/setup.py ${CMAKE_BINARY_DIR}/external/vmec/setup.py)
    # file(COPY_FILE ${PROJECT_SOURCE_DIR}/external/vmec/Makefile ${CMAKE_BINARY_DIR}/external/vmec/Makefile)
    # file(COPY_FILE ${PROJECT_SOURCE_DIR}/external/vmec/CMakeLists.txt ${CMAKE_BINARY_DIR}/external/vmec/CMakeLists.txt)
    # set(SETUP_PY_IN "${CMAKE_BINARY_DIR}/external/vmec/setup.py")
    # set(SETUP_PY    "${CMAKE_BINARY_DIR}/external/vmec/setup.py")
    # set(DEPS        "${CMAKE_BINARY_DIR}/external/vmec/python/vmec/__init__.py")
    # set(OUTPUT      "vmec")

    # configure_file(${SETUP_PY_IN} ${SETUP_PY})

    # add_custom_command(OUTPUT ${OUTPUT}
    #                    COMMAND ${PYTHON}
    #                    ARGS ${SETUP_PY} build
    #                    DEPENDS ${DEPS}
    #                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external/vmec)

    # add_custom_target(target ALL DEPENDS ${OUTPUT})

    install(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install --user WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external/vmec)")

endif()

if(INSTALL_SIMPLE)
    add_subdirectory(external/simple)
endif()

add_subdirectory(external/gyronimo)

# PYBIND11 specific configurations
if(SKBUILD)
  # Scikit-Build does not add your site-packages to the search path
  # automatically, so we need to add it _or_ the pybind11 specific directory
  # here.
  execute_process(
    COMMAND "${PYTHON}" -c
            "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE _tmp_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT)
  list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")
endif()
  
find_package(pybind11 CONFIG REQUIRED)

# Build NEAT
pybind11_add_module(${PROJECT_NAME}
    src/neatpp/fields_NEAT/equilibrium_stellna_qs.cc
    src/neatpp/metrics_NEAT/metric_stellna_qs.cc
    src/neatpp/neatpp.cpp
    # src/neatpp/NEAT_Mercier.cc
)
target_link_libraries(${PROJECT_NAME} PUBLIC gyronimo)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/neatpp/fields_NEAT)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/neatpp/metrics_NEAT)
target_link_libraries(${PROJECT_NAME} PUBLIC ${GSL_LIBRARIES})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/src/neat)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# Install neatpp
install(TARGETS ${PROJECT_NAME} DESTINATION .)
