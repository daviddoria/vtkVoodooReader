#include "vtkImageCamera.h"

#include "vtkObjectFactory.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include "vtkSmartPointer.h"
#include "vtkLabeledDataMapper.h"
#include "vtkCameraActor.h"
#include "vtkCommand.h"
#include "vtkActor2D.h"
#include "vtkSliderWidget.h"
#include "vtkSliderRepresentation.h"
#include "vtkSphereSource.h"
#include "vtkJPEGReader.h"
#include "vtkLine.h"
#include "vtkCellData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkAxesActor.h"
#include "vtkRenderer.h"
#include "vtkTexture.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolygon.h"
#include "vtkFloatArray.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkMatrix3x3.h"
#include "vtkPoints.h"
#include "vtkImageCanvasSource2D.h"

vtkStandardNewMacro(vtkImageCamera);

vtkImageCamera::vtkImageCamera()
{
  this->ImageActor = vtkSmartPointer<vtkActor>::New();

  this->DistanceFromCamera = 1.0;

  this->TextureImage = vtkSmartPointer<vtkImageData>::New();

}

void vtkImageCamera::SetTextureFileName(vtkstd::string filename)
{
  this->TextureFileName = filename;
}

void vtkImageCamera::SetRenderer(vtkRenderer* renderer)
{
  this->Renderer = renderer;
}
/*
void vtkImageCamera::CreateReprojectionRays(vtkPoints* inputPoints)
{
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  points->DeepCopy(inputPoints);
  points->InsertNextPoint(this->CameraCenter);

  //Create a cell array to store the lines in and add the lines to it
  vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();

  for(unsigned int i = 0; i < points->GetNumberOfPoints(); i++)
    {

    vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
    line->GetPointIds()->SetId(0,i);
    line->GetPointIds()->SetId(1,points->GetNumberOfPoints()-1); //connect to the camera center

    lines->InsertNextCell(line);
    }

  //Create a polydata to store everything in
  vtkSmartPointer<vtkPolyData> linesPolyData = vtkSmartPointer<vtkPolyData>::New();

  //add the points to the dataset
  linesPolyData->SetPoints(points);

  //add the lines to the dataset
  linesPolyData->SetLines(lines);

  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInput(linesPolyData);

  this->ReprojectionRaysActor->SetMapper(mapper);
}
*/

/*
void vtkImageCamera::CreateReprojectedImage(vtkPoints* points)
{
  int* extent = this->TextureImage->GetExtent();

  vtkSmartPointer<vtkImageCanvasSource2D> canvasSource = vtkSmartPointer<vtkImageCanvasSource2D>::New();
  canvasSource->SetExtent(extent);
  canvasSource->SetNumberOfScalarComponents(3);
  canvasSource->SetScalarTypeToUnsignedChar();
  canvasSource->SetDrawColor(255, 255, 255, 255);
  canvasSource->FillBox(extent[0], extent[1], extent[2], extent[3]);
  canvasSource->Update();

  ReprojectedImage->DeepCopy(canvasSource->GetOutput());

  unsigned int outOfImageCounter = 0;
  unsigned int inImageCounter = 0;
  for(unsigned int i = 0; i < points->GetNumberOfPoints(); i++)
    {
    double p[3];
    points->GetPoint(i, p);
    double pix[2];
    this->ProjectPoint(p, pix);

    //cout << "pix before shift: " << pix[0] << " " << pix[1] << endl;

    //cout << "w: " << this->GetWidth() << " h: " << this->GetHeight() << endl;

    //switch from (0,0) in the center of the image to (0,0) in the top left corner of the image
    pix[0] = round(pix[0] + this->GetWidth()/2.0);
    pix[1] = round(pix[1] + this->GetHeight()/2.0);

    //cout << "pix after shift: " << pix[0] << " " << pix[1] << endl;

    if(pix[0] < 0 || pix[1] < 0 || pix[0] > this->GetWidth() || pix[1] > this->GetHeight())
      {
      //cout << "pixel not in image" << endl;
      outOfImageCounter++;
      continue;
      }

    inImageCounter++;

    //red
    ReprojectedImage->SetScalarComponentFromDouble(pix[0],pix[1],0,0,255);
    ReprojectedImage->SetScalarComponentFromDouble(pix[0],pix[1],0,1,0);
    ReprojectedImage->SetScalarComponentFromDouble(pix[0],pix[1],0,2,0);

    }

    cout << "There were " << inImageCounter << " pixels in the image." << endl;
    cout << "There were " << outOfImageCounter << " pixels not in the image." << endl;

}
*/

