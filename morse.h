#ifndef MORSE_H
#define MORSE_H

#include "sock_row.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"

typedef struct language{
    int dot;
    int line;
    int end_l;
    int end_w;
    int end_s;
	int end_connection;
	int* intmap;
	char* bitmap;
} lang;

lang* init_lang(void);
lang* define_language(int dot, int line, int end_w, int end_s, int end_l);
int* init_intmap(void);
char* init_bitmap(void);
void print_msg(data_t* data, lang* l);
char get_char(int size[], char* bitmap, int len);
void to_morse(char* msg, lang* l, char* ip, char* id);
char* morse_workshop(lang* l, char c, data_t* data, int* intmap, char* id);
void send_pings(data_t* dat);
void destroy_lang(lang* l);
void end_of_connection(char* ip, lang* l);

#endif

