import socket
import sys

HOST = "127.0.0.1"
PORT = 5555

data = "aadasdsada"

file = open("server.c", "r")
file_content = file.read()
file.close();

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.connect((HOST, PORT)) # Connect
    
    sock.sendall(bytes("{}\n".format(sys.argv[1]), "utf-8")) # Send data
    received = str(sock.recv(1024), "utf-8") # Receive data synchronically
    print(received)
    