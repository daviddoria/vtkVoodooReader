#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkAxesActor.h>
#include <vtkObjectFactory.h>

#include <vtksys/ios/sstream>

#include "vtkVoodooReader.h"
#include "vtkCalibratedImageCollection.h"
#include "vtkImageCamera.h"

class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
{
  public:
    static KeyPressInteractorStyle* New();
    vtkTypeMacro(KeyPressInteractorStyle,vtkInteractorStyleTrackballCamera);

    KeyPressInteractorStyle()
    {
      this->CurrentCameraId = 0;
    }

    virtual void OnKeyPress()
    {
      //get the keypress
      vtkRenderWindowInteractor *rwi = this->Interactor;
      std::string key = rwi->GetKeySym();

      if(key.compare("Left") == 0 && this->CurrentCameraId > 0)
        {
        cout << "The left arrow was pressed." << endl;
        this->ImageCollection->GetCamera(CurrentCameraId)->GetAxesActor()->VisibilityOff();
        this->ImageCollection->GetCamera(CurrentCameraId)->GetImageActor()->VisibilityOff();
        CurrentCameraId--;
        this->ImageCollection->GetCamera(CurrentCameraId)->GetAxesActor()->VisibilityOn();
        this->ImageCollection->GetCamera(CurrentCameraId)->GetImageActor()->VisibilityOn();
        }

      if(key.compare("Right") == 0 && this->CurrentCameraId < this->ImageCollection->GetNumberOfCameras() - 1)
        {
        cout << "The right arrow was pressed." << endl;
        this->ImageCollection->GetCamera(CurrentCameraId)->GetAxesActor()->VisibilityOff();
        this->ImageCollection->GetCamera(CurrentCameraId)->GetImageActor()->VisibilityOff();
        CurrentCameraId++;
        this->ImageCollection->GetCamera(CurrentCameraId)->GetAxesActor()->VisibilityOn();
        this->ImageCollection->GetCamera(CurrentCameraId)->GetImageActor()->VisibilityOn();
        }

      if(key.compare("Up") == 0)
        {
        //this->ImageCollection->GetCamera(CurrentCameraId)->SetDistanceFromCamera();
        }
      std::cout << "Displaying camera: " << this->CurrentCameraId << std::endl;
      this->RenderWindow->Render();

      // Forward events
      vtkInteractorStyleTrackballCamera::OnKeyPress();
    }

    void SetRenderWindow(vtkSmartPointer<vtkRenderWindow> renderWindow) {this->RenderWindow = renderWindow;}
    void SetImageCollection(vtkSmartPointer<vtkCalibratedImageCollection> imageCollection) {this->ImageCollection = imageCollection;}
  private:
    vtkSmartPointer<vtkRenderWindow> RenderWindow;
    unsigned int CurrentCameraId;
    vtkSmartPointer<vtkCalibratedImageCollection> ImageCollection;
};

vtkStandardNewMacro(KeyPressInteractorStyle);

int main (int argc, char *argv[])
{
  // Verify input arguments
  if(argc != 3)
    {
    std::cout << "Required arguments: InputFilename.voodoo ImageDirectory" << std::endl;
    return EXIT_FAILURE;
    }

  // Parse commandline arguments
  std::string inputFilename = argv[1];
  std::string inputImageDirectory = argv[2];

  //Create a renderer, render window, and interactor
  vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkSmartPointer<KeyPressInteractorStyle> style =
      vtkSmartPointer<KeyPressInteractorStyle>::New();
  renderWindowInteractor->SetInteractorStyle( style );
  style->SetRenderWindow(renderWindow);
  renderWindow->SetSize(800,800);
  //renderer->SetBackground(1,1,1); // Background color white

  // Create a voodoo reader
  vtkSmartPointer<vtkVoodooReader> reader =
    vtkSmartPointer<vtkVoodooReader>::New();
  reader->SetImageFileExtension(".png");
  reader->SetFileName(inputFilename.c_str());
  reader->SetImageDirectory(inputImageDirectory.c_str());
  reader->Update();

  // Add the cameras/images to the scene
  //vtkCalibratedImageCollection* imageCollection = reader->GetOutputImageCollection();
  vtkCalibratedImageCollection* imageCollection = reader->GetCameras();
  std::cout << "There are " << imageCollection->GetNumberOfCameras() << " cameras." << std::endl;
  style->SetImageCollection(imageCollection);

  for(unsigned int i = 0; i < imageCollection->GetNumberOfCameras(); i++)
    {
    vtkImageCamera* camera = imageCollection->GetCamera(i);
    //std::cout << "Camera " << i << endl << *camera << std::endl;
    camera->SetRenderer(renderer);
    camera->SetDistanceFromCamera(6);
    camera->UpdateCameraActor();
    camera->UpdateImageActor();
    camera->GetAxesActor()->VisibilityOff();
    camera->GetImageActor()->VisibilityOff();
    std::stringstream ssX;
    ssX << "X" << i;
    std::stringstream ssY;
    ssY << "Y" << i;
    std::stringstream ssZ;
    ssZ << "Z" << i;
    camera->GetAxesActor()->SetXAxisLabelText(ssX.str().c_str());
    camera->GetAxesActor()->SetYAxisLabelText(ssY.str().c_str());
    camera->GetAxesActor()->SetZAxisLabelText(ssZ.str().c_str());

    }

    imageCollection->GetCamera(0)->GetAxesActor()->VisibilityOn();
    imageCollection->GetCamera(0)->GetImageActor()->VisibilityOn();

  // Add the points to the scene
  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(reader->GetPoints()->GetProducerPort());
  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  renderer->AddActor(actor);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
