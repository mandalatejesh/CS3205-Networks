#include "./headers/game.h"
#include <stdio.h>
#include <cstring>
#include <time.h>

long guid = 1;

GameBoard::GameBoard(int player1, int player2)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board[i][j] = '_';
        }
    }
    rem = 9;
    gameId = guid++;
    fd1 = player1;
    fd2 = player2;
    winner = -1;
    start_time = time(NULL);
}

bool chk(int num)
{
    return (num >= 0 && num <= 2);
}

bool GameBoard::set_x(int x, int y)
{
    x -= 1;
    y -= 1;
    if (!(chk(x) && chk(y) && board[x][y] == '_'))
    {
        return false;
    }
    board[x][y] = 'X';
    rem--;
    if (rem == 0)
    {
        quit_game(false);
    }
    return true;
}

bool GameBoard::set_o(int x, int y)
{
    x -= 1;
    y -= 1;
    if (!(chk(x) && chk(y) && board[x][y] == '_'))
    {
        return false;
    }
    board[x][y] = 'O';
    rem--;
    if (rem == 0)
    {
        quit_game(false);
    }
    return true;
}
void GameBoard::print_board()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

bool GameBoard::is_game_over()
{
    for (int i = 0; i < 3; i++)
    {
        // rows
        if (board[i][0] != '_' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
        {
            if (board[i][0] == 'X')
                winner = fd1;
            else
                winner = fd2;
            return true;
        }
        // cols
        if (board[0][i] != '_' && board[0][i] == board[1][i] && board[1][i] == board[2][i])
        {
            if (board[0][i] == 'X')
                winner = fd1;
            else
                winner = fd2;
            return true;
        }
    }
    // diagonals
    // d1
    if (board[0][0] != '_' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
    {
        if (board[1][1] == 'X')
            winner = fd1;
        else
            winner = fd2;
        return true;
    }
    // d2
    if (board[2][0] != '_' && board[2][0] == board[1][1] && board[1][1] == board[0][2])
    {
        if (board[1][1] == 'X')
            winner = fd1;
        else
            winner = fd2;
        return true;
    }

    return this->rem <= 0;
}

void GameBoard::get_board(char *s)
{
    memset(s, 0, 20);
    int cnt = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            s[cnt++] = ' ';
            if (board[i][j] == '_')
            {
                s[cnt++] = board[i][j];
                s[cnt++] = '_';
                if (j == 1)
                {
                    s[cnt++] = '_';
                }
            }
            else
            {
                s[cnt++] = ' ';
                s[cnt++] = board[i][j];
                if (j == 1)
                {
                    s[cnt++] = ' ';
                }
            }
            s[cnt++] = ' ';
            if (j != 2)
            {
                s[cnt++] = '|';
            }
        }
        s[cnt++] = '\n';
    }
    s[cnt++] = '\0';
}

int GameBoard::get_winner()
{
    return winner;
}

void GameBoard::quit_game(bool aborted)
{
    rem = 0;
    if (aborted)
        winner = ABORTED;
    end_time = time(NULL);
}
