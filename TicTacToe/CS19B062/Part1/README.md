# TIC_TAC_TOE on a traditional Client server architecture

run `g++ -pthread server.cpp game.cpp player.cpp -o gameserver` to generate the server executable.
run `g++ client.cpp -o gameclient` to generate the client executable

- Start the server using the command `./gameserver`

  - Multiple simultaneous games are supported by this server.
  - If a player does not make their move within 15 seconds, game is terminated and players are asked whether they want to restart the game.
  - Server maintains a log file named `./logs/server_log.txt`.

- Start the client using the command `./gameclient`
  - player will be prompted to input coordinates to mark their turn.

The information logged in the log file is:

- Start time of a game
- Duration of a game
- Sequence of moves of each player in a game
- Server status
- Player connection status

References used:

- https://man7.org/linux/man-pages/dir_section_2.html
- https://www.geeksforgeeks.org/handling-multiple-clients-on-server-with-multithreading-using-socket-programming-in-c-cpp/
