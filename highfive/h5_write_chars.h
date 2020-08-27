#ifndef H5_WRITE_CHARS_H
#define H5_WRITE_CHARS_H
#include <highfive/H5DataSet.hpp>
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5DataType.hpp>
#include <highfive/H5File.hpp>

#include "h5_write_1D_chars.h"
#include "serialize_dataprods.h"
#include "utilities.h"

using namespace HighFive;
using product_t = std::vector<char>;

void
write_batch(std::vector<product_t> const& products, 
            std::vector<std::string> const& ds_names, 
            long unsigned int batch, 
            long unsigned int nbatch, 
            long unsigned int round,
            Group lumi) 
{
  auto num_prods = ds_names.size();
  for (int prod_index = 0; prod_index < num_prods; ++prod_index) {
    auto prod_size = products[prod_index].size(); 
    auto tmp = get_prods(products, prod_index, num_prods);
    auto sizes = get_sizes(tmp);
    auto ds_name = ds_names[prod_index];
    if(!lumi.exist(ds_names[prod_index])){ //if dataset doesn't exist, create it otherwise extend it
      std::cout << "creating dataset for: " << ds_name << "\n";
      DataSetCreateProps props;
      props.add(Chunking(std::vector<hsize_t>{batch, prod_size}));
      DataSpace dataspace = DataSpace({1, prod_size}, {DataSpace::UNLIMITED, DataSpace::UNLIMITED});
      DataSet dataset = lumi.createDataSet<char>(ds_name, dataspace, props); 
      //resize the dataset to match the ragged array
      //currently only works for batch size 1
      for (auto i = 1; i<batch; ++i){
        dataset.resize({i+1, sizes[i]});
        }
        dataset.write(tmp);
      }
    else {
      DataSet dataset = lumi.getDataSet(ds_names[prod_index]);
      std::cout << "extending dataset for: " << dataset.getPath() << "\n";
      auto offset = batch*round;
      for (auto i = 0; i<nbatch; ++i){
        dataset.resize({offset+i+1, sizes[i]}); 
    }
    dataset.select({offset, 0}, {nbatch, prod_size}).write(tmp); //get_prods(products, prod_index, num_prods));
  }
  DataSet reads = lumi.getDataSet(ds_names[prod_index]);
  std::vector<std::vector<char>> chars;
  reads.read(chars);
  for (int i=0; i<chars.size(); ++i)
    std::cout << "read size: "<< chars[i].size() << "\n";
  }
}


void h5_write_ds(char* rname, 
                 int batch, 
                 int run_num, 
                 int lumi_num,
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
  
  File file(outname, File::ReadWrite | File::Create);
  Group run = file.createGroup("run");
  Attribute a = run.createAttribute<int>("run_num", DataSpace::From(run_num));
  a.write(run_num);
  Group lumi = run.createGroup("lumi");
  Attribute b = lumi.createAttribute<int>("lumi_num", DataSpace::From(lumi_num));
  b.write(lumi_num);

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
        
        //write_batch(products, ds_names, batch, nbatch, round, lumi);
        write_1D_chars(products, ds_names, batch, nbatch, round, lumi);
        nbatch = 0;
        ++round;
        products.clear();
      }
      ++ievt;
  }

  file.flush();
}

#endif
