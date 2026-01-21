import socket
import time
from random import randint

PORT = 5150
# YOUR TARGET SUBNET (based on your earlier message)
WIFI_PREFIX = "192.168.86." 

def get_wifi_ip():
    # This gets all IPs assigned to your computer
    hostname = socket.gethostname()
    ip_list = socket.gethostbyname_ex(hostname)[2]
    
    for ip in ip_list:
        if ip.startswith(WIFI_PREFIX):
            return ip
    return None

def start_udp_broadcaster():
    wifi_ip = get_wifi_ip()
    
    if not wifi_ip:
        print(f"FAILED: Could not find an interface starting with {WIFI_PREFIX}")
        print("Is your WiFi on and connected to the same router as the ESP?")
        return

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    # THE MAGIC LINE: Bypasses the VPN by binding to the physical WiFi IP
    sock.bind((wifi_ip, 0))

    print(f"--- MIDINET VPN BYPASS MODE ---")
    print(f"Forcing traffic through WiFi IP: {wifi_ip}")
    print(f"Broadcasting to 255.255.255.255:{PORT}")

    try:
        count = 0
        while True:
            message = str(
                {
                    "ch": randint(0, 16),
                    "type": "note on",
                    "value": randint(0, 127)
                }
			)
            data = message.encode('utf-8')
            # Sending to the subnet broadcast is more reliable than 255.255.255.255 on VPNs
            sock.sendto(data, ("192.168.86.255", PORT))
            print(message)
            count += 1
            time.sleep(1)
    except KeyboardInterrupt:
        sock.close()

if __name__ == "__main__":
    start_udp_broadcaster()