#ifndef H5_WRITE_1D_CHARS_H
#define H5_WRITE_1D_CHARS_H

#include "utilities.h"

using namespace HighFive;
using product_t = std::vector<char>;

std::vector<char>
flatten(std::vector<std::vector<char>> const& tmp) {
  std::vector<char> tmp1 = tmp[0];
  for (int i = 1; i<tmp.size(); ++i)
     tmp1.insert(end(tmp1), begin(tmp[i]), end(tmp[i]));
  return tmp1;
}

void
write_1D_chars(std::vector<product_t> const& products, 
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
    auto tmp1 = flatten(tmp);
    auto ds_name = ds_names[prod_index];
    auto sum_prods = std::accumulate(sizes.begin(), sizes.end(), 0);
    //create and extend dataset for sizes here!
    auto szds_name = ds_name+"_sz";
    if(!lumi.exist(szds_name)) {
      std::cout << "creating size dataset for : " << ds_name << '\n';
      DataSetCreateProps props;
      props.add(Chunking(std::vector<hsize_t>{batch}));
      DataSpace dataspace = DataSpace({batch}, {DataSpace::UNLIMITED});
      DataSet dataset = lumi.createDataSet<int>(szds_name, dataspace, props);
      dataset.write(sizes);
    } else {
      DataSet dataset = lumi.getDataSet(szds_name); 
      std::cout << "extending size dataset for : " << dataset.getPath() << "\n";     
      auto offset = batch*round;
      dataset.resize({offset+batch});
      dataset.select({offset}, {nbatch}).write(sizes);
    }
    //now write data products
    if(!lumi.exist(ds_names[prod_index])){ //if dataset doesn't exist, create it otherwise extend it
      std::cout << "creating dataset for: " << ds_name << "\n";
      DataSetCreateProps props;
      props.add(Chunking(std::vector<hsize_t>{batch*sum_prods}));
      DataSpace dataspace = DataSpace({batch*sum_prods}, {DataSpace::UNLIMITED});
      DataSet dataset = lumi.createDataSet<char>(ds_name, dataspace, props); 
      dataset.write(tmp1);
      }
    else {
      DataSet dataset = lumi.getDataSet(ds_names[prod_index]);
      std::cout << "extending dataset for: " << dataset.getPath() << "\n";
      auto offset = batch*round*sum_prods;
      dataset.resize({offset+(batch*sum_prods)}); 
    dataset.select({offset}, {nbatch*sum_prods}).write(tmp1); //get_prods(products, prod_index, num_prods));
  }
  }
}
#endif
