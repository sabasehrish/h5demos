#include <highfive/H5DataSet.hpp>
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5DataType.hpp>
#include <highfive/H5File.hpp>

#include "serialize_dataprods.h"

namespace test_rw {
  struct BranchInfo {
    std::string name_;
    TBranch* branch_;
    void* buffer_ = nullptr;
  };
}

using namespace HighFive;

//inroot is only used to construct the root file structure
//outFile is the output root file that we can look at for verification
//h5fname is the input hdf5 file we will be reading data from
//dsname is the dataset name we will be reading from
void
hdf5_read_ds(std::string h5fname, std::string dsname, char* inroot) {
  //read corresponding root file to create the structure
  auto f = TFile::Open(inroot);
  auto e = f->Get<TTree>("Events");
  auto l = e->GetListOfBranches();

  //create output root file
  
  TFile outFile{"test_out.root","RECREATE"};
  TTree outEvents("Events","Events");
  std::vector<test_rw::BranchInfo> branchInfo;

  std::vector<TClass*> classes;
  classes.reserve(l->GetEntriesFast());
  branchInfo.reserve(l->GetEntriesFast());
  for( int i=0; i< l->GetEntriesFast(); ++i) {
    auto b = dynamic_cast<TBranch*>((*l)[i]);
    std::cout<<b->GetName()<<std::endl;
    std::cout<<b->GetClassName()<<std::endl;
    b->SetupAddresses();
    TClass* class_ptr=nullptr;
    EDataType type;
    b->GetExpectedType(class_ptr,type);
    classes.push_back( class_ptr);

    test_rw::BranchInfo bi;
    bi.name_ = b->GetName();
    bi.branch_ = outEvents.Branch(b->GetName(),b->GetClassName(), nullptr);
    bi.buffer_ = *reinterpret_cast<void**>(bi.branch_->GetAddress());
    branchInfo.push_back(bi);
  }


  File file(h5fname, File::ReadOnly);
  std::vector<std::vector<char>> read_data;
  DataSet dataset = file.getDataSet(dsname);
  auto sz = dataset.getSpace().getDimensions()[0];
  dataset.read(read_data);
  std::cout << "read " << read_data.size() << " items\n";
  //loop over all events and then for this branch write to the out file
  TBufferFile readBufferFile{TBuffer::kRead};
  for (int i = 0; i< sz; ++i) {
    std::cout << read_data[i].size() << " is num bytes read\n";
    readBufferFile.SetBuffer(&read_data[i].front(), read_data[i].size(), kFALSE);
    classes[0]->ReadBuffer(readBufferFile, branchInfo[0].buffer_);
    readBufferFile.Reset();
    outEvents.Fill();
  }
  outFile.Write();
}


int main(int argc, char* argv[]) {
  auto h5name = argv[1];
  auto rname = argv[2];
  hdf5_read_ds(h5name, "EventAuxiliary", rname);
  return 0;
}