void vtkImageCamera::SetDistanceFromCamera(const double d)
{
  this->DistanceFromCamera = d;
}

unsigned int vtkImageCamera::GetNumberOfRows()
{
  int dims[3];
  this->TextureImage->GetDimensions(dims);
  return dims[1];
}

unsigned int vtkImageCamera::GetHeight()
{
  int dims[3];
  this->TextureImage->GetDimensions(dims);
  return dims[1];
}

unsigned int vtkImageCamera::GetNumberOfColumns()
{
  int dims[3];
  this->TextureImage->GetDimensions(dims);
  return dims[0];
}

unsigned int vtkImageCamera::GetWidth()
{
  int dims[3];
  this->TextureImage->GetDimensions(dims);
  return dims[0];
}

int vtkImageCamera::RequestData(vtkInformation *vtkNotUsed(request),
    vtkInformationVector **vtkNotUsed(inputVector),
    vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  return 1;
}

void vtkImageCamera::SetTextureImage(vtkImageData* image)
{
  this->TextureImage->DeepCopy(image);
}

void vtkImageCamera::UpdateImageActor()
{

  //get the coordinates of the corners of the quad
  double TLpix[2];
  GetTopLeftPixel(TLpix);
  double TRpix[2];
  GetTopRightPixel(TRpix);
  double BLpix[2];
  GetBottomLeftPixel(BLpix);
  double BRpix[2];
  GetBottomRightPixel(BRpix);

  //cout << "TLpix: " << TLpix[0] << " " << TLpix[1] << endl;
  //cout << "TRpix: " << TRpix[0] << " " << TRpix[1] << endl;
  //cout << "BLpix: " << BLpix[0] << " " << BLpix[1] << endl;
  //cout << "BRpix: " << BRpix[0] << " " << BRpix[1] << endl;

  double TLray[3];
  GetRay(TLpix, TLray);
  double TRray[3];
  GetRay(TRpix, TRray);
  double BLray[3];
  GetRay(BLpix, BLray);
  double BRray[3];
  GetRay(BRpix, BRray);

  /*
  cout << "TLray: " << TLray[0] << " " << TLray[1] << " " << TLray[2] << endl;
  cout << "TRray: " << TRray[0] << " " << TRray[1] << " " << TRray[2] << endl;
  cout << "BLray: " << BLray[0] << " " << BLray[1] << " " << BLray[2] << endl;
  cout << "BRray: " << BRray[0] << " " << BRray[1] << " " << BRray[2] << endl;
  */

  double TL[3];
  double TR[3];
  double BL[3];
  double BR[3];

  //cout << "Distance from camera: " << this->DistanceFromCamera << endl;
  //cout << "Camera center: " << this->CameraCenter[0] << " " << this->CameraCenter[1] << " " << this->CameraCenter[2] << endl;

  for(unsigned int i = 0; i < 3; i++)
    {
    TL[i] = this->CameraCenter[i] + this->DistanceFromCamera * TLray[i];
    TR[i] = this->CameraCenter[i] + this->DistanceFromCamera * TRray[i];
    BL[i] = this->CameraCenter[i] + this->DistanceFromCamera * BLray[i];
    BR[i] = this->CameraCenter[i] + this->DistanceFromCamera * BRray[i];
    }

  /*
  cout << "TL: " << TL[0] << " " << TL[1] << " " << TL[2] << endl;
  cout << "TR: " << TR[0] << " " << TR[1] << " " << TR[2] << endl;
  cout << "BL: " << BL[0] << " " << BL[1] << " " << BL[2] << endl;
  cout << "BR: " << BR[0] << " " << BR[1] << " " << BR[2] << endl;
  */

  //cout << "Origin: " << Origin[0] << " " << Origin[1] << " " << Origin[2] << endl;
  //cout << "DistanceFromCamera: " << this->DistanceFromCamera << endl;

  //create a plane
  vtkSmartPointer<vtkPoints> points =
      vtkSmartPointer<vtkPoints>::New();

  points->InsertNextPoint(BL);
  points->InsertNextPoint(TL);
  points->InsertNextPoint(TR);
  points->InsertNextPoint(BR);

  vtkSmartPointer<vtkCellArray> polygons =
      vtkSmartPointer<vtkCellArray>::New();
  vtkSmartPointer<vtkPolygon> polygon =
      vtkSmartPointer<vtkPolygon>::New();
  polygon->GetPointIds()->SetNumberOfIds(4); //make a quad
  polygon->GetPointIds()->SetId(0, 0);
  polygon->GetPointIds()->SetId(1, 1);
  polygon->GetPointIds()->SetId(2, 2);
  polygon->GetPointIds()->SetId(3, 3);

  polygons->InsertNextCell(polygon);

  vtkSmartPointer<vtkPolyData> quad =
      vtkSmartPointer<vtkPolyData>::New();
  quad->SetPoints(points);
  quad->SetPolys(polygons);

  vtkSmartPointer<vtkFloatArray> textureCoordinates =
      vtkSmartPointer<vtkFloatArray>::New();
  textureCoordinates->SetNumberOfComponents(3);
  textureCoordinates->SetName("TextureCoordinates");

  float lowerLeft[2] = {0.0, 0.0};
  textureCoordinates->InsertNextTuple(lowerLeft);

  float upperLeft[2] = {0.0, 1.0};
  textureCoordinates->InsertNextTuple(upperLeft);

  float upperRight[2] = {1.0, 1.0};
  textureCoordinates->InsertNextTuple(upperRight);

  float lowerRight[2] = {1.0, 0.0};
  textureCoordinates->InsertNextTuple(lowerRight);

  quad->GetPointData()->SetTCoords(textureCoordinates);

  //apply the texture
  vtkSmartPointer<vtkTexture> texture =
      vtkSmartPointer<vtkTexture>::New();
  texture->SetInput(this->TextureImage);

  //vtkSmartPointer<vtkTexture> reprojectedTexture =
    //  vtkSmartPointer<vtkTexture>::New();
  //reprojectedTexture->SetInput(this->ReprojectedImage);

  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInput(quad);

  //vtkSmartPointer<vtkPolyDataMapper> reprojectedMapper =
    //  vtkSmartPointer<vtkPolyDataMapper>::New();
  //reprojectedMapper->SetInput(Quad);

  this->ImageActor->SetMapper(mapper);
  this->ImageActor->SetTexture(texture);

  /*
  //sequentially label the corners of the image (BL=0, proceeds clockwise)
  vtkSmartPointer<vtkPolyData> polydata =
      vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(points);

  vtkSmartPointer<vtkLabeledDataMapper> labelMapper =
    vtkSmartPointer<vtkLabeledDataMapper>::New();
  labelMapper->SetInputConnection(polydata->GetProducerPort());
  vtkSmartPointer<vtkActor2D> labelActor =
      vtkSmartPointer<vtkActor2D>::New();
  labelActor->SetMapper(labelMapper);
  this->Renderer->AddActor(labelActor);
  */

  this->Renderer->AddActor(this->ImageActor);
  this->Renderer->AddActor(this->AxesActor);

  /*
  //draw a sphere at the camera center
  vtkSmartPointer<vtkSphereSource> sphereSource =
      vtkSmartPointer<vtkSphereSource> ::New();
  //sphereSource->SetRadius(.5);
  sphereSource->SetRadius(.1);
  sphereSource->SetCenter(this->GetCameraCenter());
  sphereSource->Update();
  vtkSmartPointer<vtkPolyDataMapper> centerMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  centerMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkSmartPointer<vtkActor> centerActor =
      vtkSmartPointer<vtkActor>::New();
  centerActor->SetMapper(centerMapper);
  this->Renderer->AddActor(centerActor);
  */

  //this->ReprojectedImageActor->SetMapper(reprojectedMapper);
  //this->ReprojectedImageActor->SetTexture(reprojectedTexture);
}

