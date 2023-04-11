#define Phoenix_No_WPI // remove WPI dependencies
#define BUFFER_SIZE 1

#include "ctre/Phoenix.h"
#include "ctre/phoenix/cci/Unmanaged_CCI.h"
#include "ctre/phoenix/platform/Platform.h"
#include "ctre/phoenix/unmanaged/Unmanaged.h"
#include <arpa/inet.h>
#include <chrono>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace ctre::phoenix;
using namespace ctre::phoenix::platform;
using namespace ctre::phoenix::motorcontrol;
using namespace ctre::phoenix::motorcontrol::can;

TalonSRX cimMotor(1); // Use the specified interface

void configureMotor() {
    cimMotor.ConfigOpenloopRamp(2.0);
}

void drive(double fwd) {
    cimMotor.Set(ControlMode::PercentOutput, fwd);
    ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100);
}

/** simple wrapper for code cleanup */
void sleepApp(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int create_connect_socket() {
    int server_socket, client_socket;
    struct sockaddr_in server_address {};
    struct sockaddr_in client_address {};
    socklen_t client_address_size = sizeof(client_address);
    char buffer[BUFFER_SIZE];

    // create the server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // set up the server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(50001);

    // bind the socket to the server address
    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    // listen for incoming connections
    listen(server_socket, 5);

    printf("Listening on port %d\n", ntohs(server_address.sin_port));

    // accept the client connection
    client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_size);

    printf("Accepted connection from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    // set socket to non-blocking
    fcntl(client_socket, F_SETFL, O_NONBLOCK);

    return client_socket;
}

int receive_byte(int client_socket, char *buffer) {
    // receive a single byte of data
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, MSG_DONTWAIT);

    if (bytes_received < 0) {
        // handle error
        // perror("Error receiving data");
        return -1;
    } else if (bytes_received == 0) {
        // handle connection closed by remote peer
        // printf("Connection closed by remote peer\n");
        return -1;
    } else {
        // do something with the received data
        // printf("Received %d bytes of data: %d\n", bytes_received, buffer[0] - 49);
        return buffer[0] - 49;
    }
}


int main() {
    drive(0.0);

    int client_socket = create_connect_socket();
    char buffer[BUFFER_SIZE];

    while (true) {
        int bytes_received = receive_byte(client_socket, buffer);

        switch (bytes_received) {
            case 0:
                std::cout << "Burst" << std::endl;
                break;
            case 1:
                std::cout << "Burst++" << std::endl;
                break;
            case 2:
                std::cout << "Uphill" << std::endl;
                break;
            case 3:
                std::cout << "Uphill++" << std::endl;
                break;
            case 4:
                std::cout << "STOP" << std::endl;
                drive(0.0);
                break;
            default:
                std::cout << "default case" << std::endl;
                break;
        }

        sleepApp(20);
    }

    // close the sockets
    close(client_socket);

    return 0;
}
