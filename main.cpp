#define Phoenix_No_WPI // remove WPI dependencies

#include "ctre/Phoenix.h"
#include "ctre/phoenix/cci/Unmanaged_CCI.h"
#include "ctre/phoenix/platform/Platform.h"
#include "ctre/phoenix/unmanaged/Unmanaged.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>
#include <string>
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

int main() {
    drive(0.0);

    while (true) {
        drive(0.1);

        /* loop yield for a bit */
        sleepApp(20);
    }

    SDL_Quit();
    return 0;
}
