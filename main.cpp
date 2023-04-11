#define Phoenix_No_WPI // remove WPI dependencies

#include "ctre/Phoenix.h"
#include "ctre/phoenix/cci/Unmanaged_CCI.h"
#include "ctre/phoenix/platform/Platform.h"
#include "ctre/phoenix/unmanaged/Unmanaged.h"
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

void drive(double fwd) {
    cimMotor.Set(ControlMode::PercentOutput, fwd);
    ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100);
}

/** simple wrapper for code cleanup */
void sleepApp(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int listen_socket;

// Function to create a TCP listen socket
void create_listen_socket(int port) {
    // Create a TCP listen socket
    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Set the SO_REUSEADDR option to reuse the address
    int reuse_addr = 1;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

    // Bind the socket to the address and port
    struct sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    bind(listen_socket, (struct sockaddr *) &address, sizeof(address));

    // Listen for incoming connections
    listen(listen_socket, SOMAXCONN);
}

int accept_client() {
    struct sockaddr_in client_addr {};
    socklen_t client_addr_len = sizeof(client_addr);
    int client_socket = accept(listen_socket, (struct sockaddr *) &client_addr, &client_addr_len);

    if (client_socket < 0) {
        // handle error
        return -1;
    }

    // set socket to non-blocking
    int flags = fcntl(client_socket, F_GETFL, 0);
    if (flags == -1) {
        // handle error
        return -1;
    }
    if (fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        // handle error
        return -1;
    }

    return 1;
}

// Function to read a single byte from a non-blocking socket
int read_byte() {
    char buffer[1];
    int bytes_read = read(listen_socket, buffer, 1);
    if (bytes_read == -1) {
        // Error reading from socket
        return -1;
    } else if (bytes_read == 0) {
        // No data available
        return 0;
    } else {
        // Return the byte as an integer
        return (int) buffer[0];
    }
}

int main() {
    drive(0.0);

    create_listen_socket(50001);

    int client_conn = -1;

    while (client_conn < 0) {
        // accept client connection
        accept_client();

        // wait a bit before trying again
        usleep(1000);
    }

    while (true) {
        int byte = read_byte();
        if (byte == -1) {
            // Error reading from socket
            break;
        } else if (byte == 0) {
            // No data available, continue loop
            continue;
        } else {
            // Data available, do something with the byte
            // ...
            printf("byte received is %d", byte);
        }
    }


    // while (true) {
    //     drive(0.1);

    //     // loop yield for a bit
    //     sleepApp(20);
    // }

    return 0;
}
