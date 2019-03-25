#include "vector.h"
#include <stdio.h>

int can_add(size_t* size, size_t* c){
    if(*(size) + 1 < (*c))return 1;else return 0;
}

svector_t init_svector(){
    svector_t t;
    t._size = (size_t*)malloc(sizeof(size_t));
    t._capacity = (size_t*)malloc(sizeof(size_t));
    t.data = (string*)malloc(sizeof(string));
    *(t._size) = 0u;
    *(t._capacity) = 1;
    t._begin = &t.data[0];
    t._end = &t.data[0];
    return t;
}

svector_t svector_add(svector_t v, string s){
    if(can_add(v._size, v._capacity)){
        *(v._end) = s;
        v._end ++;
        (*(v._size)) ++;
        return v;
    }else{
        size_t n = *(v._capacity);
        n *= 2;
        string* temp = (string*)malloc(sizeof(string) * n);
        for(size_t i = 0;i < *(v._size);i ++){
            temp[i] = v.data[i];
        }
        string* temp2 = v.data;
        v.data = temp;
        free(temp2);
        v._begin = &v.data[0];
        v.data[*(v._size)] = s;
        (*(v._size)) ++;
        v._end = &(v.data[*(v._size)]);
        *(v._capacity) = n;
        return v;
    }
}

string sv_at(svector_t t, size_t pos){
    if(pos >= *(t._size)){
        return init_string_c("-1");
    }else{
        return t.data[pos];
    }
}