/*
void vtkImageCamera::CreateViewingDirectionActor()
{
  double imageCenter[2] = {0,0};
  double ray[3];
  this->GetRay(imageCenter, ray);

  double p2[3];
  for(unsigned int i = 0; i < 3; i++)
    {
    p2[i] = this->CameraCenter[i] + ray[i];
    }
  //create a vtkPoints object and store the points in it
  vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
  pts->InsertNextPoint(this->CameraCenter);
  pts->InsertNextPoint(p2);

  //setup color
  unsigned char white[3] = {255, 255, 255};

  //setup the colors array
  vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
  colors->SetNumberOfComponents(3);
  colors->SetName("Colors");

  //add the colors we created to the colors array
  colors->InsertNextTupleValue(white);

  //Create the first line (between Origin and P0)
  vtkSmartPointer<vtkLine> line0 =
      vtkSmartPointer<vtkLine>::New();
  line0->GetPointIds()->SetId(0,0); //the second 0 is the index of the Origin in the vtkPoints
  line0->GetPointIds()->SetId(1,1); //the second 1 is the index of P0 in the vtkPoints

  //create a cell array to store the lines in and add the lines to it
  vtkSmartPointer<vtkCellArray> lines =
      vtkSmartPointer<vtkCellArray>::New();
  lines->InsertNextCell(line0);

  //create a polydata to store everything in
  vtkSmartPointer<vtkPolyData> linesPolyData =
      vtkSmartPointer<vtkPolyData>::New();
  linesPolyData->SetPoints(pts);
  linesPolyData->SetLines(lines);
  linesPolyData->GetCellData()->SetScalars(colors);

  //setup actor and mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInput(linesPolyData);

  this->ViewingDirectionActor->SetMapper(mapper);

}
*/

