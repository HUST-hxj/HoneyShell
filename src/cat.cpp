#include <iostream>
#include <fstream>
#include <string>

#include <dirent.h>

int main(int argc, char **argv) {
  if (argc == 1) {
    std::string temp;
    while (std::cin >> temp)
      std::cout << temp << std::endl;
  } else {
    for (int i = 1; i < argc; ++i) {
      DIR *dir = opendir(argv[i]);
      if (dir) {
        std::cerr << "错误：是一个目录。" << std::endl;
        closedir(dir);
        continue;
      }
      std::fstream fs;
      fs.open(argv[i], std::ios_base::in);
      if (!fs.is_open()) {
        std::cerr << "错误：文件不存在。" << std::endl;
        continue;
      }
      while (!fs.eof()) {
        char temp[2021];
        fs.getline(temp, 2021);
        std::cout << temp << std::endl;
      }  
      fs.close();
    }
    
  }  
  return 0;
}