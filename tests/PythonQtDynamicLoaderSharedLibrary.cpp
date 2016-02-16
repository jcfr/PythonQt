

#include "PythonQt.h"

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

}

struct StaticInitializer
{
  StaticInitializer()
  {
    std::cout << "PythonQtDynamicLoaderSharedLibrary::StaticInitializer" << std::endl;
    PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);
  }
};


StaticInitializer staticInitializer;

