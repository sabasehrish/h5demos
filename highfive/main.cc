#include "h5_write_chars.h"

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Please specify input file, output file and batch size\n";
    return 1;
  }
  auto input_root_fname = argv[1];
  auto output_h5_fname = argv[2];
  int batch_size = atoi(argv[3]);
  auto run_num = 1;
  auto lumi_num = 1;
  h5_write_ds(input_root_fname, batch_size, run_num, lumi_num, output_h5_fname);
  return 0;
}

