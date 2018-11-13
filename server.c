#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <string.h>
#include "threadpool.h"
#include "sock_row.h"
#include "morse.h"

// size 16 - 23 not work

char* get_ack(char* buf);
int check_ping(long send_pack, char* buf, data_t* data);
int get_ping(void* null);

int main(int argc, char* argv[]){
    threadpool* pool = create_threadpool(1);
    dispatch(pool, get_ping, argv[1]);
    char buf[1024];
    lang* l = init_lang();
    char* id = (char*)malloc(sizeof(char) * 17);
    sprintf(id, "%016lld", (long long)0);
    while(1) {
        fgets(buf, sizeof(buf), stdin);
		if(strcmp(buf, "exit\n") == 0) {
			//puts("end_of_connection(argv[1], l);");
			end_of_connection(argv[1], l);
			break;
		}
        to_morse(buf, l, argv[1], id);
    }
	free(id);
	destroy_lang(l);
	free(l);
    destroy_threadpool(pool);
    return 0;
}

char* get_ack(char* buf) {
    int i;
    char* id = (char*)malloc(sizeof(char) * 17);
    for (i = 0; i < 8; i++) {
        sprintf(id + 2*i,"%02X", (uint8_t)buf[i]);
    }
    id[16] = 0;
    return id;
}

int get_ping(void* client_ip) {
    char* ip = (char*)client_ip;
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0){
        perror("sock:");
        exit(1);
    }
    struct sockaddr_in cliaddr;
    char buf[284]; // for 256 package size
    socklen_t clilen = sizeof(struct sockaddr_in);
    data_t* dat = (data_t*)malloc(sizeof(data_t));
    init_data(dat, ip);
    lang* l = init_lang();
	int status;
    while(1){
        long n=recvfrom(sockfd, buf, 284, 0, (struct sockaddr *)&cliaddr, &clilen);
        if(n == 284) {
			status = check_ping(n, buf, dat);
		    if(status >= 0) {
				//puts("end_of_connection(ip, l);");
				end_of_connection(ip, l);
				break;
		    }
        }
        if(n < 0) {
            perror("recvfrom");
            return -1;
        }
        status = check_ping(n, buf, dat);
        if(status == l->end_s) {
			print_msg(dat, l);
            init_data(dat, ip);
        }
    }

	destroy_lang(l);
	free(l);
    free(dat);
    return 0;
}

int check_ping(long send_pack, char* buf, data_t* dat) {
    struct iphdr *ip_hdr = (struct iphdr *)buf;
    struct icmphdr *icmp_hdr = (struct icmphdr *)((char *)ip_hdr + (4 * ip_hdr->ihl));
    if(icmp_hdr->type == 8) {
        struct sockaddr_in source, dest;
        memset(&source, 0, sizeof(source));
        source.sin_addr.s_addr = ip_hdr->saddr;
        
        memset(&dest, 0, sizeof(dest));
        dest.sin_addr.s_addr = ip_hdr->daddr;
        
        char* source_ip = strdup(inet_ntoa(source.sin_addr));
        if(strcmp(source_ip, dat->ip) == 0) { // check if it is the same client
            int pack = send_pack - ip_hdr->ihl*4 - 8;
			char* ack = get_ack(buf + 44);
            add_data(dat, init_info(pack, ack));
			free(ack);
			free(source_ip);
            return pack; // all good
        }

        free(source_ip);
        return -2; // unknowen client
    }

    return -1; // not a resive ping
}


