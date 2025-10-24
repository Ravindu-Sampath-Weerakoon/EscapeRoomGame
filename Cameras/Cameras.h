#pragma once

// We get <glut.h> and <math.h> from our precompiled header
#include "pch.h" 

// Define M_PI if it's not already
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEG_TO_RAD (M_PI / 180.0)

class Camera {
public:
    // Constructor: Needs your window's dimensions to center the mouse
    Camera(int windowWidth, int windowHeight);

    // --- CORE FUNCTIONS ---

    /**
     * @brief Updates all movement and physics. Call this in your idle() function.
     * @param dt Delta-time (time since the last frame)
     */
    void update(float dt);

    /**
     * @brief Applies the camera's view. Call this in your display() function
     * INSTEAD of gluLookAt().
     */
    void applyView();


    /**
     * @brief Finishes camera setup. Call this in main() AFTER glutCreateWindow().
     */
    void init();


    // --- INPUT FUNCTIONS ---
    // You will call these from your main.cpp's glut callbacks

    void onKeyDown(unsigned char key);
    void onKeyUp(unsigned char key);
    void onSpecialKeyDown(int key);
    void onSpecialKeyUp(int key);

    /**
     * @brief Handles mouse look. Call this from glutPassiveMotionFunc.
     */
    void onMouseMovement(int x, int y);

    /**
     * @brief Updates internal window size when the window is resized.
     * Call this from your glutReshapeFunc.
     */
    void onWindowResize(int w, int h);


    // --- CONFIGURATION ---

    /**
     * @brief Returns true if the camera is in developer fly mode.
     */
    bool isDeveloperMode() const { return m_isDeveloperMode; }

    void setPosition(float x, float y, float z);
    void setGroundLevel(float level) { m_groundLevel = level; m_posY = level; }

private:
    // --- INTERNAL HELPERS ---
    void jump();
    void toggleMode();
    void recalculateVectors();
    void centerMouse();

    // --- CAMERA STATE ---
    bool  m_isDeveloperMode;
    float m_posX, m_posY, m_posZ; // Camera position
    float m_yaw;   // Degrees (rotation left/right)
    float m_pitch; // Degrees (rotation up/down)

    // --- CALCULATED VECTORS ---
    float m_forwardX, m_forwardY, m_forwardZ;
    float m_rightX, m_rightY, m_rightZ;

    // --- PHYSICS (GAME MODE) ---
    float m_groundLevel;
    float m_velocityY;   // For jumping
    bool  m_isJumping;
    float m_gravity;
    float m_jumpForce;

    // --- INPUT STATE (INTERNAL) ---
    // This tracks which keys are currently held down
    //
    // --- TYPO FIX IS HERE ---
    bool m_inputForward, m_inputBackward, m_inputStrafeLeft, m_inputStrafeRight;
    //
    bool m_inputLookLeft, m_inputLookRight, m_inputLookUp, m_inputLookDown;
    bool m_inputFlyUp, m_inputFlyDown; // Dev mode Q/E

    // --- SPEEDS & SETTINGS ---
    float m_moveSpeed;
    float m_devMoveSpeed;
    float m_lookSpeed;        // For arrow keys
    float m_mouseSensitivity;

    // --- MOUSE LOOK STATE ---
    int  m_windowWidth, m_windowHeight;
    int  m_windowCenterX, m_windowCenterY;
    bool m_firstMouse; // Prevents camera from jumping on first mouse move
};