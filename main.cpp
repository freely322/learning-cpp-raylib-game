#include <cmath>
//#include <iostream>
#include "raylib.h"
#include <tuple>
#include <cstdlib>
#include <random>

char *formattedString(const int value) {
    const auto buffer = static_cast<char *>(malloc(100));
    if (!buffer) return nullptr;

    std::snprintf(buffer, 100, "Progress: %i", value);

    return buffer;
}

float getRandomFloat(const float min, const float max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(min, max);
    return static_cast<float>(dist(gen));
}

typedef Vector2 Coordinates;
typedef Vector2 Velocity2d;
typedef Vector2 Dimensions;
typedef std::tuple<Coordinates, float> CircleHitbox;
typedef std::tuple<Coordinates, Coordinates, Coordinates, Coordinates> Hitbox;

void apply2dVelocity(Coordinates &target, const Velocity2d &velocity, const float dt) {
    target.x += velocity.x * dt;
    target.y += velocity.y * dt;
}

float applyDeltaTime(const float val, const float dt) {
    return val * dt;
}

void applyXVelocity(Coordinates &target, const float velocity1d, const float dt) {
    target.x += applyDeltaTime(velocity1d, dt);
}

void applyYVelocity(Coordinates &target, const float velocity1d, const float dt) {
    target.y += applyDeltaTime(velocity1d, dt);
}

void invertXVelocity(Velocity2d &velocity) {
    velocity.x = -velocity.x;
}

void invertYVelocity(Velocity2d &velocity) {
    velocity.y = -velocity.y;
}

float getYCoordinate(const Coordinates &coordinates) {
    return coordinates.y;
}

float getXCoordinate(const Coordinates &coordinates) {
    return coordinates.x;
}

Hitbox getRectangleHitBox(const Coordinates &coordinates, const Dimensions &dimensions) {
    Coordinates origin;
    origin.x = coordinates.x;
    origin.y = coordinates.y;

    Coordinates origin2;
    origin2.x = origin.x + dimensions.x;
    origin2.y = origin.y + dimensions.y;

    Coordinates origin3;
    origin3.x = origin.x;
    origin3.y = origin.y + dimensions.y;

    Coordinates origin4;
    origin4.x = origin2.x;
    origin4.y = origin3.y;

    return {
        std::tuple(
            origin,
            origin2,
            origin3,
            origin4
        )
    };
}

CircleHitbox getCircleHitBox(const Coordinates &coordinates, const float &radius) {
    CircleHitbox hitbox = std::tuple(coordinates, radius);
    return hitbox;
}

float getXDimension(const Dimensions &dimensions) {
    return dimensions.x;
}

float getYDimension(const Dimensions &dimensions) {
    return dimensions.y;
}

bool isPointInsideCircle(const Coordinates &point, const CircleHitbox &circle) {
    const Coordinates circleCenter = std::get<0>(circle);
    const float radius = std::get<1>(circle);

    const float dx = getXCoordinate(point) - getXCoordinate(circleCenter);
    const float dy = getYCoordinate(point) - getYCoordinate(circleCenter);

    return (dx * dx + dy * dy) <= (radius * radius);
}

bool isRectangleInsideCircle(const Hitbox &rectangle, const CircleHitbox &circle) {
    return isPointInsideCircle(std::get<0>(rectangle), circle) &&
           isPointInsideCircle(std::get<1>(rectangle), circle) &&
           isPointInsideCircle(std::get<2>(rectangle), circle) &&
           isPointInsideCircle(std::get<3>(rectangle), circle);
}

bool isCircleInsideRectangle(const CircleHitbox &circle, const Hitbox &rectangle) {
    const Coordinates circleCenter = std::get<0>(circle);
    const float radius = std::get<1>(circle);

    const Coordinates rectTopLeft = std::get<0>(rectangle);
    const Coordinates rectBottomRight = std::get<3>(rectangle);

    const float circleLeft = getXCoordinate(circleCenter) - radius;
    const float circleRight = getXCoordinate(circleCenter) + radius;
    const float circleTop = getYCoordinate(circleCenter) - radius;
    const float circleBottom = getYCoordinate(circleCenter) + radius;

    return circleLeft >= getXCoordinate(rectTopLeft) &&
           circleRight <= getXCoordinate(rectBottomRight) &&
           circleTop >= getYCoordinate(rectTopLeft) &&
           circleBottom <= getYCoordinate(rectBottomRight);
}

bool isCircleRectanglePartialCollision(const CircleHitbox &circle, const Hitbox &rectangle) {
    const Coordinates circleCenter = std::get<0>(circle);
    const float radius = std::get<1>(circle);

    const Coordinates rectTopLeft = std::get<0>(rectangle);
    const Coordinates rectBottomRight = std::get<3>(rectangle);

    const float circleY = getYCoordinate(circleCenter);
    const float circleX = getXCoordinate(circleCenter);
    const float rectX1 = getXCoordinate(rectTopLeft);
    const float rectY1 = getYCoordinate(rectTopLeft);
    const float rectX2 = getXCoordinate(rectBottomRight);
    const float rectY2 = getYCoordinate(rectBottomRight);

    const float closestX = std::max(rectX1, std::min(circleX, rectX2));
    const float closestY = std::max(rectY1, std::min(circleY, rectY2));

    const float distanceX = circleX - closestX;
    const float distanceY = circleY - closestY;

    return (distanceX * distanceX + distanceY * distanceY) <= (radius * radius);
}

