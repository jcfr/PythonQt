
#include <QApplication>
#include <QLibrary>
#include <QtGlobal>

#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
  QApplication qapp(argc, argv);

#ifdef Q_OS_WIN32
  std::cout << "PATH [" << qPrintable(qgetenv("PATH")) << "]" << std::endl;
#else
  std::cout << "LD_LIBRARY_PATH [" << qPrintable(qgetenv("LD_LIBRARY_PATH")) << "]" << std::endl;
#endif


  QLibrary library("PythonQtDynamicLoaderSharedLibrary");
  if (!library.load())
    {
    std::cerr << "Failed to load 'PythonQtDynamicLoaderSharedLibrary': "
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
  return EXIT_SUCCESS;
}

