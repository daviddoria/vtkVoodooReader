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
    std::cout << "Required arguments: InputFilename(text) OutputFilename(vtp)" << std::endl;
    return EXIT_FAILURE;
    }

  std::string inputFilename = argv[1];
  std::string outputFilename = argv[2];

  vtkSmartPointer<vtkVoodooReader> reader =
    vtkSmartPointer<vtkVoodooReader>::New();
  reader->SetImageFileExtension(".png");
  reader->SetFileName(inputFilename.c_str());
  reader->Update();

  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(outputFilename.c_str());
  writer->SetInputConnection(reader->GetOutputPort(1));
  //writer->SetInput(reader->GetOutputPoints()); //this also works
  writer->Write();

  vtkCalibratedImageCollection* imageCollection = reader->GetOutputImageCollection();
  std::cout << "There are " << imageCollection->GetNumberOfCameras() << " cameras." << std::endl;

  for(unsigned int i = 0; i < imageCollection->GetNumberOfCameras(); i++)
    {
    //std::cout << "Camera " << i << endl << *(imageCollection->GetCamera(i)) << std::endl;

    }
  return EXIT_SUCCESS;
}
