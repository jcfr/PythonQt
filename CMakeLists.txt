#-----------------------------------------------------------------------------
# NOTE: The CMake files have been contributed to PythonQt and have not been tested with the current
# PythonQt version. They have not yet been updated to support Qt 5 and/or Python 3.
#
# If you are not a CMake expert, you should better use the provided qmake profiles.
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8)

#-----------------------------------------------------------------------------
project(PythonQt)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Python libraries

find_package(PythonLibs REQUIRED)
include_directories("${PYTHON_INCLUDE_DIR}")
add_definitions(
  -DPYTHONQT_USE_RELEASE_PYTHON_FALLBACK
  -DPYTHONQT_SUPPORT_NAME_PROPERTY
  )

#-----------------------------------------------------------------------------
# Build options

if(NOT DEFINED PythonQt_INSTALL_RUNTIME_DIR)
  set(PythonQt_INSTALL_RUNTIME_DIR bin)
endif()

if(NOT DEFINED PythonQt_INSTALL_LIBRARY_DIR)
  set(PythonQt_INSTALL_LIBRARY_DIR lib${LIB_SUFFIX})
endif()

if(NOT DEFINED PythonQt_INSTALL_ARCHIVE_DIR)
  set(PythonQt_INSTALL_ARCHIVE_DIR lib${LIB_SUFFIX})
endif()

if(NOT DEFINED PythonQt_INSTALL_INCLUDE_DIR)
  set(PythonQt_INSTALL_INCLUDE_DIR include/PythonQt)
endif()

option(PythonQt_Wrap_QtAll "Make all Qt components available in python" OFF)

set(qtlibs core gui network opengl sql svg uitools webkit xml xmlpatterns)
foreach(qtlib ${qtlibs})
  OPTION(PythonQt_Wrap_Qt${qtlib} "Make all of Qt${qtlib} available in python" OFF)
endforeach()

# Force option if it applies
if(PythonQt_Wrap_QtAll)
  list(REMOVE_ITEM qtlibs xmlpatterns) # xmlpatterns wrapper does *NOT* build at all :(
  foreach(qtlib ${qtlibs})
    if(NOT ${PythonQt_Wrap_Qt${qtlib}})
      set(PythonQt_Wrap_Qt${qtlib} ON CACHE BOOL "Make all of Qt${qtlib} available in python" FORCE)
      message(STATUS "Enabling [PythonQt_Wrap_Qt${qtlib}] because of [PythonQt_Wrap_QtAll] evaluates to True")
    endif()
  endforeach()
endif()

option(PythonQt_DEBUG "Enable/Disable PythonQt debug output" OFF)
if(PythonQt_DEBUG)
  add_definitions(-DPYTHONQT_DEBUG)
else()
  remove_definitions(-DPYTHONQT_DEBUG)
endif()

#-----------------------------------------------------------------------------
# Setup Qt

set(minimum_required_qt_version "4.6.2")

find_package(Qt4)

if(QT4_FOUND)

  set(found_qt_version ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH})

  if(${found_qt_version} VERSION_LESS ${minimum_required_qt_version})
      message(FATAL_ERROR "error: PythonQt requires Qt >= ${minimum_required_qt_version} -- you cannot use Qt ${found_qt_version}.")
  endif()

  # Enable required qt module
  foreach(qtlib network opengl sql svg uitools webkit xml xmlpatterns)
    string(TOUPPER ${qtlib} qtlib_uppercase)
    if (NOT ${QT_QT${qtlib_uppercase}_FOUND})
      message(FATAL_ERROR "QT_QT${qtlib_uppercase} *not* FOUND - Try to disable PythonQt_Wrap_Qt${qtlib}")
    endif()
    set(QT_USE_QT${qtlib_uppercase} ${PythonQt_Wrap_Qt${qtlib}})
  endforeach()

  set(QT_USE_QTTEST ${BUILD_TESTING})

  include(${QT_USE_FILE})
