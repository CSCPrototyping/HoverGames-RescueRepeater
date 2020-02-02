#setup tethering internet to beaglebone
sudo iptables --append FORWARD --in-interface <USB interface with 192.168.7.1 IP> -j ACCEPT
sudo iptables --table nat --append POSTROUTING --out-interface <network interface> -j MASQUERADE