/*
void vtkImageCamera::CreateCornerActors(double tl[3], double tr[3])
{
  //the top left corner line should be green
  //the top right corner line should be red
  //vtkstd::cout << "CreateCornerActors" << vtkstd::endl;
  //vtkstd::cout << "tl: " << tl[0] << " " << tl[1] << " " << tl[2] << vtkstd::endl;
  //vtkstd::cout << "tr: " << tr[0] << " " << tr[1] << " " << tr[2] << vtkstd::endl;

  //create a vtkPoints object and store the points in it
  vtkSmartPointer<vtkPoints> pts =
      vtkSmartPointer<vtkPoints>::New();
  pts->InsertNextPoint(this->CameraCenter);
  pts->InsertNextPoint(tl);
  pts->InsertNextPoint(tr);

  //setup two colors - one for each line
  unsigned char green[3] = {0, 255, 0};
  unsigned char red[3] = {255, 0, 0};

  //setup the colors array
  vtkSmartPointer<vtkUnsignedCharArray> colors =
      vtkSmartPointer<vtkUnsignedCharArray>::New();
  colors->SetNumberOfComponents(3);
  colors->SetName("Colors");

  //add the colors we created to the colors array
  colors->InsertNextTupleValue(green);
  colors->InsertNextTupleValue(red);

  //Create the first line (between Origin and P0)
  vtkSmartPointer<vtkLine> line0 =
      vtkSmartPointer<vtkLine>::New();
  line0->GetPointIds()->SetId(0,0); //the second 0 is the index of the Origin in the vtkPoints
  line0->GetPointIds()->SetId(1,1); //the second 1 is the index of P0 in the vtkPoints

  //Create the second line (between Origin and P1)
  vtkSmartPointer<vtkLine> line1 =
      vtkSmartPointer<vtkLine>::New();
  line1->GetPointIds()->SetId(0,0); //the second 0 is the index of the Origin in the vtkPoints
  line1->GetPointIds()->SetId(1,2); //2 is the index of P1 in the vtkPoints

  //create a cell array to store the lines in and add the lines to it
  vtkSmartPointer<vtkCellArray> lines =
      vtkSmartPointer<vtkCellArray>::New();
  lines->InsertNextCell(line0);
  lines->InsertNextCell(line1);

  //create a polydata to store everything in
  vtkSmartPointer<vtkPolyData> linesPolyData =
      vtkSmartPointer<vtkPolyData>::New();
  linesPolyData->SetPoints(pts);
  linesPolyData->SetLines(lines);
  linesPolyData->GetCellData()->SetScalars(colors);

  //setup actor and mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInput(linesPolyData);

  this->CornerActor->SetMapper(mapper);

  //this->tlCornerActor;
  //this->trCornerActor;

}
*/

vtkActor* vtkImageCamera::GetImageActor()
{
  return this->ImageActor;
}
/*
vtkActor* vtkImageCamera::GetReprojectedImageActor()
{
  return this->ReprojectedImageActor;
}

vtkActor* vtkImageCamera::GetReprojectionRaysActor()
{
  return this->ReprojectionRaysActor;
}
*/

//----------------------------------------------------------------------------
void vtkImageCamera::PrintSelf(ostream& os, vtkIndent indent)
{
  //cout << "ImageCamera Superclass: " << this->Superclass->GetClassName() << endl;
  //cout << "ImageCamera Superclass: " << ::GetClassName() << endl;
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Width: " << this->GetWidth() << "\n";
  os << indent << "Height: " << this->GetHeight() << "\n";
}


void vtkImageCamera::GetTopLeftPixel(double* p)
{
  //get offsets to the sides of the image
  double x = (this->GetWidth() / 2.0);
  double y = (this->GetHeight() / 2.0);

  p[0] = x;   p[1] = y;
  //cout << "Top left pixel: " << x << " " << y << endl;
}

