#include "h5_write_chars.h"

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Please specify input file, output file and batch size\n";
    return 1;
  }
  auto input_root_fname = argv[1];
  auto output_h5_fname = argv[2];
  int batch_size = atoi(argv[3]);
  h5_write_chars(input_root_fname, batch_size, output_h5_fname);
  return 0;
}

