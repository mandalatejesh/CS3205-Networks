import os
import json
from matplotlib import pyplot as plt

# get distances of servers from iitm
ip_list = open('./serverInfo/distances_json.txt','r')
string = ip_list.read()
servers_list = json.loads(string) 

# sort the server ips with respect to the distances
sorted_servers_list = {k: v for k, v in sorted(
    servers_list.items(), key=lambda item: item[1])}

rtt_vals = {} # stores RTT values as a dictionary

# ping each server 10 times
for i in sorted_servers_list.keys():
    rtt_vals[i]=[]
    for j in range(10):
        ping_data = os.popen(f'ping -c 1 {i}').read()
        after_split= ping_data.split('time=')
        try:
            after_split= after_split[1].split('ms')
            rtt_vals[i].append(float(after_split[0]))
        except:
            print(f'ping failed: {i}, retrying...')

# if some pings failed in the previous loop, 
# try until 10 successful pings are obtained            
for i in rtt_vals.keys():
    if len(rtt_vals[i])>0 and len(rtt_vals[i])<10:
        while(len(rtt_vals[i])<10):
            ping_data = os.popen(f'ping -c 1 {i}').read()
            after_split = ping_data.split('time=')
            try:
                after_split = after_split[1].split('ms')
                rtt_vals[i].append(float(after_split[0]))
            except:
                print(f'ping failed: {i}, retrying...')


# Save the RTT values as a json object in a file, to use later            
f = open('./serverInfo/rtt.txt','w')
f.write(json.dumps(rtt_vals))
f.close()

## Direct loading
g = open('./serverInfo/rtt.txt','r')
string = g.read()
rtt_vals = json.loads(string)
g.close()

## Saving to log file
log = open('./log/ping.log','w')
pos = open('./serverInfo/positions.txt','r')
string1 = pos.read()
positions = json.loads(string1)
log.write('<MY_LOC>, <SERVER_IP>, <SERVER_LOC>, <RTT1, RTT2, ... RTT10>')
for i in rtt_vals.keys():
    to_write = '{lat: 12.99, lon: 80.23},\t'
    to_write = to_write + f'{i},\t' + f'{positions[i]},\t<'
    for j in range(10):
        if j>len(rtt_vals[i])-1:
            to_write = to_write + ' -, '
        else:
            to_write = to_write + f' {rtt_vals[i][j]}, '
    to_write = to_write + '>\n'
    log.write(to_write)
    
    
pos.close()
log.close()

distances = []
avg_rtt_vals = []
for i in rtt_vals.keys():
    cnt = sorted_servers_list[i]
    x = [cnt for i in range(len(rtt_vals[i]))]
    avg = 0
    for j in rtt_vals[i]:
        avg = avg + j
    if len(rtt_vals[i])>0:
        avg = avg/len(rtt_vals[i])
    avg_rtt_vals.append(avg)
    distances.append(cnt)
    plt.scatter(x, rtt_vals[i], marker='o')
# plt.grid()
plt.xlabel(r'distance (km) $\rightarrow$')
plt.ylabel(r'RTT (ms) $\rightarrow$')
plt.savefig('./plots/rtt_plot.png')
# plt.show()

for i in range(len(avg_rtt_vals)):
    avg_rtt_vals[i] = avg_rtt_vals[i]*150/distances[i]
plt.title('plot of RTT/min possible time vs distance')
plt.scatter(distances, avg_rtt_vals)
plt.xlabel(r'distance (km) $\rightarrow$')
plt.ylabel(r'RTT observed / min RTT possible$\rightarrow$')
plt.grid()
plt.savefig('./plots/RTT_compare.png')
# plt.show()

ip_list.close()
