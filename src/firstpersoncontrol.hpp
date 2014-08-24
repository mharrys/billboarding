#ifndef FIRSTPERSONCONTROL_HPP_INCLUDED
#define FIRSTPERSONCONTROL_HPP_INCLUDED

#include "clock.hpp"

class Input;
class WorldObject;

class FirstPersonControl {
public:
    FirstPersonControl();
    void update(seconds dt, const Input & input, WorldObject & object);

    float window_width;
    float window_height;

    float rotation_speed;
    float movement_speed;
private:
    void rotate(seconds dt, const Input & input, WorldObject & object);
    void move(seconds dt, const Input & input, WorldObject & object);

    float yaw_angle;
    float pitch_angle;
};

#endif