else()
  message(FATAL_ERROR "error: Qt4 was not found on your system. You probably need to set the QT_QMAKE_EXECUTABLE variable")
endif()

#-----------------------------------------------------------------------------
# The variable "generated_cpp_suffix" allows to conditionnally compile the generated wrappers
# associated with the Qt version being used.
set(generated_cpp_suffix "_${QT_VERSION_MAJOR}${QT_VERSION_MINOR}")
if("${generated_cpp_suffix}" STREQUAL "_46")
  set(generated_cpp_suffix "_47") # Also use 4.7 wrappers for 4.6.x version
endif()

#-----------------------------------------------------------------------------
# Sources

set(sources
    src/PythonQtBoolResult.cpp
    src/PythonQtClassInfo.cpp
    src/PythonQtClassWrapper.cpp
    src/PythonQtConversion.cpp
    src/PythonQt.cpp
    src/PythonQtImporter.cpp
    src/PythonQtInstanceWrapper.cpp
    src/PythonQtMethodInfo.cpp
    src/PythonQtMisc.cpp
    src/PythonQtObjectPtr.cpp
    src/PythonQtQFileImporter.cpp
    src/PythonQtSignalReceiver.cpp
    src/PythonQtSlot.cpp
    src/PythonQtSignal.cpp
    src/PythonQtStdDecorators.cpp
    src/PythonQtStdIn.cpp
    src/PythonQtStdOut.cpp
    src/gui/PythonQtScriptingConsole.cpp

    generated_cpp${generated_cpp_suffix}/PythonQt_QtBindings.cpp

    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin0.cpp
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin_init.cpp
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin0.cpp
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin_init.cpp
)

#-----------------------------------------------------------------------------
# List headers.  This is list is used for the install command.

set(headers
    src/PythonQtBoolResult.h
    src/PythonQtClassInfo.h
    src/PythonQtClassWrapper.h
    src/PythonQtConversion.h
    src/PythonQtCppWrapperFactory.h
    src/PythonQtDoc.h
    src/PythonQt.h
    src/PythonQtImporter.h
    src/PythonQtImportFileInterface.h
    src/PythonQtInstanceWrapper.h
    src/PythonQtMethodInfo.h
    src/PythonQtMisc.h
    src/PythonQtObjectPtr.h
    src/PythonQtQFileImporter.h
    src/PythonQtSignalReceiver.h
    src/PythonQtSlot.h
    src/PythonQtSignal.h
    src/PythonQtStdDecorators.h
    src/PythonQtStdIn.h
    src/PythonQtStdOut.h
    src/PythonQtSystem.h
    src/PythonQtUtils.h
    src/PythonQtVariants.h
    src/PythonQtPythonInclude.h
    generated_cpp${generated_cpp_suffix}/PythonQt_QtBindings.h
)

#-----------------------------------------------------------------------------
# Headers that should run through moc

set(moc_sources
    src/PythonQt.h
    src/PythonQtSignalReceiver.h
    src/PythonQtStdDecorators.h
    src/gui/PythonQtScriptingConsole.h

    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin0.h
    generated_cpp${generated_cpp_suffix}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin0.h
)

#-----------------------------------------------------------------------------
# Add extra sources
foreach(qtlib core gui network opengl sql svg uitools webkit xml xmlpatterns)

  if (${PythonQt_Wrap_Qt${qtlib}})

    ADD_DEFINITIONS(-DPYTHONQT_WRAP_Qt${qtlib})

    set(file_prefix generated_cpp${generated_cpp_suffix}/com_trolltech_qt_${qtlib}/com_trolltech_qt_${qtlib})

    foreach(index RANGE 0 11)

      # Source files
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${file_prefix}${index}.cpp)
        list(APPEND sources ${file_prefix}${index}.cpp)
      endif()

      # Headers that should run through moc
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${file_prefix}${index}.h)
        list(APPEND moc_sources ${file_prefix}${index}.h)
      endif()

    endforeach()

    list(APPEND sources ${file_prefix}_init.cpp)

  endif()