void vtkImageCamera::GetTopRightPixel(double* p)
{
  //get offsets to the sides of the image
  double x = -(this->GetWidth() / 2.0);
  double y = (this->GetHeight() / 2.0);

  p[0] = x;  p[1] = y;
  //cout << "Top right pixel: " << x << " " << y << endl;
}

void vtkImageCamera::GetBottomLeftPixel(double* p)
{
  //get offsets to the sides of the image
  double x = (this->GetWidth() / 2.0);
  double y = -(this->GetHeight() / 2.0);

  p[0] = x;   p[1] = y;
  //cout << "Bottom left pixel: " << x << " " << y << endl;
}

void vtkImageCamera::GetBottomRightPixel(double* p)
{
  //get offsets to the sides of the image
  double x = -(this->GetWidth() / 2.0);
  double y = -(this->GetHeight() / 2.0);

  p[0] = x;  p[1] = y;
  //cout << "Bottom right pixel: " << x << " " << y << endl;
}

void vtkImageCamera::OutputCorners()
{
  //output the pixel values of the corners of the image
  double TL[2];
  GetTopLeftPixel(TL);
  double TR[2];
  GetTopRightPixel(TR);
  double BL[2];
  GetBottomLeftPixel(BL);
  double BR[2];
  GetBottomRightPixel(BR);

  cout << "Corners:" << endl;
  cout << "TopLeft: (" << TL[0] << " , " << TL[1] << " )" << endl;
  cout << "TopRight: (" << TR[0] << " , " << TR[1] << " )" << endl;
  cout << "BottomLeft: (" << BL[0] << " , " << BL[1] << " )" << endl;
  cout << "BottomRight: (" << BR[0] << " , " << BR[1] << " )" << endl;
}

void vtkImageCamera::OutputRays()
{
  double TL[2];
  GetTopLeftPixel(TL);
  double TR[2];
  GetTopRightPixel(TR);
  double BL[2];
  GetBottomLeftPixel(BL);
  double BR[2];
  GetBottomRightPixel(BR);

  double TLRay[3];
  GetRay(TL, TLRay);
  double TRRay[3];
  GetRay(TR, TRRay);
  double BLRay[3];
  GetRay(BL, BLRay);
  double BRRay[3];
  GetRay(BR, BRRay);

  cout << "Rays:" << vtkstd::endl;
  cout << "TopLeftRay: (" << TLRay[0] << " , " << TLRay[1] << " , " << TLRay[2] << " )" << endl;
  cout << "TopRightRay: (" << TRRay[0] << " , " << TRRay[1] << " , " << TRRay[2] << " )" << endl;
  cout << "BottomLeftRay: (" << BLRay[0] << " , " << BLRay[1] << " , " << BLRay[2] << " )" << endl;
  cout << "BottomRightRay: (" << BRRay[0] << " , " << BRRay[1] << " , " << BRRay[2] << " )" << endl;
}


void vtkImageCamera::CopyImageCamera(vtkImageCamera* C)
{
  this->CopyPhysicalCamera(dynamic_cast<vtkPhysicalCamera*>(C));

  this->DistanceFromCamera = C->DistanceFromCamera;

  if(C->TextureImage)
    {
    this->TextureImage->DeepCopy(C->TextureImage);
    }


  this->ImageActor->ShallowCopy(C->ImageActor);
  this->CameraActor->ShallowCopy(C->CameraActor);
  /*
  this->ImageActor->DeepCopy(C->ImageActor);
  this->CameraActor->DeepCopy(C->CameraActor);
  */

}


void vtkImageCamera::CopyPhysicalCamera(vtkPhysicalCamera* physical)
{
  //vtkPhysicalCamera(
  //shouldn't have to copy this from parent class vtkPhysicalCamera?
  /*
  this->FocalLength = physical->FocalLength;

  for(unsigned int i = 0; i < 3; i++)
    {
    this->T[i] = physical->T[i];
    }

  this->R->DeepCopy(physical->R);

  for(unsigned int i = 0; i < 2; i++)
    {
    this->ImageCenter[i] = physical->ImageCenter[i];
    }

  */

  this->FocalLength = physical->GetFocalLength();
  //cout << "Old focal length: " << physical->GetFocalLength() << endl;
  //cout << "New focal length: " << this->FocalLength << endl;


  double* t = physical->GetCameraCenter();
  for(unsigned int i = 0; i < 3; i++)
    {
    this->CameraCenter[i] = t[i];
    }

  this->R->DeepCopy(physical->GetR());

  //double ic[2];
  double* ic = physical->GetImageCenter();
  for(unsigned int i = 0; i < 2; i++)
    {
    this->ImageCenter[i] = ic[i];
    }

}