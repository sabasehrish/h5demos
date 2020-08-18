#include <highfive/H5DataSet.hpp>
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5DataType.hpp>
#include <highfive/H5File.hpp>

#include "serialize_dataprods.h"

using namespace HighFive;
using product_t = std::vector<char>;


std::vector<product_t> 
get_prods(std::vector<product_t> const & input, 
         int prod_index, 
         int stride) {
  std::vector<product_t> tmp; 
  for(int j = prod_index; j< input.size(); j+=stride) {
    tmp.push_back(input[j]);
  }
  return tmp;
}

void
write_batch(std::vector<product_t> const& products, 
            std::vector<std::string> const& ds_names, 
            long unsigned int batch, 
            long unsigned int nbatch, 
            long unsigned int round,
            std::string const&  outname) 
{
  File file(outname, File::ReadWrite | File::Create);
  auto num_prods = ds_names.size();
  for (int prod_index = 0; prod_index < num_prods; ++prod_index) {
    auto prod_size = products[prod_index].size(); 
    if(!file.exist(ds_names[prod_index])){ //if dataset doesn't exist, create it otherwise extend it
      std::cout << "creating dataset for: " << ds_names[prod_index] << "\n";
      DataSetCreateProps props;
      props.add(Chunking(std::vector<hsize_t>{batch, prod_size}));
      DataSpace dataspace = DataSpace({batch, prod_size}, {DataSpace::UNLIMITED, prod_size});
      DataSet dataset = file.createDataSet<char>(ds_names[prod_index], dataspace, props); 
      dataset.write(get_prods(products, prod_index, num_prods));
      }
    else {
      DataSet dataset = file.getDataSet(ds_names[prod_index]);
      std::cout << "extending dataset for: " << dataset.getPath() << "\n";
      auto offset = batch*round;
      dataset.resize({offset+nbatch, prod_size}); 
      dataset.select({offset, 0}, {nbatch, prod_size}).write(get_prods(products, prod_index, num_prods));
      }
  file.flush();
  }
}

void h5_write_chars(char* rname, 
                    int batch, 
                    std::string const& outname) {
  auto f = TFile::Open(rname);
  auto e = f->Get<TTree>("Events");
  auto l = e->GetListOfBranches();
  auto classes = return_classes(l);
  auto nevts = e->GetEntriesFast(); // we may not have that information available in real case

  auto ds_names = return_dsetnames(l); // ds_names have names of all data products to be written
  auto num_datasets = ds_names.size(); 
  std::vector<product_t> products; // a container for all data products
  auto nbatch = 0; // index in the batch 
  auto round = 0;  // number of batches (rounds)
  auto ievt = 0; 
  //while we have more events, accumulate a batch of events and write that out 
  while (ievt < nevts) {
    e->GetEntry(ievt);
    for(int ib = 0; ib< l->GetEntriesFast(); ++ib) {
      auto b = dynamic_cast<TBranch*>((*l)[ib]);
      auto dataprod_name = b->GetName();
      auto blob =  return_blob(b, classes[ib]);
      std::cout << blob.size() << " is the length of "<< dataprod_name << "in event " << ievt << "\n"; 
      products.push_back(blob);
    }
      nbatch++;
      // if you have collected batch events, then do the write
      // nbatch will be different if in the last batch we see last event before end of batch 
      if(nbatch == batch || ievt == nevts-1) {
        write_batch(products, ds_names, batch, nbatch, round, outname);
        nbatch = 0;
        ++round;
        products.clear();
      }
      ++ievt;
  }
}

