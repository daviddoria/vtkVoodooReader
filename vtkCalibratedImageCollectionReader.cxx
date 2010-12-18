#include "vtkCalibratedImageCollectionReader.h"
#include "vtkCalibratedImageCollection.h"
#include "vtkImageCamera.h"

#include "vtkObjectFactory.h"
#include "vtkDataObject.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"

#include "vtkSmartPointer.h"
#include "vtkMatrix3x3.h"
#include "vtkMath.h"
#include "vtkDirectory.h"
#include "vtkPolyData.h"

#include <sstream>
#include <vtkstd/algorithm>

#include <vtksys/SystemTools.hxx>


vtkCalibratedImageCollectionReader::vtkCalibratedImageCollectionReader()
{
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(2);
}

vtkCalibratedImageCollectionReader::~vtkCalibratedImageCollectionReader()
{

}

///////////// VTK Filter Functions //////////////

//----------------------------------------------------------------------------
vtkCalibratedImageCollection* vtkCalibratedImageCollectionReader::GetOutputImageCollection()
{
  return vtkCalibratedImageCollection::SafeDownCast(this->GetOutputDataObject(0));
}

//----------------------------------------------------------------------------
vtkPolyData* vtkCalibratedImageCollectionReader::GetOutputPoints()
{
  return vtkPolyData::SafeDownCast(this->GetOutputDataObject(1));
}


//----------------------------------------------------------------------------
int vtkCalibratedImageCollectionReader::ProcessRequest(vtkInformation* request,
                                  vtkInformationVector** inputVector,
                                  vtkInformationVector* outputVector)
{
  // Create an output object of the correct type.
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT()))
  {
    return this->RequestDataObject(request, inputVector, outputVector);
  }
  // generate the data
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
  {
    return this->RequestData(request, inputVector, outputVector);
  }

  if(request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT()))
  {
    return this->RequestUpdateExtent(request, inputVector, outputVector);
  }

  // execute information
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()))
  {
    return this->RequestInformation(request, inputVector, outputVector);
  }

  return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
int vtkCalibratedImageCollectionReader::FillOutputPortInformation(
                                             int port, vtkInformation* info)
{
  if(port == 0)
    {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkCalibratedImageCollection");
    }
  else if(port == 1)
    {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
    }

  return 1;
}


//----------------------------------------------------------------------------
int vtkCalibratedImageCollectionReader::RequestDataObject(
                                     vtkInformation* vtkNotUsed(request),
    vtkInformationVector** vtkNotUsed(inputVector),
                                      vtkInformationVector* outputVector )
{

  //setup image collection port
  vtkInformation* outInfoImageCollection = outputVector->GetInformationObject( 0 );
  vtkCalibratedImageCollection* outputImageCollection = vtkCalibratedImageCollection::SafeDownCast(
                                          outInfoImageCollection->Get( vtkDataObject::DATA_OBJECT() ) );
  if ( ! outputImageCollection)
    {
    outputImageCollection = vtkCalibratedImageCollection::New();
    outInfoImageCollection->Set( vtkDataObject::DATA_OBJECT(), outputImageCollection );
    outputImageCollection->FastDelete();
    outputImageCollection->SetPipelineInformation( outInfoImageCollection );
    this->GetOutputPortInformation(0)->Set(
                                   vtkDataObject::DATA_EXTENT_TYPE(), outputImageCollection->GetExtentType() );
    }

  //setup points port
  vtkInformation* outInfoPoints = outputVector->GetInformationObject( 1 );
  vtkPolyData* outputPoints = vtkPolyData::SafeDownCast(
      outInfoPoints->Get( vtkDataObject::DATA_OBJECT() ) );
  if ( ! outputPoints )
    {
    outputPoints = vtkPolyData::New();
    outInfoPoints->Set( vtkDataObject::DATA_OBJECT(), outputPoints );
    outputPoints->FastDelete();
    outputPoints->SetPipelineInformation( outInfoPoints );
    this->GetOutputPortInformation(1)->Set(
                                   vtkDataObject::DATA_EXTENT_TYPE(), outputPoints->GetExtentType() );
    }

  return 1;
}

//----------------------------------------------------------------------------
int vtkCalibratedImageCollectionReader::RequestInformation(
                                      vtkInformation* vtkNotUsed(request),
    vtkInformationVector** vtkNotUsed(inputVector),
                                      vtkInformationVector* vtkNotUsed(outputVector))
{
  // do nothing let subclasses handle it

  return 1;
}

//----------------------------------------------------------------------------
int vtkCalibratedImageCollectionReader::RequestUpdateExtent(
                                       vtkInformation* vtkNotUsed(request),
    vtkInformationVector** inputVector,
    vtkInformationVector* vtkNotUsed(outputVector))
{
  int numInputPorts = this->GetNumberOfInputPorts();
  for (int i=0; i<numInputPorts; i++)
  {
    int numInputConnections = this->GetNumberOfInputConnections(i);
    for (int j=0; j<numInputConnections; j++)
    {
      vtkInformation* inputInfo = inputVector[i]->GetInformationObject(j);
      inputInfo->Set(vtkStreamingDemandDrivenPipeline::EXACT_EXTENT(), 1);
    }
  }
  return 1;
}
