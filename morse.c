#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include "morse.h"

void printArray(char* arr, int len);

lang* define_language(int dot, int line, int end_w, int end_s, int end_l) {
    lang* new_lang = (lang*)malloc(sizeof(lang));
    new_lang->dot = dot;
    new_lang->line = line;
    new_lang->end_l = end_l;
    new_lang->end_w = end_w;
    new_lang->end_s = end_s;
    return new_lang;
}

lang* init_lang() {
    lang* l = (lang*)malloc(sizeof(lang));
    l->dot = 25;
    l->line = 35;
    l->end_l = 45;
    l->end_w = 55;
    l->end_s = 65;
	l->end_connection = 284;
	l->intmap = init_intmap();
	l->bitmap = init_bitmap();
    return l;
}



int* init_intmap() {
    int* intmap = (int*)malloc(sizeof(int)*36);
    intmap[0] = 142;
    intmap[1] = 264;
    intmap[2] = 328;
    intmap[3] = 268;
    intmap[4] = 15;
    intmap[5] = 72;
    intmap[6] = 396;
    intmap[7] = 8;
    intmap[8] = 14;
    intmap[9] = 232;
    intmap[10] = 332;
    intmap[11] = 136;
    intmap[12] = 398;
    intmap[13] = 270;
    intmap[14] = 460;
    intmap[15] = 200;
    intmap[16] = 424;
    intmap[17] = 140;
    intmap[18] = 12;
    intmap[19] = 271;
    intmap[20] = 76;
    intmap[21] = 40;
    intmap[22] = 204;
    intmap[23] = 296;
    intmap[24] = 360;
    intmap[25] = 392;
	intmap[26] = 496;
    intmap[27] = 240;
    intmap[28] = 112;
    intmap[29] = 48;
    intmap[30] = 16;
    intmap[31] = 0;
    intmap[32] = 256;
    intmap[33] = 384;
    intmap[34] = 448;
    intmap[35] = 480;
    
    return intmap;
}

char* init_bitmap() {
    char* bitmap = (char*)malloc(sizeof(char)*512);
    bitmap[142] = 'a';
    bitmap[264] = 'b';
    bitmap[328] = 'c';
    bitmap[268] = 'd';
    bitmap[15] = 'e';
    bitmap[72] = 'f';
    bitmap[396] = 'g';
    bitmap[8] = 'h';
    bitmap[14] = 'i';
    bitmap[232] = 'j';
    bitmap[332] = 'k';
    bitmap[136] = 'l';
    bitmap[398] = 'm';
    bitmap[270] = 'n';
    bitmap[460] = 'o';
    bitmap[200] = 'p';
    bitmap[424] = 'q';
    bitmap[140] = 'r';
    bitmap[12] = 's';
    bitmap[271] = 't';
    bitmap[76] = 'u';
    bitmap[40] = 'v';
    bitmap[204] = 'w';
    bitmap[296] = 'x';
    bitmap[360] = 'y';
    bitmap[392] = 'z';
    bitmap[240] = '1';
    bitmap[112] = '2';
    bitmap[48] = '3';
    bitmap[16] = '4';
    bitmap[0] = '5';
    bitmap[256] = '6';
    bitmap[384] = '7';
    bitmap[448] = '8';
    bitmap[480] = '9';
    bitmap[496] = '0';
    return bitmap;
}

void print_msg(data_t* data, lang* l) {
	//printf("the data is:");
	//print_data(data);
    int index = 0;
    int temp_size[5];
    char buff[data->qlength+2];
    info_t* info = data->qhead;
	int place = 0;
    //for(i = 0; i < data->qlength; i++){
	while(info) {
        if(info->package_size == l->end_l){
            int size[index];
            int j;
            for(j = 0; j< index; j++){
                size[j] = temp_size[j];
            }
			
            buff[place] = get_char(size, l->bitmap, index);
			index = 0;
			place++;
        }
        else if(info->package_size == l->end_w) {
            buff[place] = ' ';
			place++;
		}
        else if(info->package_size == l->end_s){
            buff[place] = '\n';
			buff[place + 1] = 0;
            printf("\n\nmesseng: %s", buff);
            return;
        }
        else{
            temp_size[index] = info->package_size == l->dot ? 0 : 1;
            index++;
        }
		info = info->next;
    }
	//printf("print_msg data = %p\n", data->qhead);
	destroy_data(data);
	//printf("print_msg destroy_data\n");
	
}

