#include <sys/socket.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <queue>
#include <csignal>
#include <fstream>
#include <time.h>
#include <mutex>
#include "./headers/game.h"
#include "./headers/player.h"
#define MAX 80
#define PORT 8080
#define THREAD_POOL_SZ 30

using namespace std;

ofstream logger;
time_t timer = time(NULL);
long pid = 1;
Player waiting_player(-1);
int waiting_fd = -1;
int socketfd;

pthread_t pool[THREAD_POOL_SZ];
struct arguments
{
    Player p1;
    Player p2;
};
queue<struct arguments *> pending_work;
pthread_mutex_t work_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cfd_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t check = PTHREAD_COND_INITIALIZER;
pthread_mutex_t id_mutex = PTHREAD_MUTEX_INITIALIZER;

std::mutex log_mutex;
string current_time();
void log(string s)
{
    lock_guard<std::mutex> lock(log_mutex);
    logger << "[ " << current_time() << " ] " << s << endl;
}

bool replay_consent(Player p1, Player p2)
{
    char msg[MAX_SIZE];
    char buffer[MAX_SIZE];
    strcpy(msg, "replay\n");
    p1.sendMsg(msg);
    p2.sendMsg(msg);
    int val = p1.readMsg(buffer);
    printf("%s\n", buffer);
    if (strcmp(buffer, "yes") != 0 && strcmp(buffer, "no") != 0)
    {
        val = p1.readMsg(buffer);
        printf("%s\n", buffer);
    }
    bool p1_consent = false, p2_consent = false;
    if (val == READ_SUCCESS && strcmp(buffer, "yes") == 0)
    {
        p1_consent = true;
    }
    val = p2.readMsg(buffer);
    printf("%s\n", buffer);
    if (strcmp(buffer, "yes") != 0 && strcmp(buffer, "no") != 0)
    {
        val = p2.readMsg(buffer);
        printf("%s\n", buffer);
    }
    if (val == READ_SUCCESS && strcmp(buffer, "yes") == 0)
    {
        p2_consent = true;
    }
    if (!p1_consent || !p2_consent)
    {
        if (p1_consent)
        {
            char message[] = "Sorry, your partner quit the game\n";
            p1.sendMsg(message);
        }
        else if (p2_consent)
        {
            char message[] = "Sorry, your partner quit the game\n";
            p2.sendMsg(message);
        }
        return false;
    }
    return true;
}

void *handle_connection(void *args)
{
    Player p1 = ((struct arguments *)args)->p1;
    Player p2 = ((struct arguments *)args)->p2;
    free(args);

    char msg[100];

    p2.opponent_id = p1.self_id;
    p1.opponent_id = p2.self_id;
    p2.opponent_fd = p1.fd;
    p1.opponent_fd = p2.fd;
    bool first = true;
    while (true)
    {
        if (!first)
        {
            bool replay = replay_consent(p1, p2);
            if (!replay)
                break;
        }
        first = false;
        sprintf(msg, "Your partner's ID is %d. Your symbol is 'X'\n", p2.self_id);
        p1.sendMsg(msg);

        sprintf(msg, "Your partner's ID is %d. Your symbol is 'O'\n", p1.self_id);
        p2.sendMsg(msg);

        strcpy(msg, "Starting the game ...\n");
        p1.sendMsg(msg);
        p2.sendMsg(msg);
        GameBoard board = GameBoard(p1.fd, p2.fd);
        log("Player " + to_string(p1.self_id) + ", Player " + to_string(p2.self_id) + " started game with ID: " + to_string(board.gameId));
        log("[GameID: " + to_string(board.gameId) + "] Player " + to_string(p2.self_id) + " marker is 'X'");
        log("[GameID: " + to_string(board.gameId) + "] Player " + to_string(p1.self_id) + " marker is 'O'");
        bool turn_of_x = true;
        char buffer[MAX_SIZE];
        while (!board.is_game_over())
        {
            if (turn_of_x)
            {
                cout << "turn of x in game: " << board.gameId << "\n";
                int x, y;
                // check for illegal input
                bool break_loop = false;
                do
                {
                    // write(cfd1, "input", 5);
                    strcpy(msg, "input\n");
                    p1.sendMsg(msg);
                    strcpy(msg, "wait\n");
                    p2.sendMsg(msg);
                    memset(buffer, 0, sizeof(buffer));
                    // int rd = read(cfd1, buffer, sizeof(buffer));
                    int val = p1.readMsg(buffer);
                    if (val == READ_TIMEOUT)
                    {
                        board.quit_game(true);
                        break;
                    }
                    else if (val == READ_FAIL)
                    {
                        return NULL;
                    }
                    int v = atoi(buffer);
                    x = v / 10;
                    y = v % 10;
                    break_loop = board.set_x(x, y);
                } while (!break_loop);
                if (break_loop)
                {
                    log("[GameID: " + to_string(board.gameId) + " ] Player " + to_string(p2.self_id) + " marked (" + to_string(x) + ", " + to_string(y) + ")");
                }
            }
            else
            {
                // check for illegal input
                cout << "turn of o in game: " << board.gameId << "\n";
                int x, y;
                bool break_loop = false;
                do
                {
                    strcpy(msg, "input\n");
                    p2.sendMsg(msg);
                    strcpy(msg, "wait\n");
                    p1.sendMsg(msg);
                    memset(buffer, 0, sizeof(buffer));
                    int val = p2.readMsg(buffer);
                    if (val == READ_TIMEOUT)
                    {
                        board.quit_game(true);
                        break;
                    }
                    else if (val == READ_FAIL)
                    {
                        return NULL;
                    }
                    int v = atoi(buffer);
                    x = v / 10;
                    y = v % 10;
                    break_loop = board.set_o(x, y);
                } while (!break_loop);
                if (break_loop)
                {
                    log("[GameID: " + to_string(board.gameId) + " ] Player " + to_string(p1.self_id) + " marked (" + to_string(x) + ", " + to_string(y) + ")");
                }
            }
            char b[40];
            board.get_board(&b[0]);
            p1.sendMsg(b);
            p2.sendMsg(b);
            turn_of_x = !turn_of_x;
        }
        log("[GameID: " + to_string(board.gameId) + "] Game over");
        int wfd = board.get_winner();
        printf("winner is %d\n", wfd);
        if (wfd == -1)
        {
            char message[] = "It's a tie\n";
            log("[GameID: " + to_string(board.gameId) + "] tied.");

            p1.sendMsg(message);
            p2.sendMsg(message);
        }
        else if (wfd == ABORTED)
        {
            char message[] = "Game quit due to timeout\n";
            log("[GameID: " + to_string(board.gameId) + "] game terminated due to timeout");
            p1.sendMsg(message);
            p2.sendMsg(message);
        }
        else
        {
            int nbytes = write(wfd, "You Won\n", 8);
            int winner = wfd == p1.fd ? p1.self_id : p2.self_id;
            int loser = wfd == p1.fd ? p2.self_id : p1.self_id;

            log("[Game ID: " + to_string(board.gameId) + "] Player " + to_string(winner) + " won the game");
            log("[Game ID: " + to_string(board.gameId) + "] Player " + to_string(loser) + " lost the game");

            if (nbytes != 0)
                write(p1.fd + p2.fd - wfd, "You Lost\n", 9);
        }
        board.print_board();
    }
    strcpy(msg, "exit\n");
    log("connection closed to player " + to_string(p1.self_id));
    log("connection closed to player " + to_string(p2.self_id));
    printf("closing connection to player %d\n", p1.self_id);
    printf("closing connection to player %d\n", p2.self_id);
    p1.sendMsg(msg);
    p2.sendMsg(msg);
    return NULL;
}

