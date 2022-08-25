#include "./headers/player.h"
#include <map>
#include <string.h>
#include <unistd.h>


Player::Player(int desc) {
    fd = desc;
    opponent_id = -1;
}

bool Player::sendMsg(char* msg)
{
    int cnt = write(fd, msg, strlen(msg));
    if (strlen(msg) > 0 && cnt <= 0)
    {
        if (opponent_id != -1)
        {
            write(opponent_fd, "exit1\n", 6);
        }
        return false;
    }
    return true;
}// returns whether the write was successful

int Player::readMsg(char* msg)
{
    memset(msg, 0, MAX_SIZE);
    int rd = read(fd, msg, MAX_SIZE);
    if (rd <= 0 && opponent_id != -1)
    {
        if (rd == 0) {
            write(opponent_fd, "exit1\n", 6);
            return READ_FAIL;
        }
        if (rd < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return READ_TIMEOUT;
        }
    }
    return READ_SUCCESS;
}// -1 : timeout
// 0 : broken pipe
// 2 : success