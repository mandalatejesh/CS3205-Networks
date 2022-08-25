from math import radians, cos, sin, asin, sqrt
import requests
import json

ACCESS_KEY = '05d5242e328c20ef213b1513a79265ef'
positions = {}

def getLocation(ip):
    url = f"http://api.ipapi.com/api/{ip}?access_key={ACCESS_KEY}"
    payload = {}
    headers = {}

    response = requests.request("GET", url, headers=headers, data=payload)
    resp = json.loads(response.text)
    positions[str(ip)] = {'lat': resp['latitude'], 'lon': resp['longitude']}
    return resp['latitude'], resp['longitude']


"""
    Calculate the great circle distance in kilometers between two points 
    on the earth (specified in decimal degrees)
"""


def haversine(lon1, lat1, lon2, lat2):
    # convert decimal degrees to radians
    lon1, lat1, lon2, lat2 = map(radians, [lon1, lat1, lon2, lat2])
    # haversine formula
    dlon = lon2 - lon1
    dlat = lat2 - lat1
    a = sin(dlat/2)**2 + cos(lat1) * cos(lat2) * sin(dlon/2)**2
    c = 2 * asin(sqrt(a))
    r = 6371  # Radius of earth in kilometers.
    return c * r


servers_list = open('ping-servers.txt', 'r')
server_distances = {}
lat1 = 12.99
lon1 = 80.23
for line in servers_list:
    server = line.split()[0]
    lat2, lon2 = getLocation(server)
    server_distances[f'{server}'] = haversine(lon1, lat1, lon2, lat2)
f = open('./serverInfo/distances_json.txt', 'w')
p = open('./serverInfo/positions.txt','w')
g = open('distances.txt','w')
g.write('<SERVER_IP> <SERVER_DISTANCE>')
for i in server_distances:
    g.write(f'{i}: {server_distances[i]}\n')
g.close()
string = json.dumps(server_distances)
f.write(string)
# print(positions)
p.write(json.dumps(positions))
f.close()
p.close()
servers_list.close()
print('done')
