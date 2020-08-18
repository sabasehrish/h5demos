# h5demos
Demos for writing serialized data products to HDF5 files (using HighFive interface, and H5CPP in future).  

## Using HighFive

This code currently depends on the following libraries:
* [HighFive](https://github.com/BlueBrain/HighFive)
* [HDF5](https://www.hdfgroup.org/downloads/hdf5/)
* MPI 
* [ROOT](https://root.cern/releases/release-62202/)
* [CMSSW](https://github.com/cms-sw/cmssw)

The first set of demo is done for writing CMS data products to the HDF5 files, 
hence to make sure all the ROOT dictionaries are available, it is currently run with CMSSW environment setup. 
MPI is not used in the first version. 
Following environment variables need to be set, `MPI_INC`, `MPI_LIB`, for MPI headers and libraries, 
`HDF5_INC` and `HDF5_LIB` for HDF5, 
`HIGHFIVE_INC` for HighFive headers, 
`ROOTSYS` for ROOT installation directory, and 
`CMSSWBASE` pointing to CMSSW code base (only 1-2 packages needed in reading). 

```git clone git@github.com:sabasehrish/h5demos.git
cd h5demos/highfive
mkdir build
//set all env variables
cmake ../
make
./main <input root file> <output HDF5 file> <batch size>
```

Weekly status updates on this work are [here](https://drive.google.com/drive/folders/17JDgXWoazBFQsiRwo6glsolIcOee6JX1?usp=sharing).

### Current HDF5 schema
Below is an example output using a small file with 20 events and 7 data products, 
each data product is serialized into `std::vector<char>` and resulting HDF5 
schema is creating 2D dataset for each product. 
```
F5 "out.h5" {
GROUP "/" {
   GROUP "run" {
      ATTRIBUTE "run_num" {
         DATATYPE  H5T_STD_I32LE
         DATASPACE  SCALAR
      }
      GROUP "lumi" {
         ATTRIBUTE "lumi_num" {
            DATATYPE  H5T_STD_I32LE
            DATASPACE  SCALAR
         }
         DATASET "BranchListIndexes" {
            DATATYPE  H5T_STD_I8LE
            DATASPACE  SIMPLE { ( 20, 18 ) / ( H5S_UNLIMITED, 18 ) }
         }
         DATASET "EventAuxiliary" {
            DATATYPE  H5T_STD_I8LE
            DATASPACE  SIMPLE { ( 20, 135 ) / ( H5S_UNLIMITED, 135 ) }
         }
         DATASET "EventProductProvenance" {
            DATATYPE  H5T_STD_I8LE
            DATASPACE  SIMPLE { ( 20, 42 ) / ( H5S_UNLIMITED, 42 ) }
         }
         DATASET "EventSelections" {
            DATATYPE  H5T_STD_I8LE
            DATASPACE  SIMPLE { ( 20, 41 ) / ( H5S_UNLIMITED, 41 ) }
         }
         DATASET "edmTriggerResults_TriggerResults__TESTOUTPUT." {
            DATATYPE  H5T_STD_I8LE
            DATASPACE  SIMPLE { ( 20, 90 ) / ( H5S_UNLIMITED, 90 ) }
         }
         DATASET "edmtestOtherThings_OtherThing_testUserTag_TESTOUTPUT." {
            DATATYPE  H5T_STD_I8LE
            DATASPACE  SIMPLE { ( 20, 6472 ) / ( H5S_UNLIMITED, 6472 ) }
         }
         DATASET "edmtestThings_Thing__TESTOUTPUT." {
            DATATYPE  H5T_STD_I8LE
            DATASPACE  SIMPLE { ( 20, 111 ) / ( H5S_UNLIMITED, 111 ) }
         }
      }
   }
}
}
```
