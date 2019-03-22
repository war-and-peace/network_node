#include <stddef.h>
#include <string.h>

typedef struct _string{
  size_t _size;
  size_t _capacity;
  size_t (*size)();
  char (*at)();
  struct _string* (*append)();
  struct _string* (*insert)();
  char *_begin, *_end;
  char* data;
} string;

string init_string();
string init_string_s(string* str);
string init_string_p();
string init_string_sp(string* str);
