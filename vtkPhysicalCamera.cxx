#include "vtkPhysicalCamera.h"

#include "vtkObjectFactory.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkCellArray.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkImageData.h"
#include "vtkMath.h"
#include "vtkMatrix3x3.h"
#include "vtkMatrix4x4.h"
#include "vtkCaptionActor2D.h"
#include "vtkTextProperty.h"
#include "vtkAxesActor.h"
#include "vtkCameraActor.h"
#include "vtkLine.h"
#include "vtkAppendPolyData.h"
#include "vtkTransform.h"
#include "vtkTextActor.h"
#include "vtkPoints.h"
#include "vtkPoints2D.h"
#include "vtkImageData.h"
#include "vtkImageCanvasSource2D.h"
#include "vtkImageFlip.h"
#include "vtkCamera.h"
#include "vtkTransform.h"

vtkStandardNewMacro(vtkPhysicalCamera);

vtkPhysicalCamera::vtkPhysicalCamera()
{
  this->CameraCenter[0] = 0.0;
  this->CameraCenter[1] = 0.0;
  this->CameraCenter[2] = 0.0;

  this->R = vtkSmartPointer<vtkMatrix3x3>::New();
  double ang = vtkMath::Pi();
  R->SetElement(0,0,cos(ang));
  R->SetElement(0,2,-sin(ang));
  R->SetElement(2,0,sin(ang));
  R->SetElement(2,2,cos(ang));

  this->FocalLength = 1.0;

  this->ImageCenter[0] = 0.0;
  this->ImageCenter[1] = 0.0;

  this->AxesActor = vtkSmartPointer<vtkAxesActor>::New();
  this->AxesActor->SetTotalLength(.5, .5, .5);
  this->AxesActor->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(10);
  this->AxesActor->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(10);
  this->AxesActor->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(10);
  this->AxesActor->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->ShadowOff();
  this->AxesActor->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->ShadowOff();
  this->AxesActor->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->ShadowOff();
  this->AxesActor->GetXAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
  this->AxesActor->GetYAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
  this->AxesActor->GetZAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();

  this->CameraActor = vtkSmartPointer<vtkCameraActor>::New();
  vtkSmartPointer<vtkCamera> camera =
      vtkSmartPointer<vtkCamera>::New();
  this->CameraActor->SetCamera(camera);

  //this->CameraActor->GetUserTransform()->Scale(.01, .01, .01);
  //vtkTransform::SafeDownCast (this->CameraActor->GetUserTransform())->Scale(.01, .01, .01);
  //vtkTransform* t = this->CameraActor->GetUserTransform();
  //vtkTransform* t = vtkTransform::SafeDownCast (this->CameraActor->GetUserTransform());
  vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
  t->Scale(.001, .001, .001);
  this->CameraActor->SetUserTransform(t);
  //cout << *t << endl;
}


vtkSmartPointer<vtkAxesActor> vtkPhysicalCamera::GetAxesActor()
{
  return this->AxesActor;
}

vtkSmartPointer<vtkCameraActor> vtkPhysicalCamera::GetCameraActor()
{
  return this->CameraActor;
}

void vtkPhysicalCamera::SetR(vtkSmartPointer<vtkMatrix3x3> r)
{
  this->R->DeepCopy(r);
}

vtkSmartPointer<vtkMatrix3x3> vtkPhysicalCamera::GetR()
{
  return this->R;
}

bool vtkPhysicalCamera::GetRay(double* pixelCoordinate, double* ray)
{
  //vtkstd::cout << "Image center: " << this->ImageCenter[0] << " " << this->ImageCenter[1] << vtkstd::endl;

  //PixelCoordinate should be specified as [x y]
  //Ray will be a 3d vector [x y z] that is the direction of the 3d pixel from the camera center

  //From Hartley and Zisserman, if P = [M | P4], then the ray through a pixel x is
  // inv(M) * (x - p4)
  /*
  double u = pixelCoordinate[0] - this->ImageCenter[0];
  double v = pixelCoordinate[1] - this->ImageCenter[1];
  */

  double u = pixelCoordinate[0];
  double v = pixelCoordinate[1];

  double x[3]; //image point
  x[0] = u;
  x[1] = v;
  x[2] = 1;
  //x[2] = this->FocalLength;

  //get the camera matrix
  vtkSmartPointer<vtkMatrix4x4> P = vtkSmartPointer<vtkMatrix4x4>::New();
  this->GetP(P);

  //compute (x - P4)
  double temp[3];
  for(unsigned int i = 0; i < 3; i++)
    {
    temp[i] = x[i] - P->GetElement(i,3);
    }

  //cout << "temp: " << temp[0] << " " << temp[1] << " " << temp[2] << endl;

  //extract M
  vtkSmartPointer<vtkMatrix3x3> M = vtkSmartPointer<vtkMatrix3x3>::New();
  for(unsigned int i = 0; i < 3; i++)
    {
    for(unsigned int j = 0; j < 3; j++)
      {
        M->SetElement(i,j, P->GetElement(i,j));
      }
    }

  // invert M
  M->Invert();
  //cout << "Mt: " << *M << endl;

  //Multiply inv(M) by (x - p4) and store the result in 'ray'
  //M->MultiplyPoint(temp, ray);
  M->MultiplyPoint(x, ray); //don't actually use (x - P4), but rather just x??!!

  //cout << "ray: " << ray[0] << " " << ray[1] << " " << ray[2] << endl;

  //get a unit vector in the direction of the ray
  vtkMath::Normalize(ray);

  //negate the ray (it is backwards for some reason?)
  /*
  for(unsigned int i = 0; i < 3; i++)
    {
    ray[i] = -ray[i];
    }
  */

  //ray[2] = -ray[2]; //have to negate this because we want +z to be forward (why?) !!!

  //cout << "ray (normalized): " << ray[0] << " " << ray[1] << " " << ray[2] << endl;

  return true; //everything went ok
}


