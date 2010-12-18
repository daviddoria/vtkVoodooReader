#include "vtkVoodooReader.h"
#include "vtkCalibratedImageCollection.h"
#include "vtkImageCamera.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"

#include "vtkDirectory.h"
#include "vtkImageReader2.h"
#include "vtkImageReader2Factory.h"
#include "vtkImageFlip.h"
#include "vtkJPEGReader.h"
#include "vtkMatrix3x3.h"
#include "vtkPolyData.h"
#include "vtkPNGReader.h"
#include "vtkVertexGlyphFilter.h"
#include "vtkSmartPointer.h"

#include <algorithm>

#include <vtksys/ios/sstream>
#include <vtksys/SystemTools.hxx>

vtkStandardNewMacro(vtkVoodooReader);

vtkVoodooReader::vtkVoodooReader()
{
  this->Cameras = vtkSmartPointer<vtkCalibratedImageCollection>::New();
  this->Points = vtkSmartPointer<vtkPolyData>::New();
  this->ImageFileExtension = ".png";
}

void vtkVoodooReader::SetImageFileExtension(std::string extension)
{
  this->ImageFileExtension = extension;
}

void vtkVoodooReader::SetFileName(std::string filename)
{
  this->FileName = filename;
}

vtkstd::string vtkVoodooReader::GetFileName()
{
  return this->FileName;
}

vtkSmartPointer<vtkPolyData> vtkVoodooReader::GetPoints()
{
  return this->Points;
}

vtkSmartPointer<vtkCalibratedImageCollection> vtkVoodooReader::GetCameras()
{
  return this->Cameras;
}

void vtkVoodooReader::SetImageDirectory(vtkstd::string directory)
{
  this->ImageDirectory = directory;
}

vtkstd::string vtkVoodooReader::GetImageDirectory()
{
  return this->ImageDirectory;
}