int checkFullCollision(const CircleHitbox &circleHitbox, const Hitbox &rectangleHitBox) {
    if (isRectangleInsideCircle(rectangleHitBox, circleHitbox)) {
        return 2; // Rectangle is fully inside the circle
    }
    if (isCircleInsideRectangle(circleHitbox, rectangleHitBox)) {
        return 2; // Circle is fully inside the rectangle
    }
    if (isCircleRectanglePartialCollision(circleHitbox, rectangleHitBox)) {
        return 1; // Partial collision between circle and rectangle
    }
    return 0; // No collision
}

void calculateProgress(int &progress, const int &collisionState) {
    if (collisionState == 2) {
        progress = progress + 3;
    }
    if (collisionState == 1) {
        progress = progress - 1;
    }
    if (collisionState == 0) {
        progress = progress - 3;
    }
}

int main() {
    constexpr float screenHeight = 500.0f;
    constexpr float screenWidth = 900.0f;
    // ReSharper disable once CppUseStructuredBinding
    constexpr auto rectangleDimensions = Vector2(80, 120);

    InitWindow(screenWidth, screenHeight, "Dasher");
    SetTargetFPS(60);
    int frame = 0;
    float animationTimer = 0.0f;

    // NEBULA VARIABLES
    //auto [nebula_x_velocity, nebula_y_velocity] = Vector2(0, 0);
    // ReSharper disable once CppUseStructuredBinding
    const Texture2D nebula{LoadTexture("C:/Users/chorn/CLionProjects/cpp-2/textures/12_nebula_spritesheet.png")};

    Rectangle nebulaRec{
        static_cast<float>(nebula.width * frame),
        0,
        static_cast<float>(nebula.height) / 8.0f,
        static_cast<float>(nebula.height)
    };

    Vector2 nebulaPos{0, screenHeight - nebulaRec.height};

    // THE GUY VARIABLES
    // auto rectangle_coordinates = Vector2(50, screenHeight - rectangleDimensions.y);
    Vector2 theGuyVelocity{0,0};
    auto [theGuy_x_velocity, theGuy_y_velocity] = theGuyVelocity;
    // ReSharper disable once CppUseStructuredBinding
    const auto theGuy = LoadTexture("C:/Users/chorn/CLionProjects/cpp-2/textures/scarfy.png");
    Rectangle theGuyRec{
        static_cast<float>(theGuy.width * frame),
        0,
        static_cast<float>(theGuy.width) / 6.0f,
        static_cast<float>(theGuy.height)
    };
    auto theGuyPos = Vector2(0, screenHeight - theGuyRec.height);

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();
        constexpr float speed = 600.0f;
        BeginDrawing();
        ClearBackground(BLACK);

        /*
        DrawRectangle(
            static_cast<int>(theGuyPos.x),
            static_cast<int>(theGuyPos.y),
            rectangleDimensions.x,
            rectangleDimensions.y,
            GREEN
        );
        */
        theGuyRec.x = theGuyRec.width * static_cast<float>(frame);
        const auto isInAir = theGuyPos.y < screenHeight - rectangleDimensions.y;

        if (isInAir) {
            const float gravity = applyDeltaTime(speed * 15, dt);
            theGuy_y_velocity = theGuy_y_velocity + gravity;
        } else {
            theGuyPos.y = screenHeight - rectangleDimensions.y;
            theGuy_y_velocity = 0;
        }
        animationTimer += dt;

        if (constexpr auto animationFPS = speed / 37; animationTimer > 1.0f / static_cast<float>(animationFPS)) {
            frame++;
            animationTimer = 0.0f;
        }
        if (frame > 5) {
            frame = 0;
        }

        if (isInAir) {
            frame = 5;
        }

        if (theGuy_x_velocity == 0 && !isInAir) {
            frame = 2;
        }

        DrawTextureRec(theGuy, theGuyRec, theGuyPos, WHITE);

        if (IsKeyPressed(KEY_SPACE) && !isInAir) {
            constexpr auto jumpForce = speed * 3.2;
            theGuy_y_velocity -= jumpForce;
        }

        if (!isInAir) {
            if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
                theGuy_x_velocity = speed;
            }
            if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
                theGuy_x_velocity = -speed;
            }
            if (IsKeyDown(KEY_D) && IsKeyDown(KEY_A)) {
                theGuy_x_velocity = 0;
            }
            if (IsKeyUp(KEY_A) && IsKeyUp(KEY_D)) {
                theGuy_x_velocity = 0;
            }
        }

        if (isInAir) {
            constexpr float flightControlFactor = 0.1f;
            if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
                if (theGuy_x_velocity < speed) {
                    theGuy_x_velocity = theGuy_x_velocity + speed * flightControlFactor;
                }
            }
            if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
                if (theGuy_x_velocity > -speed) {
                    theGuy_x_velocity = theGuy_x_velocity - speed * flightControlFactor;
                }
            }
            if (
                (IsKeyUp(KEY_A) && IsKeyUp(KEY_A))
                || (IsKeyDown(KEY_D) && IsKeyDown(KEY_A))
            ) {
                constexpr float deltaVelocity = 0.0001f;
                if (theGuy_x_velocity < 0) {
                    theGuy_x_velocity = theGuy_x_velocity + deltaVelocity;
                }
                if (theGuy_x_velocity > 0) {
                    theGuy_x_velocity = theGuy_x_velocity - deltaVelocity;
                }
            }
        }

        applyYVelocity(theGuyPos, theGuy_y_velocity, dt);
        applyXVelocity(theGuyPos, theGuy_x_velocity, dt);

        EndDrawing();
    }
    UnloadTexture(theGuy);
    UnloadTexture(nebula);
    CloseWindow();
    return 0;
}
