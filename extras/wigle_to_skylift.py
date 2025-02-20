#!/usr/bin/env python3

# usage: wigle_to_skylift.py IN_FILE OUT_DIR TARGET_LAT TARGET_LONG [NETWORKS_PER_OUTFILE]
# Converts the provided WiGLE response JSON file to the Skylift format, creating a directory
# of Skylift-format JSON files.
# TARGET_LAT and TARGET_LONG are the coordinates of the target location.
# NETWORKS_PER_OUTFILE defines the maximum number of networks per each generated output file.
# One file will be used for each ESP32 development board you have. The default is 20.
# Uses the same venv as the `skylift` cli.

import sys
import json
import datetime
import geopy.distance
import os
import shutil

def eprint(*args, **kwargs): return print(*args, **kwargs, file=sys.stderr)

# taken from archive/skylift/app/utils/geo_utils.py
def get_geo_distance(p1, p2):
    """Returns distance (meters) between to lat/lon points"""
    d = geopy.distance.geodesic(p1, p2).m  # .m for meters 
    return -d if (p2[0] < p1[0] or p2[1] > p1[1]) else d

def calc_geo_rssi(p1, p2):
    """Estimates RSSI based on geo distance between 2 lat/lon points"""
    m = geopy.distance.geodesic(p1, p2).m
    if m > 1000:
        rssi = -90
    elif m > 500:
        rssi = -80
    elif m > 250:
        rssi = -75
    elif m > 125:
        rssi = -65
    elif m > 50:
        rssi = -55
    else:
        rssi = -50
    return rssi

def main():
    if len(sys.argv) < 5 or len(sys.argv) > 6:
        eprint("usage: wigle_to_skylift.py IN_FILE OUT_DIR TARGET_LAT TARGET_LONG [NETWORKS_PER_OUTFILE]")
        sys.exit(1)
    
    # open input file

    fn_in = sys.argv[1]
    out_dir = sys.argv[2]
    # fn_out = sys.argv[2]
    target_lat = float(sys.argv[3])
    target_long = float(sys.argv[4])
    nets_per_of = 20
    try:
        nets_per_of = int(sys.argv[5])
    except IndexError:
        pass

    f = open(fn_in, "r")
    data = json.load(f)
    f.close()

    # create meta block
    
    sl_meta = {}

    sl_meta["comment"] = "" # unused
    sl_meta["filepath"] = "" # unused
    sl_meta["lat"] = target_lat
    sl_meta["lon"] = target_long
    sl_meta["radius"] = 1
    sl_meta["run"] = 1
    sl_meta["since"] = int(datetime.date.today().strftime("%Y%m%d"))
    sl_meta["type"] = "wigle"

    # convert each network to wigle format

    sl_networks = []
    for network in data["results"]:
        sl_network = {}
        
        sl_network["bssid"] = network["netid"]
        sl_network["channel"] = network["channel"]
        
        net_lat = network["trilat"]
        net_long = network["trilong"]
        p_network = (net_lat, net_long)
        p_target = (target_lat, target_long)
        sl_network["distance_x"] = get_geo_distance(p_target, (target_lat, net_long))
        sl_network["distance_xy"] = get_geo_distance(p_target, p_network)
        sl_network["distance_y"] = get_geo_distance(p_target, (net_lat, target_long))

        sl_network["lat"] = net_lat
        sl_network["lon"] = net_long
        sl_network["qos"] = network["qos"]

        sl_network["rssi"] = calc_geo_rssi(p_network, p_target)
        
        sl_network["ssid"] = network["ssid"]

        sl_networks.append(sl_network)
    
    # ensure the networks are sorted by distance
    sl_networks.sort(key=lambda sl_network: abs(sl_network["distance_xy"]))

    # write output files

    # ensure empty out dir
    try:
        shutil.rmtree(out_dir)
    except FileNotFoundError:
        pass
    os.makedirs(out_dir, exist_ok=True)

    n_nets = len(sl_networks)

    i = 0
    c = 0 # number of networks processed so far
    while c < n_nets:
        # if no networks left to do
        if n_nets - c == 0:
            return

        if n_nets - c < nets_per_of:
            nets_to_do = n_nets - c
        else:
            nets_to_do = nets_per_of
        
        sl_data = {}
        sl_data["meta"] = sl_meta
        sl_data["networks"] = sl_networks[c:c+nets_to_do]

        fn_out = out_dir + os.sep + f"skylift_{i+1}.json"
        f = open(fn_out, "w")
        json.dump(sl_data, f, indent=2)
        f.close()

        c += nets_to_do
        i += 1

if __name__ == "__main__":
    main()