int vtkVoodooReader::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  /*
  // get the output images
  vtkInformation* outInfoImageCollection = outputVector->GetInformationObject(0);

  vtkCalibratedImageCollection* outputImageCollection = vtkCalibratedImageCollection::SafeDownCast(
      outInfoImageCollection->Get(vtkDataObject::DATA_OBJECT()));

  // get the output points
  vtkInformation* outInfoPoints = outputVector->GetInformationObject(1);

  vtkPolyData* outputPoints = vtkPolyData::SafeDownCast(
		  outInfoPoints->Get(vtkDataObject::DATA_OBJECT()));
  */

  // Read the images
  vtkSmartPointer<vtkDirectory> directory =
    vtkSmartPointer<vtkDirectory>::New();

  std::vector<std::string> imageFileNames;

  int opened = directory->Open(this->ImageDirectory.c_str());
  if(!opened)
    {
    vtkWarningMacro("Invalid ImageDirectory!");
    }
  else
    {
    int numImages = directory->GetNumberOfFiles();

    for (int i = 0; i < numImages; i++)
      {
      std::string filename = directory->GetFile(i);
      //vtkstd::cout << "File " << i << ": " << FileName << vtkstd::endl;

      std::string ext = vtksys::SystemTools::GetFilenameLastExtension(filename);

      std::string fileString = std::string(this->ImageDirectory);
      fileString += "/";
      fileString += filename;

      if(this->ImageFileExtension.compare(ext) == 0)
        {
        imageFileNames.push_back(fileString);
        }

      }

    //alphabetize
    std::sort(imageFileNames.begin(), imageFileNames.end());
    }

  ////////////////////////////
  ifstream infile;
  infile.open(this->FileName.c_str());
  if(!infile)
    {
    vtkErrorMacro("Could not open file " << this->FileName << "!");
    return 0;
    }

  std::string line;

  /*
  vtkSmartPointer<vtkCalibratedImageCollection> imageCollection =
      vtkSmartPointer<vtkCalibratedImageCollection>::New();
    */

  unsigned int cameraCounter = 0;
  //while(!infile.eof())
    //{
    //getline(infile, line);
  while(getline(infile, line))
  {
    //cout << "Line: " << line << endl;
    if(line.compare("# 3D Feature Points") == 0) //read in cameras until we get to this line
      {
      break;
      }

      if(line[0] == '#')
        {
        // do nothing, it is a comment
        continue;
        }

        double Cx, Cy, Cz, Ax, Ay, Az, Hx, Hy, Hz, Vx, Vy, Vz, K3, K5, sx, sy, ppx, ppy, f, fov, H0x, H0y, H0z, V0x, V0y, V0z;
        unsigned int width, height;

        std::stringstream ss;
        ss << line;
        ss >> Cx >> Cy >> Cz >> Ax >> Ay >> Az >> Hx >> Hy >> Hz >> Vx >> Vy >> Vz >> K3 >> K5 >> sx >> sy >> width >> height >> ppx >> ppy >> f >> fov >> H0x >> H0y >> H0z >> V0x >> V0y >> V0z;

        double cameraCenter[3];
        cameraCenter[0] = Cx;
        cameraCenter[1] = Cy;
        cameraCenter[2] = Cz;

        vtkSmartPointer<vtkMatrix3x3> r =
          vtkSmartPointer<vtkMatrix3x3>::New();
        r->SetElement(0, 0, H0x);
        r->SetElement(0, 1, H0y);
        r->SetElement(0, 2, H0z);
        r->SetElement(1, 0, V0x);
        r->SetElement(1, 1, V0y);
        r->SetElement(1, 2, V0z);
        r->SetElement(2, 0, Ax);
        r->SetElement(2, 1, Ay);
        r->SetElement(2, 2, Az);

        vtkSmartPointer<vtkImageCamera> camera =
          vtkSmartPointer<vtkImageCamera>::New();
        camera->SetR(r);
        camera->SetCameraCenter(cameraCenter);
        camera->SetFocalLength(f);

        vtkSmartPointer<vtkImageReader2Factory> readerFactory =
          vtkSmartPointer<vtkImageReader2Factory>::New();
        vtkImageReader2 * reader = readerFactory->CreateImageReader2(imageFileNames[cameraCounter].c_str());
        //std::cout << "Reading " << imageFileNames[cameraCounter] << std::endl;
        reader->SetFileName(imageFileNames[cameraCounter].c_str());
        reader->Update();

        vtkSmartPointer<vtkImageFlip> flipXFilter =
          vtkSmartPointer<vtkImageFlip>::New();
        flipXFilter->SetFilteredAxis(0); // flip x axis
        flipXFilter->SetInputConnection(reader->GetOutputPort());
        flipXFilter->Update();

        vtkSmartPointer<vtkImageFlip> flipYFilter =
          vtkSmartPointer<vtkImageFlip>::New();
        flipYFilter->SetFilteredAxis(1); // flip y axis
        flipYFilter->SetInputConnection(flipXFilter->GetOutputPort());
        flipYFilter->Update();

        camera->SetTextureImage(flipYFilter->GetOutput());

        camera->SetTextureFileName(imageFileNames[cameraCounter]);

        //camera->UpdateCameraActor();
        //camera->UpdateImageActor();
        //imageCollection->AddCamera(camera);
        this->Cameras->AddCamera(camera);

        reader->Delete();
        cameraCounter++;
        //std::cout << "Added " << cameraCounter << " cameras." << endl << line << std::endl;
    }

  vtkSmartPointer<vtkPoints> points =
    vtkSmartPointer<vtkPoints>::New();

  // Throw away the # X Y Z line
  getline(infile, line);

  while(!infile.eof())
    {
    getline(infile, line);
    //std::cout << line << std::endl;
    double x,y,z;
    vtkstd::stringstream ss;
    ss << line;
    ss >> x >> y >> z;
    points->InsertNextPoint(x,y,z);
    }


  // Close the input file
  infile.close();

  // Create polydata
  vtkSmartPointer<vtkPolyData> polydata =
    vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(points);

  vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  vertexFilter->SetInputConnection(polydata->GetProducerPort());
  vertexFilter->Update();

  this->Points->ShallowCopy(vertexFilter->GetOutput());

  /*
  //outputPoints->ShallowCopy(polydata);
  outputPoints->ShallowCopy(vertexFilter->GetOutput());
  */
  //outputImageCollection->DeepCopy(imageCollection);

  return 1;
}


//----------------------------------------------------------------------------
void vtkVoodooReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "File Name: " << this->FileName << endl;
}
