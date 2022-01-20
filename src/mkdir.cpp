#include <iostream>

#include <sys/stat.h>
#include <dirent.h>

int main(int argc, char **argv) {
  DIR *dir_ptr = opendir(argv[1]);
  if (dir_ptr) {
    std::cerr << "错误：目录已存在。" << std::endl;
    closedir(dir_ptr);
    return 0;
  }  
  if (mkdir(argv[1], 0777))
    std::cerr << "错误：目录非法。" << std::endl;
  return 0;
}