#include <iostream>
#include <fstream>

#include <dirent.h>
#include <unistd.h>

void Ln(std::string src_name, std::string dst_name) {
  DIR *src_ptr = opendir(src_name.c_str());
  if (src_ptr) {
    std::cerr << "错误：是一个目录。" << std::endl;
    closedir(src_ptr);
    return;
  }
  DIR *dst_ptr = opendir(dst_name.c_str());
  if (dst_ptr) {
    closedir(src_ptr);
    int i;
    for (i = src_name.size() - 1; src_name[i] != '/'; i--);
    dst_name = dst_name + "/" + src_name.substr(i + 1);
  } else {
    if (dst_name.back() == '/') {
      std::cerr << "错误：目录非法。" << std::endl;
      return;
    }
  }
  std::fstream src, dst;
  src.open(src_name, std::ios_base::in | std::ios_base::binary);
  if (!src.is_open()) {
    std::cerr << "错误：源文件不存在。" << std::endl;
    return;
  }
  dst.open(dst_name, std::ios_base::in | std::ios_base::binary);
  if (dst.is_open()) {
    dst.close();
    std::cerr << "错误：目标文件已存在" << std::endl;
    return;
  }
  dst.open(dst_name, std::ios_base::out | std::ios_base::binary);
  if (!dst.is_open()) {
    std::cerr << "错误：目录非法。" << std::endl;
    return;
  }
  src.close();
  dst.close();
  remove(dst_name.c_str());
  link(src_name.c_str(), dst_name.c_str());
}

void LnSymbolic(std::string src_name, std::string dst_name) {
  DIR *src_ptr = opendir(src_name.c_str());
  if (src_ptr) {
    std::cerr << "错误：是一个目录。" << std::endl;
    closedir(src_ptr);
    return;
  }
  DIR *dst_ptr = opendir(dst_name.c_str());
  if (dst_ptr) {
    closedir(src_ptr);
    int i;
    for (i = src_name.size() - 1; src_name[i] != '/'; i--);
    dst_name = dst_name + "/" + src_name.substr(i + 1);
  } else {
    if (dst_name.back() == '/') {
      std::cerr << "错误：目录非法。" << std::endl;
      return;
    }
  }
  std::fstream src, dst;
  src.open(src_name, std::ios_base::in | std::ios_base::binary);
  if (!src.is_open()) {
    std::cerr << "错误：源文件不存在。" << std::endl;
    return;
  }
  dst.open(dst_name, std::ios_base::in | std::ios_base::binary);
  if (dst.is_open()) {
    dst.close();
    std::cerr << "错误：目标文件已存在" << std::endl;
    return;
  }
  dst.open(dst_name, std::ios_base::out | std::ios_base::binary);
  if (!dst.is_open()) {
    std::cerr << "错误：目录非法。" << std::endl;
    return;
  }
  src.close();
  dst.close();
  remove(dst_name.c_str());
  symlink(src_name.c_str(), dst_name.c_str());  
}

int main(int argc, char **argv) {
  if (argc > 3 && argv[1] == std::string("-s"))
    LnSymbolic(argv[2], argv[3]);
  else 
    Ln(argv[1], argv[2]);
  return 0;
}