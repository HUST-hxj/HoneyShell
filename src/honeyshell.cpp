#include <iostream>
#include <sstream>
#include <string>
#include <deque>

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <wait.h>

void SingleCommand(std::deque<std::string> command) {
  if (command[0] == "echo") { //echo
    if (command.size() == 1) {
      std::cout << std::endl; 
    } else {
      for (int i = 1; i < command.size(); ++i)
        std::cout << command[i] << ' ';
      std::cout << std::endl;
    }      
  } else if (command[0] == "pwd") { //pwd
    char buf[2021];
    std::cout << getcwd(buf, 2021) << std::endl;
  } else if (command[0] == "cd") { //cd
    if (command.size() == 1) {
      std::cerr << "错误：参数不足。" << std::endl;
      return;
    }
    if (chdir(command[1].c_str())) {
      std::cerr << "错误：非法目录。" << std::endl;
      return;
    }
  } else if (command[0] == "export") { //export
    if (command.size() == 1) {
      std::cerr << "错误：参数不足。" << std::endl;
      return;
    }
    int i;
    for (i = 0; i < command[1].size() && command[1][i] != '='; ++i);
    setenv(command[1].substr(0,i).c_str(), command[1].substr(i + 1).c_str(), 1);
  } else if (command[0] == "kill") { //kill
    if (kill(std::atoi(command[1].c_str()), SIGINT))
      std::cerr << "错误：进程不存在。" << std::endl;
  } else if (command[0] == "exit") { //exit
    exit(0);
  } else { //default   
    char **argv = (char **)malloc((command.size() + 1) * sizeof(char *));
    for (int i = 0; i < command.size(); ++i) {
      std::stringstream ss;
      ss << command[i];
      argv[i] = (char *)malloc((command[i].size() + 1) * sizeof(char));        
      ss >> argv[i];
      argv[i][command[i].size()] = '\0';
    }
    argv[command.size()] = NULL;
    pid_t pid = fork();
    if (pid == 0) {
      signal(SIGINT, NULL);
      execvp(argv[0], argv); 
      std::cerr << "未找到命令：" << argv[0] << std::endl;
      return;
    }
    waitpid(pid, NULL, 0);
    for (int i = 0; i < command.size(); ++i)
      free(argv[i]);
    free(argv);
  }
}  

int main() {  
  int fd_in = -1, fd_out = -1, std_in = STDIN_FILENO, std_out = STDOUT_FILENO, pipedes[2] = {0, 0};
  std::deque<std::string> command;
  std::string path = getenv("PATH");
  char buf_temp[2021];
  path = std::string(getcwd(buf_temp, 2021)) + "/bin:" + path;
  setenv("PATH", path.c_str(), true);
  while (1) {
    if (fd_in > 0) {
      close(fd_in);
      fd_in = -1;
    }
    if (fd_out > 0) {
      close(fd_out);
      fd_out = -1;
    }
    if (std_in != STDIN_FILENO) {
      dup2(std_in, STDIN_FILENO);
      close(std_in);
      std_in = STDIN_FILENO;      
    }
    if (std_out != STDOUT_FILENO) {
      dup2(std_out, STDOUT_FILENO);    
      close(std_out);    
      std_out = STDOUT_FILENO;      
    }
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
      std::cerr << "已结束的进程：" << pid << std::endl;
    char buf[2021];
    std::string cwd = getcwd(buf, 2021);
    std::string line, temp;
    signal(SIGINT, SIG_IGN);
    char *line_temp = readline(("[" + cwd + "]" + "$ ").c_str());
    if (!line_temp)
      return 0;
    if (std::string(line_temp).empty())
      continue;
    add_history(line_temp);
    line = line_temp;
    std::stringstream ss;
    ss << line;
    while (ss >> temp) {
      if (temp[0] == '$') {
        char *buf = getenv(temp.substr(1).c_str());
        if (!buf) continue; 
        temp = buf;
      }      
      if (temp == "<") {
        ss >> temp;        
        if ((fd_in = open(temp.c_str(), O_RDONLY)) == -1) {
          std::cerr << "错误：文件不存在。" << std::endl;
          command.clear();
          break;
        }
        std_in = dup(STDIN_FILENO);
        dup2(fd_in, STDIN_FILENO);
      } else if (temp == ">") {
        ss >> temp;
        if ((fd_out = open(temp.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR)) == -1) {
          std::cerr << "错误：写入文件失败。" << std::endl;
          command.clear();
          break;
        }
        std_out = dup(STDOUT_FILENO);        
        dup2(fd_out, STDOUT_FILENO);        
      } else if (temp == "|") {
        if (command.empty()) {
          std::cerr << "错误：命令非法。" << std::endl;
          break;
        }
        if (command.front() == "|") {    
          if (std_in != STDIN_FILENO) {
            dup2(std_in, STDIN_FILENO);
            close(std_in);
            std_in = STDIN_FILENO;      
          }      
          command.pop_front();
          std_in = dup(STDIN_FILENO);
          dup2(pipedes[0], STDIN_FILENO);
        }
        if (pipe(pipedes)) {
          std::cerr << "错误：管道建立失败。" << std::endl;
          command.clear();
          break;
        }
        std_out = dup(STDOUT_FILENO);
        dup2(pipedes[1], STDOUT_FILENO); 
        SingleCommand(command);
        command.clear();
        command.push_back(temp);
        close(pipedes[1]);
        pipedes[1] = 0;
        dup2(std_out, STDOUT_FILENO);    
        close(std_out);    
        std_out = STDOUT_FILENO;      
        continue;
      } else if (temp == "&") {
        if (command.empty()) {
          std::cerr << "错误：命令非法。" << std::endl;
          break;
        }
        pid_t pid = fork();
        if (pid == 0) {
          SingleCommand(command);
          command.clear();
          return 0;
        }
        command.clear();
        std::cerr << "新的进程：" << pid << std::endl;
        continue;
      } else {
        command.push_back(temp);
      }
    }
    if (command.empty() || (command.size() == 1 && command[0] == "|"))
      continue;
    if (command.front() == "|") {   
      if (std_in != STDIN_FILENO) {
        dup2(std_in, STDIN_FILENO);
        close(std_in);
        std_in = STDIN_FILENO;      
      }        
      command.pop_front();
      std_in = dup(STDIN_FILENO);
      dup2(pipedes[0], STDIN_FILENO);
    }    
    SingleCommand(command);
    command.clear();
    if (pipedes[0]) {
      close(pipedes[0]);
      pipedes[0] = 0;
    }
  }
}