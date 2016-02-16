
#include <QApplication>
#include <QLibrary>
#include <QtGlobal>

#include <cstdlib>
#include <iostream>

#ifndef RUN_TESTSUITE_IN_STATIC_INITIALIZER
# define DYNAMIC_LOADER_SHARED_LIBRARY_NAME "PythonQtDynamicLoaderSharedLibrary_0"
#else
# define DYNAMIC_LOADER_SHARED_LIBRARY_NAME "PythonQtDynamicLoaderSharedLibrary_1"
#endif

int main(int argc, char* argv[])
{
  QApplication qapp(argc, argv);

#ifdef Q_OS_WIN32
  std::cout << "PATH [" << qPrintable(qgetenv("PATH")) << "]" << std::endl;
#else
  std::cout << "LD_LIBRARY_PATH [" << qPrintable(qgetenv("LD_LIBRARY_PATH")) << "]" << std::endl;
#endif


  QLibrary library(DYNAMIC_LOADER_SHARED_LIBRARY_NAME);
  std::cout << "Loading '" << DYNAMIC_LOADER_SHARED_LIBRARY_NAME << "'" << std::endl;
  if (!library.load())
    {
    std::cerr << "Failed to load '" << DYNAMIC_LOADER_SHARED_LIBRARY_NAME << "': "
      << qPrintable(library.errorString()) << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Resolve and invoke 'this_function_returns_42' function.
  //
  typedef int (*FUNC_RETURNS_INT_TYPE)();
  FUNC_RETURNS_INT_TYPE func = (FUNC_RETURNS_INT_TYPE) library.resolve("this_function_returns_42");
  if (!func)
    {
    std::cerr << "Failed to resolve symbol 'this_function_returns_42'" << std::endl;
    return EXIT_FAILURE;
    }
  int result = func();
  int expected = 42;
  if (result != expected)
    {
    std::cerr << "Problem with function 'this_function_returns_42':\n"
              << "\tresult: " << result << "\n"
              << "\texpected: " << expected
              << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Resolve and invoke 'run_pythonqt_tests' function.
  //
  typedef int (*FUNC_ARGC_ARGV_RETURNS_INT_TYPE)(int argc, char* argv[]);
  FUNC_ARGC_ARGV_RETURNS_INT_TYPE func2 =
    (FUNC_ARGC_ARGV_RETURNS_INT_TYPE) library.resolve("run_pythonqt_tests");
  if (!func2)
    {
    std::cerr << "Failed to resolve symbol 'run_pythonqt_tests'" << std::endl;
    return EXIT_FAILURE;
    }

  result = func2(argc, argv);
  expected = 0;
  if (result != expected)
    {
    std::cerr << "Problem with function 'run_pythonqt_tests':\n"
              << "\tresult: " << result << "\n"
              << "\texpected: " << expected
              << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

