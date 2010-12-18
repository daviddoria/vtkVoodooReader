cmake_minimum_required(VERSION 2.6)

PROJECT(vtkVoodooReader)
ENABLE_TESTING()

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

########## Examples ###########
ADD_EXECUTABLE(vtkVoodooReaderExample_PointsAndImages vtkVoodooReaderExample_PointsAndImages.cxx
vtkVoodooReader.cxx
vtkCalibratedImageCollectionReader.cxx
vtkCalibratedImageCollection.cxx
vtkImageCamera.cxx
vtkPhysicalCamera.cxx
)
TARGET_LINK_LIBRARIES(vtkVoodooReaderExample_PointsAndImages vtkHybrid )


ADD_EXECUTABLE(vtkVoodooReaderExample_Points vtkVoodooReaderExample_Points.cxx
vtkVoodooReader.cxx
vtkCalibratedImageCollectionReader.cxx
vtkCalibratedImageCollection.cxx
vtkImageCamera.cxx
vtkPhysicalCamera.cxx
)
TARGET_LINK_LIBRARIES(vtkVoodooReaderExample_Points vtkHybrid )

ADD_EXECUTABLE(vtkVoodooReaderExample_Visualize vtkVoodooReaderExample_Visualize.cxx
vtkVoodooReader.cxx
vtkCalibratedImageCollectionReader.cxx
vtkCalibratedImageCollection.cxx
vtkImageCamera.cxx
vtkPhysicalCamera.cxx
)
TARGET_LINK_LIBRARIES(vtkVoodooReaderExample_Visualize vtkHybrid )

############ Tests ############
ADD_EXECUTABLE(vtkVoodooReaderTest vtkVoodooReaderTest.cxx
vtkVoodooReader.cxx
vtkCalibratedImageCollectionReader.cxx
vtkCalibratedImageCollection.cxx
vtkImageCamera.cxx
vtkPhysicalCamera.cxx
)
TARGET_LINK_LIBRARIES(vtkVoodooReaderTest vtkHybrid )
ADD_TEST(vtkVoodooReaderTest vtkVoodooReaderTest)