void *thread_fn(void *arg)
{
    while (true)
    {
        struct arguments *clients = NULL;
        pthread_mutex_lock(&work_mutex);
        if (pending_work.empty())
        {
            pthread_cond_wait(&check, &work_mutex);
            clients = pending_work.front();
        }
        pending_work.pop();
        pthread_mutex_unlock(&work_mutex);
        if (clients != NULL)
        {
            handle_connection(clients);
        }
    }
}

Player welcome(int fd)
{
    Player p = Player(fd);
    pthread_mutex_lock(&id_mutex);
    p.self_id = pid++;
    pthread_mutex_unlock(&id_mutex);
    char msg[MAX_SIZE];
    sprintf(msg, "Connected to the game server. Your player id is %d\n", p.self_id);
    p.sendMsg(msg);
    return p;
}

void signal_handler(int num)
{
    cout << "\nStopping the server...\n";
    close(socketfd);
    log("Server stopped");
    logger.close();
    exit(num);
}

string current_time()
{
    timer = time(NULL);
    string s(ctime(&timer));
    return s.substr(0, 24);
}

int main()
{
    // open logger file
    logger.open("./logs/server_log.txt");
    // [ TIME ] [ GAME_ID ] %MESSAGE%

    // signal handler
    signal(SIGINT, signal_handler);
    signal(SIGABRT, signal_handler);

    // ignore sigpipe interrupts
    signal(SIGPIPE, SIG_IGN);

    // create threads to serve future connections
    for (int i = 0; i < THREAD_POOL_SZ; i++)
    {
        pthread_create(&pool[i], NULL, thread_fn, NULL);
    }

    // create a socket
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in my_addr, peer_addr_1, peer_addr_2;
    socklen_t peer_addr_size_1, peer_addr_size_2;
    if (socketfd == -1)
    {
        cout << "Err: socket not created\n";
        return 1;
    }
    // set timout
    struct timeval tv;
    tv.tv_sec = 15;
    tv.tv_usec = 0;
    setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));

    // bind()
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(PORT);
    if (bind(socketfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) != 0)
    {
        cout << "bind failed\n";
        return 0;
    }

    // wait for connection listen()
    if (listen(socketfd, 10) == -1)
    {
        cout << "listen failed\n";
        return 0;
    }
    cout << "Game server started. Waiting for players.\n";
    log("Game server started");
    // accept a connection accept()
    peer_addr_size_1 = sizeof(peer_addr_1);
    while (true)
    {
        int cfd = accept(socketfd, (struct sockaddr *)&peer_addr_1, &peer_addr_size_1);
        if (cfd == -1)
        {
            continue;
        }
        struct arguments *pclient = (struct arguments *)malloc(sizeof(struct arguments));
        Player p1 = welcome(cfd);
        log("Player connected, id = " + to_string(p1.self_id));
        pclient->p1 = p1;
        pthread_t t;
        pthread_mutex_lock(&cfd_mutex);
        bool wait = false;
        if (waiting_fd != -1)
        {
            pclient->p2 = waiting_player;
            pthread_mutex_lock(&work_mutex);
            pending_work.push(pclient);
            pthread_cond_signal(&check);
            pthread_mutex_unlock(&work_mutex);
            waiting_fd = -1;
            wait = false;
        }
        else
        {
            waiting_fd = cfd;
            waiting_player = p1;
            wait = true;
        }
        pthread_mutex_unlock(&cfd_mutex);
        if (wait)
        {
            char msg[] = "Waiting for a partner to join...\n";
            p1.sendMsg(msg);
        }
    }
    close(socketfd);
    return 1;
}