endforeach()

#-----------------------------------------------------------------------------
# UI files
set(ui_sources )

#-----------------------------------------------------------------------------
# Resources
set(qrc_sources )

#-----------------------------------------------------------------------------
# Do wrapping
qt4_wrap_cpp(gen_moc_sources ${moc_sources})
qt4_wrap_ui(gen_ui_sources ${ui_sources})
qt4_add_resources(gen_qrc_sources ${qrc_sources})

#-----------------------------------------------------------------------------
# Build the library

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

add_library(PythonQt SHARED
            ${sources}
            ${gen_moc_sources}
            ${gen_ui_sources}
            ${gen_qrc_sources}
  )
set_target_properties(PythonQt PROPERTIES DEFINE_SYMBOL PYTHONQT_EXPORTS)

#
# That should solve linkage error on Mac when the project is used in a superbuild setup
# See http://blog.onesadcookie.com/2008/01/installname-magic.html
#
set_target_properties(PythonQt  PROPERTIES
  INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
  )

target_link_libraries(PythonQt
              ${PYTHON_LIBRARY}
              ${QT_LIBRARIES}
  )

#-----------------------------------------------------------------------------
# Install library (on windows, put the dll in 'bin' and the archive in 'lib')

install(TARGETS PythonQt
        RUNTIME DESTINATION ${PythonQt_INSTALL_RUNTIME_DIR}
        LIBRARY DESTINATION ${PythonQt_INSTALL_LIBRARY_DIR}
        ARCHIVE DESTINATION ${PythonQt_INSTALL_ARCHIVE_DIR})
