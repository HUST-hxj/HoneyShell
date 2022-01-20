#include <fstream>
#include <iostream>
#include <string>

#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>

void Mvdir(std::string src_name, std::string dst_name) {
  DIR *src_ptr = opendir(src_name.c_str()), *dst_ptr = opendir(dst_name.c_str());
  std::fstream src, dst;
  dirent *dirent_ptr;
    while (dirent_ptr = readdir(src_ptr))
      if (std::string(dirent_ptr->d_name) != "." && std::string(dirent_ptr->d_name) != "..") {
        if (dirent_ptr->d_type == DT_DIR) {
          mkdir((dst_name + "/" + dirent_ptr->d_name).c_str(), 0777);
          Mvdir(src_name + "/" + dirent_ptr->d_name, dst_name + "/" + dirent_ptr->d_name);
          continue;
        }
        src.open(src_name + "/" + dirent_ptr->d_name, std::ios_base::in | std::ios_base::binary);
        dst.open(dst_name + "/" + dirent_ptr->d_name, std::ios_base::out | std::ios_base::binary);
        char buf[202106];
        while (!src.eof()) {
          src.read(buf, 202106);
          dst.write(buf, src.gcount());
        }
        src.close();
        dst.close();
        remove((src_name + "/" + dirent_ptr->d_name).c_str());
      }       
    closedir(src_ptr);
    remove(src_name.c_str());
}

int main(int argc, char **argv) {
  std::string src_name = argv[1], dst_name = argv[2];
  DIR *src_ptr = opendir(src_name.c_str());
  std::fstream src, dst;
  if (src_ptr) {
    DIR *dst_ptr = opendir(dst_name.c_str());
    if (!dst_ptr) {
      dst.open(dst_name);
      if (dst.is_open()) {
        dst.close();
        std::cerr << "错误：无法用目录覆盖文件。" << std::endl;
        closedir(src_ptr);
        return 0;
      }
      if (mkdir(dst_name.c_str(), 0777)) {
        std::cerr << "错误：目录非法。" << std::endl;
        closedir(src_ptr);
        return 0;
      }
    } else {
      int i;
      for (i = src_name.size() - 1; src_name[i] != '/'; i--);
      closedir(dst_ptr); 
      dst_name = dst_name + "/" + src_name.substr(i + 1);
      dst_ptr = opendir(dst_name.c_str());
      if (dst_ptr) {  
        dirent *dirent_ptr;     
        while (dirent_ptr = readdir(src_ptr))
          if (std::string(dirent_ptr->d_name) != "." && std::string(dirent_ptr->d_name) != "..") {
            std::cerr << "错误：目录非空。" << std::endl;
            closedir(src_ptr);
            closedir(dst_ptr);
            return 0;
          } 
        closedir(dst_ptr);
      } else {
        mkdir(dst_name.c_str(), 0777);    
      }      
    }
    closedir(src_ptr);
    Mvdir(src_name, dst_name);
  } else {
    src.open(src_name, std::ios_base::in | std::ios_base::binary);
    if (!src.is_open()) {
      std::cerr << "错误：源文件不存在。" << std::endl;
      return 0;
    }
    DIR *dst_ptr = opendir(dst_name.c_str());
    if (!dst_ptr) {
      dst.open(dst_name, std::ios_base::out | std::ios_base::binary);
      if (!dst.is_open()) {
        std::cerr << "错误：目录非法。" << std::endl;
        src.close();
        return 0;
      }
      char buf[202106];
      while (!src.eof()) {
        src.read(buf, 202106);
        dst.write(buf, src.gcount());
      }
      src.close();
      dst.close();
    } else {
      int i;
      for (i = src_name.size() - 1; src_name[i] != '/'; i--);
      while (dst_ptr) {     
        closedir(dst_ptr);  
        dst_name = dst_name + "/" + src_name.substr(i + 1);
        dst_ptr = opendir(dst_name.c_str());
      }
      dst.open(dst_name, std::ios_base::out | std::ios_base::binary);
      char buf[202106];
      while (!src.eof()) {
        src.read(buf, 202106);
        dst.write(buf, src.gcount());
      }
      src.close();
      dst.close();
      closedir(dst_ptr);
    }
    remove(src_name.c_str());
  }
  return 0;
}