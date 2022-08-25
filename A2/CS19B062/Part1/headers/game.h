#ifndef TIC_TAC_TOE_H
#define TIC_TAC_TOE_H
#include <time.h>

#define ABORTED -2
bool chk(int);

class GameBoard
{
public:
    bool finished;
    bool set_x(int, int);
    bool set_o(int, int);
    bool is_game_over();
    void print_board();
    void get_board(char *);
    void quit_game(bool);
    int get_winner();
    GameBoard(int, int);
    int gameId;
    time_t start_time;
    time_t end_time;

private:
    char board[3][3];
    int rem;
    int fd1;
    int fd2;
    int winner; // 0 -> none, 1-> X, 2-> O
};

#endif