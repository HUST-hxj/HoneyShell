#include <fstream>
#include <iostream>
#include <string>

#include <sys/stat.h>
#include <dirent.h>

int main(int argc, char **argv) {
  std::string src_name = argv[1], dst_name = argv[2];
  DIR *src_ptr = opendir(src_name.c_str());
  if (src_ptr) {
    std::cerr << "错误：是一个目录。" << std::endl;
    closedir(src_ptr);
    return 0;
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
      return 0;
    }
  }
  std::fstream src, dst;
  src.open(src_name, std::ios_base::in | std::ios_base::binary);
  if (!src.is_open()) {
    std::cerr << "错误：源文件不存在。" << std::endl;
    return 0;
  }
  dst.open(dst_name, std::ios_base::out | std::ios_base::binary);
  if (!dst.is_open()) {
    std::cerr << "错误：目录非法。" << std::endl;
    return 0;
  }
  char buf[202106];
  while (!src.eof()) {
    src.read(buf, 202106);
    dst.write(buf, src.gcount());
  }
  src.close();
  dst.close();
  return 0;
}