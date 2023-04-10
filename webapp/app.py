from flask import Flask, render_template, request
from flask_cors import CORS
import socket

CMD_FIELD_LEN = 1  # 1 byte commands sent from the client.
CONNECT_TIMEOUT = 10

SERVER_PORT = 50001
SERVER_ADDRESS = "0.0.0.0"
ADDRESS_PORT = (SERVER_ADDRESS, SERVER_PORT)

CMD = {
    "burst": 1,
    "burst++": 2,
    "uphill_burst": 3,
    "uphill_burst++": 4
}

def connect_to_server():
    # Create an IPv4 TCP socket.
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    client_socket.settimeout(CONNECT_TIMEOUT)

    # Connect to the server using its socket address tuple.
    client_socket.connect(ADDRESS_PORT)
    print(f"Connected to {SERVER_ADDRESS} on port {SERVER_PORT}\n")

app = Flask(__name__)
CORS(app)

@app.route('/')
def home():
    return render_template('home.html')


@app.route('/button-clicked', methods=['POST'])
def buttonClicked():
    button = request.json['button']
    if button == 'button1':
        # execute function for Burst button
        
        # Create the packet cmd field.
        cmd_field = CMD["burst"].to_bytes(CMD_FIELD_LEN, byteorder='big')
        # Send the request packet to the server.
        client_socket.sendall(cmd_field)

        print('Burst button pressed')
    elif button == 'button2':
        # execute function for Button 2

        # Create the packet cmd field.
        cmd_field = CMD["burst++"].to_bytes(CMD_FIELD_LEN, byteorder='big')
        # Send the request packet to the server.
        client_socket.sendall(cmd_field)

        print('Burst++ pressed')
    elif button == 'button3':
        # execute function for Button 3
        
        # Create the packet cmd field.
        cmd_field = CMD["uphill_burst"].to_bytes(CMD_FIELD_LEN, byteorder='big')
        # Send the request packet to the server.
        client_socket.sendall(cmd_field)

        print('Uphill Burst pressed')
    elif button == 'button4':
        # execute function for Button 4

        # Create the packet cmd field.
        cmd_field = CMD["uphill_burst++"].to_bytes(CMD_FIELD_LEN, byteorder='big')
        # Send the request packet to the server.
        client_socket.sendall(cmd_field)

        print('Uphill Burst++ pressed')
    return 'Button clicked: ' + button

if __name__ == '__main__':
    connect_to_server()

    app.run(debug=True, host='0.0.0.0', port=8089)
