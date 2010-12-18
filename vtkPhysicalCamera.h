// .NAME vtkPhysicalCamera - Store extrinsic and intrinsic parameters of a camera.
// .SECTION Description
// The conventions of this class adhere to Hartley and Zisserman
// Store extrinsic and intrinsic parameters of a camera.
// Functions are provided to obtain 3D rays from the camera center
// through a pixel.
// CreateCameraActor() can be used to visualize the camera in 3D space.
//
// Note: Since VTK allows us to index pixels with negative indices, we do
// not need to do the typical last step of (x,y) to (u,v) coordinate conversion.
// The image is always stored with the image center (principal point) at index (0,0)
// The top left corner (looking from the camera center towards the world points)
// is (-w/2, h/2).

// R is stored so that x = KR[I | -C] X
// Camera center (C) is stored so that x = KR[I | -C] X

#ifndef __vtkPhysicalCamera_h
#define __vtkPhysicalCamera_h

#include "vtkDataObject.h" //super class
#include "vtkSmartPointer.h"

class vtkInformation;
class vtkInformationVector;
class vtkMatrix3x3;
class vtkMatrix4x4;
class vtkAxesActor;
class vtkCameraActor;
class vtkPoints;
class vtkImageData;

class vtkPhysicalCamera : public vtkDataObject
{
  public:
    vtkTypeMacro(vtkPhysicalCamera,vtkDataObject);

    static vtkPhysicalCamera *New();
    void CopyPhysicalCamera(vtkPhysicalCamera*);

    void PrintSelf(ostream& os, vtkIndent indent);

    vtkSetMacro(FocalLength, double);
    vtkGetMacro(FocalLength, double);

    vtkGetVectorMacro(CameraCenter, double, 3);
    vtkSetVectorMacro(CameraCenter, double, 3);

    //rotation matrix
    void SetR(vtkSmartPointer<vtkMatrix3x3>);
    //vtkGetMacro(R, vtkMatrix3x3*);
    vtkSmartPointer<vtkMatrix3x3> GetR();

    void GetK(vtkSmartPointer<vtkMatrix3x3> K);

    vtkSetVector2Macro(ImageCenter,double);
    vtkGetVector2Macro(ImageCenter,double);

    //backward projection (2D->3D, getting rays through image pixels)
    virtual bool GetRay(double* PixelCoordinate2, double* Ray);
    void ViewingDirection(double* v);

    //forward projection (3D -> 2D, projecting 3D points into an image)
    void ProjectPoint(double p[3], double pix[2]);
    void CreateImage(vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkImageData> image);

    vtkSmartPointer<vtkAxesActor> GetAxesActor();
    vtkSmartPointer<vtkCameraActor> GetCameraActor();

    void UpdateCameraActor();

    void GetP(vtkSmartPointer<vtkMatrix4x4> p);

  protected:
    vtkPhysicalCamera();
    ~vtkPhysicalCamera() {}

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

    //Intrinsic parameters
    double FocalLength;

    //Extrinsic parameters
    double CameraCenter[3];
    vtkSmartPointer<vtkMatrix3x3> R; //rotation
    //vtkSmartPointer<vtkMatrix3x3> K; //intrinsic parameter matrix

    double ImageCenter[2];

    vtkSmartPointer<vtkAxesActor> AxesActor;//A axes indicator representing the camera center and orientation.
    vtkSmartPointer<vtkCameraActor> CameraActor;

  private:
    vtkPhysicalCamera(const vtkPhysicalCamera&);  // Not implemented.
    void operator=(const vtkPhysicalCamera&);  // Not implemented.

};

#endif
