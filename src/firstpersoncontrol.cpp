#include "firstpersoncontrol.hpp"

#include "input.hpp"
#include "worldobject.hpp"

FirstPersonControl::FirstPersonControl()
    : window_width(1.0f),
      window_height(1.0f),
      rotation_speed(3.4f),
      movement_speed(100.0f),
      yaw_angle(0.0f),
      pitch_angle(0.0f)
{
}

void FirstPersonControl::update(seconds dt, const Input & input, WorldObject & object)
{
    rotate(dt, input, object);
    move(dt, input, object);
}

void FirstPersonControl::rotate(seconds dt, const Input & input, WorldObject & object)
{
    // we use the aspect ratio to keep a more uniform sensitivy across
    // different resolutions
    const float aspect = window_width / window_height;

    const glm::vec2 mouse_movement(
        -input.position_rel().x / aspect,
        -input.position_rel().y / aspect
    );

    const float speed = rotation_speed * dt.count();

    // -90 <= pitch <= 90
    pitch_angle += mouse_movement.y * speed;
    pitch_angle = glm::clamp(pitch_angle, -90.0f, 90.0f);
    glm::quat pitch = glm::angleAxis(pitch_angle, X_UNIT);

    // -360 <= yaw <= 360
    yaw_angle += mouse_movement.x * speed;
    if (yaw_angle < -360.0f) {
        yaw_angle += 360.0f;
    } else if (yaw_angle > 360.0f) {
        yaw_angle -= 360.0f;
    }
    glm::quat yaw = glm::angleAxis(yaw_angle, Y_UNIT);

    object.orientation = glm::normalize(yaw * pitch);
}

void FirstPersonControl::move(seconds dt, const Input & input, WorldObject & object)
{
    const float speed = movement_speed * dt.count();

    glm::vec3 displacement(0.0f);

    if (input.down(Key::W) || input.down(Key::UP)) {
        displacement.z--;;
    }

    if (input.down(Key::S) || input.down(Key::DOWN)) {
        displacement.z++;
    }

    if (input.down(Key::A) || input.down(Key::LEFT)) {
        displacement.x--;
    }

    if (input.down(Key::D) || input.down(Key::RIGHT)) {
        displacement.x++;
    }

    if (input.down(Key::Q)) {
        displacement.y--;
    }

    if (input.down(Key::R)) {
        displacement.y++;
    }

    if (displacement != glm::vec3(0.0f)) {
        // limit translation to the xz-plane
        glm::quat yaw = object.orientation;
        yaw.x = 0.0f;
        yaw.z = 0.0f;
        yaw = glm::normalize(yaw);
        object.position += yaw * (glm::normalize(displacement) * speed);
    }
}
