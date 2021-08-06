#include <fstream>

int main(int argc, char **argv) {
  for (int i = 1; i < argc; ++i) {
    std::fstream fs;
    fs.open(argv[i], std::ios_base::app);
    fs.close();
  }  
  return 0;
}