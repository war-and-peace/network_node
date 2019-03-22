#include "node.h"
#include <stdio.h>

int main(){
  string s = sread();
  string t = sread();
  sprintln(s);
  sprintln(t);
  t = insert(t, s, 0);
  //sprint(s);
  sprintln(t);
  t = append(t, s);
  sprintln(t);
  t = erase(t, 0, 20);
  sprintln(t);
  t = append_c(t, "new_string");
  sprintln(t);
  printf("Pos: %d ", find(t, init_string_c("str")));
  return 0;
}
