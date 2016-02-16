

#include "PythonQt.h"
#include "PythonQtTests.h"

#ifdef Q_OS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C"
{

  int MY_EXPORT this_function_returns_42()
  {
    return 42;
  }

  int _run_pythonqt_tests(int argc, char* argv[])
  {
    std::cout << "  running test suite" << std::endl;

    // Copied from PythonQtTestMain.cpp
    int failCount = 0;
    PythonQtTestApi api;
    failCount += QTest::qExec(&api, argc, argv);
    PythonQtTestSignalHandler signalHandler;
    failCount += QTest::qExec(&signalHandler, argc, argv);
    PythonQtTestSlotCalling slotCalling;
    failCount += QTest::qExec(&slotCalling, argc, argv);

    PythonQt::cleanup();

    if (Py_IsInitialized()) {
      Py_Finalize();
    }

    return failCount;
  }


  int MY_EXPORT run_pythonqt_tests(int argc, char* argv[])
  {
    std::cout << "run_pythonqt_tests:" << std::endl;
#ifndef RUN_TESTSUITE_IN_STATIC_INITIALIZER
    return _run_pythonqt_tests(argc, argv);
#else
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    std::cout << "  no test suite" << std::endl;
    return 0;
#endif
  }

}

struct StaticInitializer
{
  StaticInitializer()
  {
    std::cout << "StaticInitializer:" << std::endl;
    PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);

#ifdef RUN_TESTSUITE_IN_STATIC_INITIALIZER
    int argc = 1;
    char * argv [] = {"RunTestSuiteInStaticInitializer"};
    _run_pythonqt_tests(argc, argv);
#else
    std::cout << "  no test suite" << std::endl;
#endif
  }
};


StaticInitializer staticInitializer;

