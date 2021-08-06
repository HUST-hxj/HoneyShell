#include <iostream>
#include <vector>
#include <sstream>
#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

int main(int argc, char **argv) {  
  if (argc == 1) {
    std::cerr << "错误：参数不足。" << std::endl;
    return 0;
  }
  std::vector<std::string> args;
  std::string buf;
  for (int i = 1; i < argc; i++)
    if (i != 1 || argv[i][0] != '-')
      args.push_back(argv[i]);
  std::getline(std::cin, buf);  
  if (argc > 2 && std::string(argv[1]).substr(0,2) == "-d") {
    int last = 0;
    for (int i = 0; i < buf.size(); ++i)
      if (buf[i] == argv[1][2]) {
        args.push_back(buf.substr(last, i - last));
        last = i + 1;
      }
    if (buf.back() != argv[1][2])
      args.push_back(buf.substr(last));
  } else {
    std::stringstream ss;  
    ss << buf;
    while (ss >> buf)
      args.push_back(buf);  
  }  
  char **argv_ = (char **)malloc((args.size() + 1) * sizeof(char *));
  for (int i = 0; i < args.size(); i++) {
    std::stringstream ss;
    ss << args[i];
    argv_[i] = (char *)malloc((args[i].size() + 1) * sizeof(char));
    ss >> argv_[i];
    argv_[i][args[i].size()] = '\0';
  }
  argv_[args.size()] = NULL;  
  pid_t pid = fork();
  if (pid == 0) {
    execvp(argv_[0], argv_);
    std::cerr << "未找到命令：" << argv_[0] << std::endl;
    return 0;
  }
  waitpid(pid, NULL, 0);
  for (int i = 0; i < args.size(); i++)
    free(argv_[i]);
  free(argv_);
  return 0;
}