void vtkPhysicalCamera::ViewingDirection(double* v)
{
  //this should simply be a GetRay(imageCenter)

  double p[3] = {0, 0, 0};
  p[2] = -1;
  this->R->MultiplyPoint(p, v);
  vtkMath::Normalize(v);
}
//----------------------------------------------------------------------------
void vtkPhysicalCamera::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  vtkstd::cout << "PhysicaCamera::PrintSelf" << vtkstd::endl;
  os << indent << "FocalLength = " << this->FocalLength << "\n";
  os << indent << "ImageCenter = (" << this->ImageCenter[0] << " , " << this->ImageCenter[1] << ") \n";
  os << indent << "CameraCenter: (" << this->CameraCenter[0] << ", " << this->CameraCenter[1] << ", " << this->CameraCenter[2] << ")" << "\n";
  os << indent << "R: " << "\n" << *(this->R) << "\n";

}

void vtkPhysicalCamera::CopyPhysicalCamera(vtkPhysicalCamera* P)
{
  this->FocalLength = P->FocalLength;

  for(unsigned int i = 0; i < 3; i++)
    {
    this->CameraCenter[i] = P->CameraCenter[i];
    }

  this->R->DeepCopy(P->R);

  for(unsigned int i = 0; i < 2; i++)
    {
    this->ImageCenter[i] = P->ImageCenter[i];
    }

}


void vtkPhysicalCamera::UpdateCameraActor()
{
  //copy the 3x3 R into the upper left 3x3 block of the 4x4 m
  vtkSmartPointer<vtkMatrix4x4> m =
      vtkSmartPointer<vtkMatrix4x4>::New();
  for(unsigned int i = 0; i < 3; i++)
    {
    for(unsigned int j = 0; j < 3; j++)
      {
      m->SetElement(i,j,this->R->GetElement(i,j));
      }
    }

    m->Transpose(); //not sure why this needs to be here?

  //copy T into the last column of m
  for(unsigned int i = 0; i < 3; i++)
    {
    m->SetElement(i,3,this->CameraCenter[i]);
    }


  /*
  // The axes are positioned with a user transform
  vtkSmartPointer<vtkTransform> transform =
      vtkSmartPointer<vtkTransform>::New();
  transform->SetMatrix(m);
  this->CameraActor->SetUserTransform(transform);
  this->AxesActor->SetUserTransform(transform);
  */

  this->CameraActor->SetUserMatrix(m);
  this->AxesActor->SetUserMatrix(m);

  //set the font properties of the axes labels
  /*
  this->CameraActor->GetXAxisCaptionActor2D ()->GetTextActor()->SetTextScaleModeToNone();
  this->CameraActor->GetYAxisCaptionActor2D ()->GetTextActor()->SetTextScaleModeToNone();
  this->CameraActor->GetZAxisCaptionActor2D ()->GetTextActor()->SetTextScaleModeToNone();
  int fontSize = 10;
  this->CameraActor->GetXAxisCaptionActor2D ()->GetCaptionTextProperty()->SetFontSize (fontSize);
  this->CameraActor->GetYAxisCaptionActor2D ()->GetCaptionTextProperty()->SetFontSize (fontSize);
  this->CameraActor->GetZAxisCaptionActor2D ()->GetCaptionTextProperty()->SetFontSize (fontSize);
  */
}

