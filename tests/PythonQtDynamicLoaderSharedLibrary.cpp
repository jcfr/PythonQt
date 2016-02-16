

#include "PythonQt.h"
#include "PythonQtTests.h"

#ifdef Q_OS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

#define RUN_TEST_IN_STATIC_INITIALIZER

extern "C"
{

  int MY_EXPORT this_function_returns_42()
  {
    return 42;
  }

  int _run_pythonqt_tests(int argc, char* argv[])
  {
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
#ifndef RUN_TEST_IN_STATIC_INITIALIZER
    return _run_pythonqt_tests(argc, argv);
#else
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    return 0;
#endif
  }

}

struct StaticInitializer
{
  StaticInitializer()
  {
    std::cout << "PythonQtDynamicLoaderSharedLibrary::StaticInitializer" << std::endl;
    PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);

#ifdef RUN_TEST_IN_STATIC_INITIALIZER
    int argc = 1;
    char * argv [] = {"RunTestInStaticInitializer"};
    _run_pythonqt_tests(argc, argv);
#endif
  }
};


StaticInitializer staticInitializer;

