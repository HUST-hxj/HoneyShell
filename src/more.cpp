#include <iostream>
#include <fstream>
#include <deque>
#include <string>

#include <stdio.h>
#include <curses.h>
#include <string.h>

int main(int argc, char **argv) {
  auto window = initscr();
  cbreak();
  std::fstream fs;
  fs.open(argv[1], std::ios_base::in);
  if (!fs.is_open()) {
    endwin();
    std::cerr << "错误：文件不存在。" << std::endl;
    return 0;
  }
  std::deque<std::string> line; 
  while (!fs.eof()) {
    char temp[202106];
    memset(temp, 0, sizeof(temp));
    fs.getline(temp, 202106);
    line.push_back(temp);
  }
  int cur, begin = 0;
  for (cur = begin; cur != line.size(); ++cur) 
    if (printw("%s\n", line[cur].c_str()))
      break;
  fs.close();
  refresh();
  bool eof = 0;
  while (1) {
    char ch = getchar();  
    if (ch == 'q') {
      endwin();
      return 0;
    } else if (ch == 'j') {
      if (cur < line.size() - 1)
        ++begin;
    } else if (ch == 'k') {
      if (begin)
        --begin;
    }
    clear();
    for (cur = begin; cur != line.size(); ++cur)
      if (printw("%s\n", line[cur].c_str()))
        break; 
    refresh();
  }
}