void vtkPhysicalCamera::GetP(vtkSmartPointer<vtkMatrix4x4> P)
{
  //P is supposed to be 3x4, but making it 4x4 we will just never use the result of the 4th row times anything.

  //cout << "p: " << p[0] << " " << p[1] << " " << p[2] << endl;
  //cout << "R: " << endl << *(this->R) << endl;
  //cout << "CameraCenter: " << CameraCenter[0] << " " << CameraCenter[1] << " " << CameraCenter[2] << endl;

  vtkSmartPointer<vtkMatrix3x3> camR =
      vtkSmartPointer<vtkMatrix3x3>::New();
  camR->DeepCopy(this->R);
  //vtkMatrix3x3::Transpose(this->R,camR);

  vtkSmartPointer<vtkMatrix3x3> K =
      vtkSmartPointer<vtkMatrix3x3>::New();
  this->GetK(K);
  //cout << "K: " << endl << *K << endl;

  vtkSmartPointer<vtkMatrix3x3> KR =
      vtkSmartPointer<vtkMatrix3x3>::New();
  vtkMatrix3x3::Multiply3x3(K,camR,KR);
  //cout << "KR: " << endl << *KR << endl;


  //copy KR into the first 3x3 block of P
  for(unsigned int i = 0; i < 3; i++)
    {
    for(unsigned int j = 0; j < 3; j++)
      {
      P->SetElement(i,j,KR->GetElement(i,j));
      }
    }

  //copy -KRC into the first 3 entries of the last column of P
  double krc[3];

  //cout << "Camera center: " << this->CameraCenter[0] << " " << this->CameraCenter[1] << " " << this->CameraCenter[2] << endl;

  KR->MultiplyPoint(this->CameraCenter, krc);
  //cout << "KRC: " << krc[0] << " " << krc[1] << " " << krc[2] << endl;
  for(unsigned int i = 0; i < 3; i++)
    {
    P->SetElement(i,3,-krc[i]);
    }

  //cout << "P: " << endl << *P << endl;

}

void vtkPhysicalCamera::ProjectPoint(double p[3], double pixel[2])
{
  //cout << "p: " << p[0] << " " << p[1] << " " << p[2] << endl;
  //make p homogeneous
  double p4[4];
  for(unsigned int i = 0; i < 3; i++)
    {
    p4[i] = p[i];
    }
  p4[3] = 1.0;
  //cout << "p4: " << p4[0] << " " << p4[1] << " " << p4[2] << " " << p4[3] << endl;

  vtkSmartPointer<vtkMatrix4x4> P = vtkSmartPointer<vtkMatrix4x4>::New();
  this->GetP(P);
  //cout << "P: " << *P << endl;
  double pix[4];
  P->MultiplyPoint(p4,pix);

  //cout << "pix: " << pix[0] << " " << pix[1] << " " << pix[2] << " " << pix[3] << endl;

  //divide by the 3rd coordinate of the result to get a 2D pixel value
  pixel[0] = pix[0]/pix[2];
  pixel[1] = pix[1]/pix[2];

  //cout << "pixel: " << pixel[0] << " " << pixel[1] << endl;
}

void vtkPhysicalCamera::GetK(vtkSmartPointer<vtkMatrix3x3> K)
{
  K->Identity();
  K->SetElement(0,0,this->FocalLength);
  K->SetElement(1,1,this->FocalLength);

}

void vtkPhysicalCamera::CreateImage(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkImageData> image)
{
  vtkSmartPointer<vtkPoints2D> points2D = vtkSmartPointer<vtkPoints2D>::New();

  for(vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
    {
    double p[3];
    points->GetPoint(i, p);
    //cout << "P: " << p[0] << " " << p[1] << " " << p[2] << endl;

    double pixel[2];
    ProjectPoint(p, pixel);
    //cout << "pixel: " << pixel[0] << " " << pixel[1] << endl;

    points2D->InsertNextPoint(pixel[0], pixel[1]);
    }

  cout << "There are " << points2D->GetNumberOfPoints() << " points in the image." << endl;

  double bounds[4];
  points2D->GetBounds(bounds);
  cout << "Bounds: " << bounds[0] << " " << bounds[1] << " " << bounds[2] << " " << bounds[3] << endl;

  vtkSmartPointer<vtkImageCanvasSource2D> canvasSource = vtkSmartPointer<vtkImageCanvasSource2D>::New();
  canvasSource->SetNumberOfScalarComponents(3);
  canvasSource->SetScalarTypeToUnsignedChar();
  canvasSource->SetExtent(bounds[0], bounds[1], bounds[2], bounds[3], 0, 0);
  canvasSource->SetDrawColor(255, 255, 255, 255); //white
  canvasSource->FillBox(bounds[0], bounds[1], bounds[2], bounds[3]);
  canvasSource->SetDrawColor(255, 0, 0, 255); //red
  for(vtkIdType i = 0; i < points2D->GetNumberOfPoints(); i++)
    {
    double p[2];
    points2D->GetPoint(i, p);
    canvasSource->DrawCircle(p[0], p[1], 2);
    }
  canvasSource->Update();

  //this is the image defined looking at the "back" of the image plane,
  //so we need to flip it since we will be looking at it from the front
  vtkSmartPointer<vtkImageFlip> flipFilter = vtkSmartPointer<vtkImageFlip>::New();
  flipFilter->SetInputConnection(canvasSource->GetOutputPort());
  flipFilter->Update();

  //image->ShallowCopy(canvasSource->GetOutput());
  image->ShallowCopy(flipFilter->GetOutput());

}
