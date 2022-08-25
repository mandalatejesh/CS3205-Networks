#ifndef PLAYER_H
#define PLAYER_H
#define MAX_SIZE 100

#define READ_SUCCESS 1
#define READ_FAIL 2
#define READ_TIMEOUT 3

class Player
{
public:
    int self_id;
    int gid;
    int opponent_id;
    int opponent_fd;
    int fd;
    bool sendMsg(char *); // send msg to player
    int readMsg(char *); // receive msg from player
    Player(int);
};

#endif