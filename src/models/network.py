#############################################################################
#
# SkyLift
# MIT License
# Copyright (c) 2016-2022 Adam Harvey
# https://github.com/adamhrv/skylift
#
#############################################################################

from typing import Dict, List, Optional
from pydantic import BaseModel, Field

class Network(BaseModel):
    bssid: str
    channel: int
    rssi: int
    lat: float = Field(0.0, alias='latitude')  # accept both lat and latitude
    lon: float = Field(0.0, alias='longitude')  # accept both lon and longitude
    ssid: Optional[str] = ''  # Make ssid optional with default empty string
    distance_x: Optional[float] = None
    distance_xy: Optional[float] = None
    distance_y: Optional[float] = None
    qos: Optional[int] = None

    def bssid_as_hex_list_ino(self):
        hex_str = ', '.join([f'0x{x}' for x in self.bssid.split(':')])
        return '{' + hex_str + '}'

    def channel_as_2pt4(self):
        return max(0, min(self.channel, 11))

    class Config:
        allow_population_by_field_name = True


class Meta(BaseModel):
    comment: Optional[str] = None
    filepath: Optional[str] = None
    lat: Optional[float] = None
    lon: Optional[float] = None
    radius: Optional[float] = None
    run: Optional[int] = None
    since: Optional[int] = None
    type: Optional[str] = None


class Networks(BaseModel):
    meta: Optional[Meta] = None
    networks: List[Network]
    filename: str = ''  # Make filename optional with default empty string
    n_wifi: int = 0
    n_bt: int = 0
    wifi: List[Network] = []  # Keep this for compatibility
    bt: List[Network] = []    # Keep this for compatibility

    def model_post_init(self):
        # Set filename from meta if not provided
        if not self.filename and self.meta and self.meta.filepath:
            self.filename = self.meta.filepath.split('/')[-1]

        # Move networks to wifi list if they're in the networks field
        if self.networks and not self.wifi:
            self.wifi = self.networks

        # Sort networks by RSSI
        self.wifi = sorted(self.wifi, key=lambda x: (x.rssi), reverse=True)
        self.bt = sorted(self.bt, key=lambda x: (x.rssi), reverse=True)

        # Update counts
        self.n_wifi = len(self.wifi)
        self.n_bt = len(self.bt)

    def get_networks(self, min_rssi=None, max_rssi=None, max_networks=None, 
                    device_type='wifi'):
        if device_type == 'wifi':
            nets = self.wifi or self.networks  # Try wifi first, fall back to networks
        elif device_type == 'bt':
            nets = self.bt

        if not nets:
            return []

        rssis = [x.rssi for x in nets]
        min_rssi = min_rssi if min_rssi else min(rssis)
        max_rssi = max_rssi if max_rssi else max(rssis)
        max_networks = max_networks if max_networks else len(nets)

        filtered_nets = [x for x in nets if x.rssi >= min_rssi and x.rssi <= max_rssi]
        return filtered_nets[:max_networks]

    class Config:
        allow_population_by_field_name = True