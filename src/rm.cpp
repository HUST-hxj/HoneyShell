#include <iostream>
#include <fstream>

#include <stdio.h>
#include <dirent.h>

int main(int argc, char **argv) {
  for (int i = 1; i < argc; ++i) {
    DIR *dir_ptr = opendir(argv[i]);
    if (dir_ptr) {
      std::cerr << "错误：" << argv[i] << "是一个目录。" << std::endl;
      closedir(dir_ptr);
      continue;
    }
    if (remove(argv[i]))
      std::cerr << "错误：文件" << argv[i] <<"不存在。" << std::endl;
  }  
  return 0;
}