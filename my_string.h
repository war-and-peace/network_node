#include <stddef.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>

#define INITIAL_BUFFER_SIZE 64

typedef struct _string{
  size_t* _size;
  size_t* _capacity;
  char *_begin, *_end;
  char* data;
} string;

string init_string(); //done
string init_string_s(string s); //done
// string init_string_s(string* str);
// string init_string_p();
// string init_string_sp(string* str);
string init_string_c(const char* str); //done
size_t size(string s); //done
char at(string s, size_t pos); //done
string insert(string s, string d, size_t s_pos);
string append(string s, string d);
string erase(string s, size_t start, size_t n);
void sprint(string s); //done
void sprintln(string s);
string sread();
string sreadln();
string clear(string s);
string append_c(string s, const char* str);
size_t find(string s, string d);
size_t find_c(string, const char* str);
string insert_c(string s, const char* str, size_t s_pos);