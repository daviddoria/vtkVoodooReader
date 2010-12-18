#include <vtkSmartPointer.h>

#include "vtkVoodooReader.h"

int main (int argc, char *argv[])
{
  vtkSmartPointer<vtkVoodooReader> reader = 
      vtkSmartPointer<vtkVoodooReader>::New();
  
  return EXIT_SUCCESS;
}
