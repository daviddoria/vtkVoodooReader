#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataWriter.h>

#include "vtkVoodooReader.h"
#include "vtkCalibratedImageCollection.h"
#include "vtkImageCamera.h"

int main (int argc, char *argv[])
{
  if(argc != 3)
    {
    cout << "Required arguments: InputFilename(text) OutputFilename(vtp)" << endl;
    return EXIT_FAILURE;
    }

  vtkstd::string inputFilename = argv[1];
  vtkstd::string outputFilename = argv[2];

  vtkSmartPointer<vtkVoodooReader> reader =
      vtkSmartPointer<vtkVoodooReader>::New();
  reader->SetFileName(inputFilename.c_str());
  reader->SetImageFileExtension(".png");
  reader->Update();

  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
      vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(outputFilename.c_str());
  writer->SetInputConnection(reader->GetOutputPort(1));
  //writer->SetInput(reader->GetOutputPoints()); //this also works
  writer->Write();

  return EXIT_SUCCESS;
}
