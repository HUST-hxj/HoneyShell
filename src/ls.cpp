#include <iostream>
#include <fstream>
#include <string>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

void Ls(std::string dir_name) {
  DIR *dir_ptr = opendir(dir_name.c_str());
  if (!dir_ptr) {
    std::fstream fs;
    fs.open(dir_name, std::ios_base::in);
    if (fs.is_open()) {
      int i;
      for (i = dir_name.size() - 1; dir_name[i] != '/'; i--);
      std::cout << dir_name.substr(i+1) << std::endl;
      return;
    }
    std::cerr << "错误：非法路径。" << std::endl;
    return;
  }
  dirent *dirent_ptr;
  while (dirent_ptr = readdir(dir_ptr))
    if (dirent_ptr->d_name[0] != '.')
      std::cout << dirent_ptr->d_name << ' ';
  std::cout << std::endl;
  closedir(dir_ptr);
}

void LsList(std::string dir_name) {
  DIR *dir_ptr = opendir(dir_name.c_str());
  if (!dir_ptr) {
    std::fstream fs;
    fs.open(dir_name, std::ios_base::in);
    if (fs.is_open()) {
      int i;
      for (i = dir_name.size() - 1; dir_name[i] != '/'; i--);
      std::cout << dir_name.substr(i+1) << std::endl;
      return;
    }
    std::cerr << "错误：非法路径。" << std::endl;
    return;
  }
  dirent *dirent_ptr;
  while (dirent_ptr = readdir(dir_ptr))
    if (dirent_ptr->d_name[0] != '.') {
      struct stat buf;
      stat((dir_name + "/" + dirent_ptr->d_name).c_str(), &buf);
      //type
      if (buf.st_mode & S_IFREG)
        std::cout << '-';
      else if (buf.st_mode & S_IFDIR)
        std::cout << 'd';
      else if (buf.st_mode & S_IFCHR)
        std::cout << 'c';
      else if (buf.st_mode & S_IFBLK)
        std::cout << 'b';  
      else if (buf.st_mode & S_IFIFO)
        std::cout << 'p';
      else if (buf.st_mode & S_IFLNK)
        std::cout << 'l';
      else if (buf.st_mode & S_IFSOCK)
        std::cout << 's';
      //access, etc.
      std::cout << ((buf.st_mode & S_IRUSR) ? 'r' : '-')
                << ((buf.st_mode & S_IWUSR) ? 'w' : '-')
                << ((buf.st_mode & S_IXUSR) ? 'x' : '-')
                << ((buf.st_mode & S_IRGRP) ? 'r' : '-')
                << ((buf.st_mode & S_IWGRP) ? 'w' : '-')
                << ((buf.st_mode & S_IXGRP) ? 'x' : '-')
                << ((buf.st_mode & S_IROTH) ? 'r' : '-')
                << ((buf.st_mode & S_IWOTH) ? 'w' : '-')
                << ((buf.st_mode & S_IXOTH) ? 'x' : '-')
                << ' ' << buf.st_nlink 
                << ' ' << getgrgid(buf.st_gid)->gr_name 
                << ' ' << getpwuid(buf.st_uid)->pw_name 
                << ' ' << buf.st_size << ' ' 
                << std::string(ctime(&buf.st_mtim.tv_sec)).substr(0, std::string(ctime(&buf.st_mtim.tv_sec)).size() - 1)
                << ' ' << dirent_ptr->d_name << std::endl;
    }
  closedir(dir_ptr);
}

int main(int argc, char **argv) {
  if (argc == 1) {
    char buf[2021];
    Ls(getcwd(buf,2021));
  } else if (argc == 2 && argv[1] == std::string("-l")) {
    char buf[2021];
    LsList(getcwd(buf,2021));
  } else if (argc > 2 && argv[1] == std::string("-l"))
    LsList(argv[2]);
  else 
    Ls(argv[1]);
  return 0;
}