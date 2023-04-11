#define Phoenix_No_WPI // remove WPI dependencies

#include "ctre/Phoenix.h"
#include "ctre/phoenix/cci/Unmanaged_CCI.h"
#include "ctre/phoenix/platform/Platform.h"
#include "ctre/phoenix/unmanaged/Unmanaged.h"
#include <SDL2/SDL.h>
#include <chrono>
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

// Function to create a TCP listen socket
int create_listen_socket(int port) {
    // Create a TCP listen socket
    int listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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

    return listen_socket;
}

// Function to read one byte from a socket and return it as an int
int read_byte(int socket) {
    // Buffer to store the byte read from the socket
    unsigned char buffer[1];

    // Read one byte from the socket
    ssize_t bytes_read = read(socket, buffer, sizeof(buffer));

    // If read was successful, return the byte as an int
    if (bytes_read == sizeof(buffer)) {
        return (int) buffer[0];
    }

    // If read failed or returned less than one byte, return -1 to indicate an error
    return -1;
}

int main() {
    drive(0.0);

    create_listen_socket(50001);

    while (true) {
        drive(0.1);

        // loop yield for a bit
        sleepApp(20);
    }

    SDL_Quit();
    return 0;
}