install(FILES ${headers} DESTINATION ${PythonQt_INSTALL_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Testing

option(BUILD_TESTING "Build the testing tree." OFF)
include(CTest)

if(BUILD_TESTING)
  create_test_sourcelist(test_sources PythonQtCppTests.cpp
    tests/PythonQtTestMain.cpp
    )

  set_property(SOURCE tests/PythonQtTestMain.cpp PROPERTY COMPILE_DEFINITIONS "main=tests_PythonQtTestMain")

  list(APPEND PythonQtTests_sources
    tests/PythonQtTests.cpp
    tests/PythonQtTests.h
    )

  QT4_WRAP_CPP(_tmp_moc_sources
    tests/PythonQtTests.h
    )
  list(APPEND PythonQtTests_sources ${_tmp_moc_sources})
  list(APPEND moc_sources ${_tmp_moc_sources})

  if(PythonQt_Wrap_Qtcore)
    include_directories(generated_cpp${generated_cpp_suffix})

    list(APPEND test_sources
      tests/PythonQtTestCleanup.cpp
      tests/PythonQtTestCleanup.h
      )
    QT4_WRAP_CPP(_tmp_moc_sources
      tests/PythonQtTestCleanup.h
      )
    list(APPEND test_sources ${_tmp_moc_sources})
    list(APPEND moc_sources ${_tmp_moc_sources})

    set_property(SOURCE tests/PythonQtTestMain.cpp APPEND PROPERTY COMPILE_DEFINITIONS "PythonQt_Wrap_Qtcore")
  endif()

  add_custom_target(PythonQtTestsWrapCpp DEPENDS ${moc_sources})

  set(PythonQtTestApi_cases
    testCall
    testVariables
    testRedirect
    testImporter
    testQColorDecorators
    testQtNamespace
    testConnects
    testProperties
    testDynamicProperties
    )

  set(PythonQtTestSlotCalling_cases
    testNoArgSlotCall
    testPODSlotCalls
    testCPPSlotCalls
    testQVariantSlotCalls
    testQListSlotCalls
    testQMapSlotCalls
    testObjectSlotCalls
    testMultiArgsSlotCall
    testPyObjectSlotCall
    testOverloadedCall
    #testCppFactory # XXX Fix this. Running this test standalone is failing.
    testInheritance
    testAutoConversion
    testProperties
    )

  set(PythonQtTestSignalHandler_cases
    testSignalHandler
    testRecursiveSignalHandler
    )

  foreach(suite Api SlotCalling SignalHandler)
    set(_compile_definition "${suite}_TEST_SUITE")
    set(_suffix "_${suite}")

    add_executable(PythonQtCppTests${_suffix} ${test_sources} ${PythonQtTests_sources})
    target_link_libraries(PythonQtCppTests${_suffix} PythonQt)
    set_target_properties(PythonQtCppTests${_suffix} PROPERTIES COMPILE_DEFINITIONS ${_compile_definition})
    add_dependencies(PythonQtCppTests${_suffix} PythonQtTestsWrapCpp)

    # Add one test for each test suite case
    foreach(case ${PythonQtTest${suite}_cases})
      add_test(
        NAME tests_PythonQtTestMain_${suite}_${case}
        COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:PythonQtCppTests_${suite}> tests/PythonQtTestMain ${case}
        )
    endforeach()
  endforeach()

  # suite: ALL
  add_executable(PythonQtCppTests ${test_sources} ${PythonQtTests_sources})
  target_link_libraries(PythonQtCppTests PythonQt)
  add_dependencies(PythonQtCppTests PythonQtTestsWrapCpp)

  add_test(
    NAME tests_PythonQtTestMain
    COMMAND ${Slicer_LAUNCH_COMMAND} $<TARGET_FILE:PythonQtCppTests> tests/PythonQtTestMain
    )

  #
  # PythonQtDynamicLoading tests
  #
  # These tests check that a dynamically loaded library (itself linked against PythonQt) properly
  # initializes PythonQt and allow the PythonQt testing suites to complete.
  #
  # Infrastructure:
  #
  #  * PythonQtDynamicLoaderSharedLibrary:
  #    - Shared library linked against PythonQt
  #    - export two "C compatible" symbols:
  #         'this_function_returns_42': function that is expected to return the integer 42
  #         'run_pythonqt_tests' : function running python test suite if RUN_TESTSUITE_IN_STATIC_INITIALIZER is 0
  #    - contain a statically initialized structure named 'StaticInitializer':
  #         - always call 'PythonQt::init()'
  #         - running python test suite if RUN_TESTSUITE_IN_STATIC_INITIALIZER is 1
  #
  #
  #  * PythonQtDynamicLoader: Executable loading the 'PythonQtDynamicLoaderSharedLibrary' using QLibrary
  #    - resolve symbols 'this_function_returns_42' and 'run_pythonqt_tests' and call the corresponding function.
  #
  #
  # To summarize, two cases are considered:
  #
  # (a) RUN_TESTSUITE_IN_STATIC_INITIALIZER = 0
  #
  #  - PythonQt::init(...) called in StaticInitializer
  #  - Python test suite executed in function 'run_pythonqt_tests'
  #
  #  Name of test: tests_PythonQtDynamicLoading_RUN_TESTSUITE_IN_STATIC_INITIALIZER_0
  #
  #
  # (b) RUN_TESTSUITE_IN_STATIC_INITIALIZER = 1
  #
  #  - PythonQt::init(...) called in 'StaticInitializer'
  #  - Python test suite executed in 'StaticInitializer'
  #
  #  Name of test: tests_PythonQtDynamicLoading_RUN_TESTSUITE_IN_STATIC_INITIALIZER_1

  # Configure 'test_wrapper' common to all 'PythonQtDynamicLoading' tests
  #
  # Since setting the ENVIRONMENT property value with semicolon (e.g. PATH) is not supported, we
  # generate a wrapper script to update PATH and LD_LIBRARY_PATH.
  set(test_wrapper "${CMAKE_CURRENT_BINARY_DIR}/test_wrapper.cmake")
  file(WRITE ${test_wrapper} "
if(UNIX)
  set(ENV{LD_LIBRARY_PATH} \"\$ENV{LD_LIBRARY_PATH}:.\")
else()
  set(ENV{PATH} \"\$ENV{PATH};.\")
endif()
set(ENV{SUITE} \"\${SUITE}\")
set(ENV{CASE} \"\${CASE}\")
execute_process(COMMAND \${TARGET_FILE} RESULT_VARIABLE result)
if(NOT result EQUAL 0)
  message(FATAL_ERROR \"test failed ! [result: \${result}]\")
endif()
")

  function(_add_dynamic_loading_test run_testsuite_in_static_initializer)

    macro(_add_targets extra_compile_definitions)

      add_library(PythonQtDynamicLoaderSharedLibrary_${suffix}${suite_suffix} SHARED
        tests/PythonQtDynamicLoaderSharedLibrary.cpp
        ${PythonQtTests_sources}
        )
      target_link_libraries(PythonQtDynamicLoaderSharedLibrary_${suffix}${suite_suffix} PythonQt)
      add_dependencies(PythonQtDynamicLoaderSharedLibrary_${suffix}${suite_suffix} PythonQtTestsWrapCpp)

      add_executable(PythonQtDynamicLoader_${suffix}${suite_suffix} tests/PythonQtDynamicLoader.cpp)
      target_link_libraries(PythonQtDynamicLoader_${suffix}${suite_suffix} ${QT_LIBRARIES})

      if(run_testsuite_in_static_initializer)
        set_property(TARGET
          PythonQtDynamicLoaderSharedLibrary_${suffix}${suite_suffix}
          PythonQtDynamicLoader_${suffix}${suite_suffix}
          PROPERTY COMPILE_DEFINITIONS "RUN_TESTSUITE_IN_STATIC_INITIALIZER" ${extra_compile_definitions}
          )
      endif()
    endmacro()

    set(suffix 0)
    if(run_testsuite_in_static_initializer)
      set(suffix 1)
    endif()

    foreach(suite Api SlotCalling SignalHandler)
      set(_compile_definition "${suite}_TEST_SUITE")
      set(suite_suffix "_${suite}")

      _add_targets("${_compile_definition}")

      # Add one test for each test suite case
      foreach(case ${PythonQtTest${suite}_cases})
        set(_testname tests_PythonQtDynamicLoading_RUN_TESTSUITE_IN_STATIC_INITIALIZER_${suffix}_${suite}_${case})
        add_test(
          NAME ${_testname}
          COMMAND ${CMAKE_COMMAND}
            -DTARGET_FILE=$<TARGET_FILE:PythonQtDynamicLoader_${suffix}${suite_suffix}>
            -DSUITE=${suite} -DCASE=${case} -P ${test_wrapper}
          )
        set_tests_properties(${_testname}
          PROPERTIES FAIL_REGULAR_EXPRESSION "Problem running _run_pythonqt_tests")
      endforeach()
    endforeach()

    # suite: ALL
    set(suite_suffix "")
    _add_targets("")

    set(_testname tests_PythonQtDynamicLoading_RUN_TESTSUITE_IN_STATIC_INITIALIZER_${suffix})
    add_test(
      NAME ${_testname}
      COMMAND ${CMAKE_COMMAND} -DTARGET_FILE=$<TARGET_FILE:PythonQtDynamicLoader_${suffix}> -P ${test_wrapper}
      )
    set_tests_properties(${_testname}
      PROPERTIES FAIL_REGULAR_EXPRESSION "Problem running _run_pythonqt_tests")

  endfunction()

  #  RUN_TESTSUITE_IN_STATIC_INITIALIZER = 0
  _add_dynamic_loading_test(0)

  #  RUN_TESTSUITE_IN_STATIC_INITIALIZER = 1
  _add_dynamic_loading_test(1)

endif()

