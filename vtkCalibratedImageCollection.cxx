#include "vtkCalibratedImageCollection.h"
#include "vtkImageCamera.h"

#include "vtkObjectFactory.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include "vtkSmartPointer.h"
#include "vtkDirectory.h"

#include <vtksys/SystemTools.hxx>
#include <vtkstd/algorithm>

vtkStandardNewMacro(vtkCalibratedImageCollection);

vtkCalibratedImageCollection::vtkCalibratedImageCollection()
{

}

vtkCalibratedImageCollection::~vtkCalibratedImageCollection()
{
}

void vtkCalibratedImageCollection::DeepCopy(vtkCalibratedImageCollection* imageCollection)
{
  this->Cameras = imageCollection->GetCameras();
}

//----------------------------------------------------------------------------
void vtkCalibratedImageCollection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "NumberOfImageCameras: " << this->Cameras.size() << "\n";

  for(unsigned int i = 0; i < this->Cameras.size(); i++)
    {
    vtkstd::cout << "Camera " << i << ":" << *(this->Cameras[i]) << vtkstd::endl;
    }

}

void vtkCalibratedImageCollection::AddCamera(vtkImageCamera* c)
{

  vtkSmartPointer<vtkImageCamera> NewCamera = vtkSmartPointer<vtkImageCamera>::New();
  NewCamera->CopyImageCamera(c);

  this->Cameras.push_back(NewCamera);

}

vtkImageCamera* vtkCalibratedImageCollection::GetCamera(const unsigned int i)
{
  return this->Cameras[i];
}

vtkstd::vector<vtkSmartPointer<vtkImageCamera> > vtkCalibratedImageCollection::GetCameras()
{
  return this->Cameras;
}

unsigned int vtkCalibratedImageCollection::GetNumberOfCameras()
{
  return this->Cameras.size();
}