char get_char(int* size_c, char* bitmap, int len) {
    char c[9];
    int i = 0;
    for(i = 0; i < len; i++){
        if(size_c[i] == 0){
            c[i] = '0';
        }
        else{
            c[i] = '1';
        }
    }

    int comp = 5-len;
    for(; i < 9; i++){
        if(i<5){
            c[i] =  '0';
        }
        else {
            if(comp > 0){
                c[i] = '1';
                comp--;
            }
            else
                c[i] = '0';
        }
    }
    int total = 0;
	int num;
    for(i = 0; i < 9; i++){
		num = c[i] == '0' ? 0 : 1;
        total = total*2 + num;
    }
	
    return bitmap[total];
}

void to_morse(char* msg, lang* l, char* ip, char* id) {
    data_t* data = (data_t*)malloc(sizeof(data_t));
    int i;
    for(i = 0; i< strlen(msg); i++){
		init_data(data, ip);
        strcpy(id, morse_workshop(l, msg[i], data, l->intmap, id));
		send_pings(data);
    }
	free(data);
}


char* morse_workshop(lang* l, char c, data_t* data, int* intmap, char* id) {

    info_t* info;
    int i;
    int size = 0;
    int mapping = 0;
	long long temp_id;
    if(c >= 'a' && c<= 'z')
        mapping = intmap[c - 'a'];
    else if(c >= '0' && c <= '9')
        mapping = intmap[26 + c - '0'];
	else if(c == ' ') {
		
		temp_id = atoll(id) + l->end_w;
		sprintf(id, "%016lld", temp_id);
		add_data(data, init_info(l->end_w, id));
		return id;
	}
	else if(c == '\n') {
		
		temp_id = atoll(id) + l->end_s;
		sprintf(id, "%016lld", temp_id);
		add_data(data, init_info(l->end_s, id));
		return id;
	}
	else {
		return id;
	}
	data_t* from = (data_t*)malloc(sizeof(data_t));
	init_data(from, data->ip);
    for(i = 0; i<4; i++){
        if(CHECK_BIT(mapping, 0)){
            size++;
        }
        mapping >>= 1;
    }

    mapping >>= size;
    for(i = 0; i < 5 - size; i++){
        if(CHECK_BIT(mapping, 0)){
            temp_id = atoll(id) + l->line;
			sprintf(id, "%016lld", temp_id);
			info = init_info(l->line, id);
        }
        else{
            temp_id = atoll(id) + l->dot;
			sprintf(id, "%016lld", temp_id);
			info = init_info(l->dot, id);
        }
        add_to_tail(from, info);
		mapping >>= 1;
    }
    add_data_to_data(data, from);
	free(from->ip);
	free(from);
    temp_id = atoll(id) + l->end_l;
    sprintf(id, "%016lld", temp_id);
    add_data(data, init_info(l->end_l, id));
    return id;
}

void end_of_connection(char* ip, lang* l) {
	data_t* data = (data_t*)malloc(sizeof(data_t));
	init_data(data, ip);
	add_data(data, init_info(l->end_connection, "ffffffffffffffff"));
	send_pings(data);
}

void send_pings(data_t* dat) {
	info_t* info = dat->qhead;
	char file[128];
	strcpy(file,"sent_pings_");
	strcat(file, dat->ip);
	strcat(file, ".log");
	char timebuf[128];
	int fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 777);
	time_t now = time(NULL);
    strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&now));
	write(fd, timebuf, strlen(timebuf));	
	write(fd, "\n", 1);
	while(info) {
		char* ping = (char*)malloc(sizeof(char)*256);
		strcpy(ping, "ping -c 1 -s ");
		char size[22];
		sprintf(size, "%d", info->package_size);
		strcat(ping, size);
		strcat(ping, " -p ");
		strcat(ping, info->id);
		strcat(ping, " ");
		strcat(ping, dat->ip);
		write(fd, ping, strlen(ping));
		write(fd, "\n", 1);
		strcat(ping, " > /dev/null 2>&1");
		system(ping);
		info = info->next;
	}
	close(fd);
	//printf("send_pings dat = %p\n", dat->qhead);
	//destroy_data(dat);
	//printf("send_pings destroy_data\n");
}

void destroy_lang(lang* l) {
	free(l->intmap);
	free(l->bitmap);
}

void printArray(char* arr, int len) {
	int i;
	for(i = 0; i < len; i++) {
		printf("[%c]", arr[i]);
	}
	printf("\n");
}

