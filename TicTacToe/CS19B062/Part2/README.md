# A simple ping program in C++

run `g++ yapp.c -o yapp` to generate the executable

ping any valid ip using `./yapp X.X.X.X`

An ICMP_ECHO packet is sent to the destination and a response is awaited for 10 seconds.
RTT time is recorded.
