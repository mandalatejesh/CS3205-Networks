import sys
import random
import math
import time
import matplotlib.pyplot as plt


sz = len(sys.argv)
i = 1
ki = 1                    # initial congestion window
km = 1                    # multiplier of CW during exponential phase
kn = 1                    # multiplier of CW during linear phase
f = 0.1                   # multiplier when timeout occurs
ps = 0.1                  # probab of receiving ack packet for a given segment before its timeout
segment_count = 100       # total number of segments to send
rws = 1000 # KB
mss = 1        # max segment size in KB
threshold = rws / 2 
outfile = 'results.txt'


acked = False
random.seed(44)
image = "plot"

# set parameters
while i < sz:
    if sys.argv[i].startswith("-"):
        if(sys.argv[i] in ('-i')):
            ki = float(sys.argv[i+1])
        elif (sys.argv[i] in ('-m')):
            km = float(sys.argv[i+1])
        elif (sys.argv[i] in ('-n')):
            kn = float(sys.argv[i+1])
        elif (sys.argv[i] in ('-f')):
            f = float(sys.argv[i+1])
        elif (sys.argv[i] in ('-s')):
            ps = float(sys.argv[i+1])
        elif (sys.argv[i] in ('-T')):
            segment_count = int(sys.argv[i+1])
        elif (sys.argv[i] in ('-o')):
            outfile = sys.argv[i+1]
        elif (sys.argv[i] in ('-a')):
            image = sys.argv[i+1]
        else:
            print("incorrect input format")
            sys.exit(0)
        i += 2

congestion_window = ki * mss
out = open(outfile,'w')
y = []
x = []
itr = 0
num_sent = segment_count

while(segment_count > 0):
    for j in range(math.ceil(congestion_window/mss)):
        acked = True if random.random() < ps else False
        if acked:
            if congestion_window < threshold:
                congestion_window = min(congestion_window + km * mss, rws, threshold)
            else:
                congestion_window = min(congestion_window + kn *( mss * mss) / congestion_window, rws)
            segment_count -= 1
        else:
            threshold = congestion_window / 2
            congestion_window = max(1, f * congestion_window)
        out.write(str(congestion_window) + str("\n"))
        y.append(congestion_window)
        itr += 1
        x.append(itr)
        if not acked:
            break

plt.title(f'ki={ki} km={km} kn={kn} kf={f} Ps={ps} T={num_sent}')
plt.plot(x,y, c='blue')
plt.grid()
plt.xlabel("update number")
plt.ylabel("congestion window size")
plt.savefig(f'{image}.png')
plt.close()

out.close()