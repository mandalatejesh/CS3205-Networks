/* reference: https://github.com/iputils/iputils/blob/master/ping.c */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>

using namespace std;

int datalen = 56;
int MAXIPLEN = 60;
int MAXICMPLEN = 76;

int main(int argc, char** argv) {
    int socketfd;
    struct sockaddr_in dst;
    struct timespec start_time, end_time;

    memset((char*)&dst, 0, sizeof(dst));
    dst.sin_family = AF_INET;

    if (inet_aton(argv[1], &dst.sin_addr) == 0) {
        cout << "Bad hostname\n";
        exit(1);
    }
    dst.sin_port = htons(1025);

    // Create a socket
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP)) == -1) {
        cout << "socket creation failed\n";
        exit(1);
    }


    // set timout
    struct timeval tv;
    tv.tv_sec = 10; // timeout of 10 seconds
    tv.tv_usec = 0;
    setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));


    unsigned char* packet;
    struct icmphdr* icp;
    int ntransmitted = 0;

    int packlen = datalen + MAXIPLEN + MAXICMPLEN;
    if (!(packet = (unsigned char*)malloc((unsigned int)packlen))) {
        cout << "cannot allocate space\n";
        exit(1);
    }
    icp = (struct icmphdr*)packet;
    /* send ICMP_ECHO packet */
    icp->type = ICMP_ECHO;
    icp->code = 0;
    icp->checksum = 0;
    icp->un.echo.sequence = htons(ntransmitted + 1);

    int sz = datalen + 8;

    /* send the ICMP packet*/
    clock_gettime(CLOCK_MONOTONIC, &start_time); // record time stamp
    int i = sendto(socketfd, icp, sz, 0, (struct sockaddr*)&dst, sizeof(dst));

    struct msghdr msg;
    char addrbuf[128];
    struct iovec iov;

    iov.iov_base = (char*)packet;
    iov.iov_len = packlen;

    memset(&msg, 0, sizeof(msg));

    msg.msg_name = addrbuf;
    msg.msg_namelen = sizeof(addrbuf);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    sz = recvmsg(socketfd, &msg, MSG_WAITALL);
    if (sz < 0) {
        cout << "Request timed out or host unreachable\n";
        exit(1);
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    if (sz < 0) {
        perror("Error in recvmsg");
        exit(1);
    }
    unsigned char* buf = (unsigned char*)msg.msg_iov->iov_base;
    struct icmphdr* icp_reply = (struct icmphdr*)buf;

    struct sockaddr_in* from = (struct sockaddr_in*)msg.msg_name;
    char src_addr[50];

    // get the address from which reply came
    getnameinfo((const sockaddr*)from, sizeof(*from), src_addr, sizeof(src_addr), NULL, 0, NI_NUMERICHOST);

    if (icp_reply->type == ICMP_ECHOREPLY) {
        double rtt = (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;
        rtt += (end_time.tv_sec - start_time.tv_sec) * 1000.0;
        printf("Reply from %s RTT = %.3lf ms\n", src_addr, rtt);
    } else {
        cout << "[Error ]Received non ICMP_ECHOREPLY packet\n";
    }
    return 0;
}