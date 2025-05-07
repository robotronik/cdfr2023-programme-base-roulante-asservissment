#include "position.h"
#include "config.h"
#include "clock.h"
#include "odometry/OTOS.h"
#include "AsservissementMath.h" // for mod_angle

position_t pos = {0.0, 0.0, 0.0};
position_t vel = {0.0, 0.0, 0.0};
position_t acc = {0.0, 0.0, 0.0};
static position_t newPosition = {0.0, 0.0, 0.0};
static bool needChangePos = false;
static bool positionChanged = false;

void updatePositionData(){
    
    if (needChangePos) {
        pos.x = newPosition.x;
        pos.y = newPosition.y;
        pos.a = newPosition.a;
        needChangePos = false;
        positionChanged = true;
        otos.setPosition(pos);
    }
    // Get the current position from the OTOS
    position_t r_pos, r_vel, r_acc;
    if (otos.getPosVelAcc(r_pos, r_vel, r_acc) != ret_OK) {
        // Handle error if needed
    }
    // Update the position, velocity, and acceleration data
    pos = r_pos;
    vel = r_vel;
    acc = r_acc;
    // Update the time of the position data
    pos.time = get_uptime_ms();
    vel.time = pos.time;
    acc.time = pos.time;
}

void setPosition(position_t inCommingposition) {
    newPosition = inCommingposition;
    newPosition.a = mod_angle(inCommingposition.a);
    needChangePos = true;
}
void setPosition(double x, double y, double a) {
    newPosition.x = x;
    newPosition.y = y;
    newPosition.a = mod_angle(a);
    newPosition.time = get_uptime_ms();
    needChangePos = true;
}

bool getPositionChanged(){
    bool bret = positionChanged;
    positionChanged = false;
    return bret;
}