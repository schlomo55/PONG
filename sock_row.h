#ifndef SOCK_ROW_H
#define SOCK_ROW_H

typedef struct data_info {
    int package_size;
    char* id;
    struct data_info* next;
} info_t;

typedef struct inet_data {
    info_t* qhead;
    info_t* qtail;
    int qlength;
    char* ip;
} data_t;

int init_data(data_t* dat, char* ip);
info_t* init_info(int package, char* id);
int add_data(data_t* dat, info_t* inf);
int add_to_tail(data_t* dat, info_t* inf);
void add_data_to_data(data_t* dat_to, data_t* dat_from);
info_t* pop_data(data_t* dat);
void print_data(data_t* dat);
void destroy_data(data_t* dat);

#endif

