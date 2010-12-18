// .NAME vtkImageCamera - Texture map an image onto a quad based on extrinsic
// camera parameters.
// .SECTION Description
// vtkImageCamera computes the frustum of a camera from its extrinsic parameters.
// A quad is computed by finding 4 points along the frustum edges that are equidistant
// from the camera center. An image is then texture mapped onto this quad.
//
//(+x, +y)  (-x, +y)
//________
//|        |
//|        |
//|        |
//|________|
//(+x, -y)    (-x, -y)

// Must negate x coordinate of pixel (in center = (0,0), +x is right, +y is up coordinate system) before requesting the ray through that pixel!

#ifndef __vtkImageCamera_h
#define __vtkImageCamera_h

#include "vtkPhysicalCamera.h"
#include "vtkCommand.h"

#include "vtkSmartPointer.h"

#include <vtkstd/string>

class vtkInformation;
class vtkInformationVector;
class vtkActor;
class vtkImageData;
class vtkPoints;
class vtkRenderer;

class vtkImageCamera : public vtkPhysicalCamera
{
  public:
    vtkTypeMacro(vtkImageCamera,vtkPhysicalCamera);
    void PrintSelf(ostream& os, vtkIndent indent);
    static vtkImageCamera *New();
    void CopyImageCamera(vtkImageCamera*);
    void CopyPhysicalCamera(vtkPhysicalCamera*);

    void SetDistanceFromCamera(const double d);
    vtkGetMacro(DistanceFromCamera, double);

    void ReadTexture();
    //void InitializeCamera();
    vtkActor* GetImageActor();
    vtkActor* GetReprojectedImageActor();
    vtkActor* GetReprojectionRaysActor();

    void SetRenderer(vtkRenderer* renderer);

    unsigned int GetNumberOfRows();
    unsigned int GetNumberOfColumns();
    unsigned int GetHeight();
    unsigned int GetWidth();

    void OutputCorners();
    void OutputRays();

    void UpdateImageActor();
    void CreateViewingDirectionActor();

    void CreateReprojectedImage(vtkPoints* points);
    //void CreateReprojectionRays(vtkPoints* points);

    void SetTextureImage(vtkImageData* image);

    void SetTextureFileName(vtkstd::string filename);

  protected:
    vtkImageCamera();
    ~vtkImageCamera(){}

    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  private:
    vtkImageCamera(const vtkImageCamera&);  // Not implemented.
    void operator=(const vtkImageCamera&);  // Not implemented.

    vtkSmartPointer<vtkImageData> TextureImage; //The image to texture map onto the quad.
    //vtkSmartPointer<vtkImageData> ReprojectedImage; //The image resulting when 3D points are projected into the image.

    vtkSmartPointer<vtkActor> ImageActor; //The quad + texture.
    //vtkSmartPointer<vtkActor> ReprojectedImageActor;
    //vtkActor* tlCornerActor;
    //vtkActor* trCornerActor;
    //vtkActor* CornerActor;
    vtkSmartPointer<vtkActor> CornerActor;
    //vtkSmartPointer<vtkActor> ViewingDirectionActor;
    //vtkSmartPointer<vtkActor> ReprojectionRaysActor;

    vtkSmartPointer<vtkRenderer> Renderer;

    //Convenience functions to get the pixel coordinates of useful pixels.
    virtual void GetTopLeftPixel(double* p);
    virtual void GetTopRightPixel(double* p);
    virtual void GetBottomLeftPixel(double* p);
    virtual void GetBottomRightPixel(double* p);

    //void CreateCornerActors(double tl[3], double tr[3]);

    vtkstd::string TextureFileName;

    double DistanceFromCamera; //Controls which slice through the camera frustum the image is displayed on. The smaller this value is, the closer to the camera center the image is displayed.


};

#endif