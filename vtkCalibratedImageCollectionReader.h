// .NAME vtkCalibratedImageCollectionReader - An abstract class for reading CalibratedImageCollections
// .SECTION Description
// vtkCalibratedImageCollectionReader is an abstract class that will serve as a superclass for several
// CalibratedImageCollection readers

#ifndef __vtkCalibratedImageCollectionReader_h
#define __vtkCalibratedImageCollectionReader_h

#include "vtkAlgorithm.h"
#include <vtkstd/vector>

class vtkCalibratedImageCollection;
class vtkPolyData;

class vtkCalibratedImageCollectionReader : public vtkAlgorithm
{
public:
  vtkTypeMacro(vtkCalibratedImageCollectionReader,vtkAlgorithm);
  //void PrintSelf(ostream& os, vtkIndent indent);

  vtkCalibratedImageCollection* GetOutputImageCollection();
  vtkPolyData* GetOutputPoints();
  //void SetOutput(vtkDataObject* d);

protected:
  vtkCalibratedImageCollectionReader();
  ~vtkCalibratedImageCollectionReader();

  virtual int ProcessRequest(vtkInformation*,
                             vtkInformationVector**,
                             vtkInformationVector*);

  virtual int RequestDataObject(
                                vtkInformation* request,
                                vtkInformationVector** inputVector,
                                vtkInformationVector* outputVector );

  virtual int RequestInformation(
                                 vtkInformation* request,
                                 vtkInformationVector** inputVector,
                                 vtkInformationVector* outputVector );

  virtual int RequestData(
                          vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector ) = 0;

  virtual int RequestUpdateExtent(
                                  vtkInformation*,
                                  vtkInformationVector**,
                                  vtkInformationVector* );

  virtual int FillOutputPortInformation( int port, vtkInformation* info );

private:

  vtkCalibratedImageCollectionReader(const vtkCalibratedImageCollectionReader&);  // Not implemented.
  void operator=(const vtkCalibratedImageCollectionReader&);  // Not implemented.

};

#endif
