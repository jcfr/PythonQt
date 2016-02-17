

#include "PythonQt.h"
#include "PythonQtTests.h"

#include <QtGlobal>

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

  int _run_pythonqt_tests()
  {
    bool all = qgetenv("SUITE").isNull();
    bool api_suite = !all && strcmp(qgetenv("SUITE"), "Api") == 0;
    bool slotCalling_suite = !all && strcmp(qgetenv("SUITE"), "SlotCalling") == 0;
    bool signalHandler_suite = !all && strcmp(qgetenv("SUITE"), "SignalHandler") == 0;

    int argc = 0;
    char* argv[2];
    if (all)
      {
      argv[0] = new char[strlen("ALL") + 1];
      strcpy(argv[0], "ALL");
      ++argc;
      }
    else
      {
      argv[0] = new char[qgetenv("SUITE").size() + 1];
      strcpy(argv[0], qgetenv("SUITE"));
      ++argc;
      Q_ASSERT(!qgetenv("CASE").isNull());
      argv[1] = new char[qgetenv("CASE").size() + 1];
      strcpy(argv[1], qgetenv("CASE"));
      ++argc;
      }

    std::cout << "  running test suite '" << argv[0] << "'";
    if (!all) { std::cout << " - case '" << argv[1] << "'"; }
    std::cout << std::endl;

    // Copied from PythonQtTestMain.cpp
    int failCount = 0;
    if (all || api_suite)
      {
      PythonQtTestApi api;
      failCount += QTest::qExec(&api, argc, argv);
      }
    if (all || signalHandler_suite)
      {
      PythonQtTestSignalHandler signalHandler;
      failCount += QTest::qExec(&signalHandler, argc, argv);
      }
    if (all || slotCalling_suite)
      {
      PythonQtTestSlotCalling slotCalling;
      failCount += QTest::qExec(&slotCalling, argc, argv);
      }

    PythonQt::cleanup();

    if (Py_IsInitialized()) {
      Py_Finalize();
    }

    for(int argpos = 0; argpos < argc; ++argpos) { delete[] argv[argpos]; }

    return failCount;
  }


  int MY_EXPORT run_pythonqt_tests()
  {
    std::cout << "run_pythonqt_tests:" << std::endl;
#ifndef RUN_TESTSUITE_IN_STATIC_INITIALIZER
    return _run_pythonqt_tests();
#else
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
    _run_pythonqt_tests();
#else
    std::cout << "  no test suite" << std::endl;
#endif
  }
};


StaticInitializer staticInitializer;

