#include "my_string.h"
#include <stdio.h>
#include <ctype.h>

size_t size(string s){
  return *(s._size);
}

char at(string s, size_t pos){
  if(pos >= 0 && pos <= size(s))
    return s.data[pos];
  else
    return '\0';
}

size_t new_size(char *begin, char *end){
  return (unsigned int)(end - begin);
}

string init_string(){
  string t;
  t.data = (char*)malloc(sizeof(char) * 1u);
  t._begin = &t.data[0];
  t._end = &t.data[0];
  t._size = (size_t*)malloc(sizeof(size_t));
  t._capacity = (size_t*)malloc(sizeof(size_t));
  *(t._size) = new_size(t._begin, t._end);
  *(t._capacity) = 1;
  return t;
}

string init_string_s(string s){
  string t;
  t._size = (size_t*)malloc(sizeof(size_t));
  t._capacity = (size_t*)malloc(sizeof(size_t));
  *(t._size) = *(s._size);
  *(t._capacity) = *(s._size) + 1;
  t.data = (char*) malloc(sizeof(char) * *(t._capacity));
  for(size_t i = 0;i < *(s._size);i ++){
    t.data[i] = s.data[i];
  }
  t._begin = &t.data[0];
  t._end = &t.data[*(t._capacity) - 1];
  return t;
}

string init_string_c(const char* str){
  if(str == NULL){
    return init_string();
  }
  string t = init_string();
  size_t len = strlen(str);
  free(t.data);
  t.data = (char*)malloc(sizeof(char) * (len + 1));
  for(size_t i = 0;i < len; i++){
    t.data[i] = str[i];
  }
  t._begin = &t.data[0];
  t._end = &t.data[len];
  *(t._capacity) = len + 1;
  *(t._size) = len;
  return t;
}

void sprint(string s){
  for(char *i = s._begin;i < s._end;i ++){
    fprintf(stdout, "%c", *i);
  }
}

int is_valid_pos(size_t* size, size_t pos){
  if(pos >= 0 && pos < *size)return 1;else return 0;
}

string insert(string s, string d, size_t s_pos){
  if(!is_valid_pos(s._size, s_pos))return s;
  if(*(s._size) + *(d._size) < *(s._capacity)){
      size_t len_d = size(d);
      size_t len_s = size(s);
      size_t end_pos = len_d + len_s - 1;
      for(size_t i = 0;i < len_d;i ++){
        s.data[end_pos - i] = s.data[s_pos + len_d - i - 1];
      }
      for(size_t i = 0;i < len_d;i ++){
        s.data[s_pos + i] = d.data[i];
      }
      *(s._size) = len_d + len_s;
      (s._end) = &s.data[end_pos + 1];
      return s;
  }else{
    //free(s.data);
    size_t len_d = size(d);
    size_t len_s = size(s);
    char *temp = (char*)malloc(sizeof(char) * (len_d + len_s + 1));
    size_t current = 0;
    for(size_t i = 0;i < s_pos;i ++){
      temp[current] = s.data[i]; current ++;
    }
    for(size_t i = 0;i < len_d;i ++){
      temp[current] = d.data[i]; current ++;
    }
    for(size_t i = s_pos;i < len_s;i ++){
      temp[current] = s.data[i]; current ++;
    }
    for(size_t i = 0;i < len_s + len_d;i ++){
      printf("%c", temp[i]);
    }
    printf("\n");
    char *temp2 = s.data;
    s.data = temp;
    free(temp2);
    s._begin = &s.data[0];
    s._end = &s.data[current];
    *(s._size) = len_d + len_s;
    *(s._capacity) = len_d + len_s + 1;
    return s;
  }
}

void sprintln(string s){
  sprint(s);printf("\n");
}

string append(string s, string d){
  size_t len_s = size(s);
  size_t len_d = size(d);
  *(s._size) = len_s + len_d;
  *(s._capacity) = len_s + len_d + 1;
  char* temp = (char*)malloc(sizeof(char) * (len_s + len_d + 1));
  size_t current = 0;
  for(size_t i = 0;i < len_s;i ++){
    temp[current ++] = s.data[i];
  }
  for(size_t i = 0;i < len_d;i ++){
    temp[current ++] = d.data[i];
  }
  char* temp2 = s.data;
  s.data = temp;
  free(temp2);
  s._begin = &s.data[0];
  s._end = &s.data[len_s + len_d];
  return s;
}

string erase(string s, size_t start, size_t n){
  if(!is_valid_pos(s._size, start)){
    return s;
  }
  if(n <= 0){
    return s;
  }
  size_t len = size(s);
  size_t l = start;
  size_t r = start + n - 1;
  if(r < len){
    for(size_t i = r + 1;i < len;i ++){
      s.data[l++] = s.data[i];
    }
    s._end = &s.data[l];
    *(s._size) = l;
  }else{
    *(s._size) = l;
    s._end = &s.data[l];
  }
  return s;
}

string sread(){
  string t;
  size_t n = INITIAL_BUFFER_SIZE, len = 0;
  char* temp = (char*)malloc(INITIAL_BUFFER_SIZE * sizeof(char));
  int c = getc(stdin);
  while(!isspace(c) && c != EOF){
    if(len < n - 1){
      temp[len ++] = c;
    }else{
      char* temp2 = temp;
      n *= 2;
      temp = (char*)malloc(n * sizeof(char));
      for(size_t i = 0;i < len;i ++){
        temp[i] = temp2[i];
      }
      temp[len ++] = c;
      free(temp2);
    }
    c = getc(stdin);
  }
  t.data = temp;
  t._size = (size_t*)malloc(sizeof(size_t) * 1);
  t._capacity = (size_t*)malloc(sizeof(size_t));
  *(t._capacity) = n;
  *(t._size) = len;
  t._begin = &temp[0];
  t._end = &temp[len];
  return t;
}

string clear(string s){
  free(s._begin);
  free(s._capacity);
  free(s._end);
  free(s._size);
  free(s.data);
  return init_string();
}

string append_c(string s, const char* str){
  return append(s, init_string_c(str));
}

size_t find(string s, string d){
  size_t slen = size(s);
  size_t dlen = size(d);
  for(size_t i = 0;i < slen - dlen + 1;i ++){
    int same = 1;
    for(size_t k = 0;k < dlen;k ++){
      if(s.data[i + k] != d.data[k]){
        same = 0;
        break;
      }
    }
    if(same){
      return i;
    }
  }
  return -1;
}

size_t find_c(string s, const char* str){
  return find(s, init_string(str));
}

string insert_c(string s, const char* str, size_t s_pos){
  return insert(s, init_string(str), s_pos);
}