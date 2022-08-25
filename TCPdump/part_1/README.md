# Part 1: 
## (a) script to estimate the distance of the servers from my location
- latitude and longitude corresponding to iitm is (12.99, 80.23).
- run `python3 find_distance.py` to compute estimate the distances of servers present in `ping-servers.txt`
    - distance estimates of the servers are stored  in `distances.txt`
    - coordinates of the servers are stored as a json object in `./serverInfo/positions.txt`

## (b) Ping each server 10 times and record RTT values

- run `python3 ping.py` to ping each server present in `ping-servers.txt` 10 times.
- a log file named `./log/ping.log` is generated in the given format.

## (\c) Scatter plot and analysis
- running `python3 ping.py` in the previous part also generate a scatter plot for RTTs and distance values, the plot is stored as `./plots/rtt_plot.png`
- It can be observed that a the best result is 2.5X slower compared to the case when the packets travel at the speed of light, the ratio is plotted as a function of distance in `./plots/RTT_compare.png`

## (d ) traceroute
- `traceroute 139.130.4.5` was run on terminal and the following results were obtained
- It took 20 hops to reach the destination server, the ip addresses of the routers are stored in `./traceroute/traceroute.txt`
- Locations of these ip addresses are stored in `./traceroute/ip-location_mapping.txt`
- IP address of router that forwards packet to foreign router is `115.247.85.129` in Maharashtra, please note that the next router in the trace is `172.31.3.25` whose location is unknown, Hence I'm considering `115.247.85.129` is forwarding the packet to foreign router.
- The packet visits `103.198.140.170` whose location is in Singapore.