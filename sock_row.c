#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sock_row.h"

int init_data(data_t* dat, char* ip) {
	if(ip) {
		dat->ip = strdup(ip);
		//dat->ip = malloc(strlen(ip) + 1);
		//strcpy(dat->ip, ip);
	}
	else {
		dat->ip = NULL;
	}
    
    dat->qhead = NULL;
    dat->qtail = NULL;
    dat->qlength = 0;
}

info_t* init_info(int package, char* id) {
    info_t* inf = (info_t*)malloc(sizeof(info_t));
    inf->package_size = package;
	if(id) {
    	inf->id = strndup(id, 16);
	}
	else {
		inf->id = NULL;
	}
    return inf;
}

int add_data(data_t* dat, info_t* inf) {
    if(dat->qlength == 0) {
        dat->qhead = inf;
        dat->qtail = inf;
    }
    
    else {
        dat->qtail->next = inf;
        dat->qtail = dat->qtail->next;
    }
    
    dat->qlength++;
    return 0;
}

int add_to_tail(data_t* dat, info_t* inf) {
    if(dat->qlength == 0) {
        dat->qhead = inf;
        dat->qtail = inf;
    }
    
    else {
	info_t* temp = dat->qhead;
        dat->qhead = inf;
        dat->qhead->next = temp;
    }
    
    dat->qlength++;
    return 0;
}

info_t* pop_data(data_t* dat) {
    if(dat->qlength == 0) {
        return NULL;
    }
    
    info_t* inf = dat->qhead;
    if(dat->qlength == 1) {
        dat->qhead = NULL;
        dat->qtail = NULL;
    }
    
    else {
        dat->qhead = dat->qhead->next;
    }
    
    dat->qlength--;
    return inf;
}

void destroy_data(data_t* dat) {
    info_t* inf;
	free(dat->ip);
    while(dat->qhead) {
		//printf("destroy_data dat = %p\n", dat->qhead);
        inf = pop_data(dat);
		if(inf->id) {
			free(inf->id);
		}
		free(inf);
    }
	printf("nana\n");
}

void print_data(data_t* dat) {
    info_t* inf = dat->qhead;
    while(inf) {
        printf("%d->", inf->package_size);
        inf = inf->next;
    }
    printf("null\n");
}

void add_data_to_data(data_t* dat_to, data_t* dat_from) {
	if(!dat_to->qhead) {
		dat_to->qhead = dat_from->qhead;
		dat_to->qtail = dat_from->qtail;
	}

	else {
		dat_to->qtail->next = dat_from->qhead;
		dat_to->qtail = dat_from->qtail;
	}

	dat_to->qlength += dat_from->qlength;
}

