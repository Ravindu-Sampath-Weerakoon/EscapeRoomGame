// Cameras.cpp : Defines the functions for the static library.

#include "pch.h" // Must be the first include
#include "Cameras.h" // Include your own header
#include <stdio.h> // For printf

Camera::Camera(int windowWidth, int windowHeight) {
    onWindowResize(windowWidth, windowHeight); // Set up window dimensions

    m_isDeveloperMode = false; // Start in Game Mode
    m_firstMouse = true;

    // Set initial position and orientation
    setPosition(0.0f, 0.0f, 5.0f);
    m_yaw = -90.0f; // Look forward (along -Z)
    m_pitch = 0.0f;

    // Clear all input states
    m_inputForward = m_inputBackward = m_inputStrafeLeft = m_inputStrafeRight = false;
    m_inputLookLeft = m_inputLookRight = m_inputLookUp = m_inputLookDown = false;
    m_inputFlyUp = m_inputFlyDown = false;

    // Set speeds
    m_moveSpeed = 3.0f;   // units per second
    m_devMoveSpeed = 8.0f; // Dev mode is faster
    m_lookSpeed = 90.0f;  // degrees per second
    m_mouseSensitivity = 0.1f;

    // Set physics
    m_groundLevel = 0.0f;
    m_velocityY = 0.0f;
    m_isJumping = false;
    m_gravity = -20.0f; // A good gameplay gravity value
    m_jumpForce = 8.0f;

    // === ADDED FOR SMOOTHING ===
    m_acceleration = 12.0f;  // Tune this value
    m_damping = 10.0f;       // Tune this value
    m_velX = 0.0f;
    m_velY = 0.0f;
    m_velZ = 0.0f;
    // ===========================

    // Calculate initial vectors
    recalculateVectors();

    // DO NOT call glutSetCursor here. GLUT is not initialized yet.
}

// This is the new function that gets called from main()
void Camera::init() {
    // Hide cursor for game mode
    // We do this here because glut has been initialized.
    if (!m_isDeveloperMode) {
        glutSetCursor(GLUT_CURSOR_NONE);
    }
}


void Camera::setPosition(float x, float y, float z) {
    m_posX = x;
    m_posY = y;
    m_posZ = z;
}

void Camera::onWindowResize(int w, int h) {
    m_windowWidth = w;
    m_windowHeight = (h == 0) ? 1 : h; // Prevent divide by zero
    m_windowCenterX = m_windowWidth / 2;
    m_windowCenterY = m_windowHeight / 2;
}

void Camera::recalculateVectors() {
    // Convert yaw and pitch from degrees to radians
    float yawRad = m_yaw * DEG_TO_RAD;
    float pitchRad = m_pitch * DEG_TO_RAD;

    // Calculate the new Forward vector
    m_forwardX = cos(pitchRad) * sin(yawRad);
    m_forwardY = sin(pitchRad);
    m_forwardZ = -cos(pitchRad) * cos(yawRad);

    // Calculate the Right vector (Cross product of Forward and World Up)
    // World Up is (0, 1, 0)
    m_rightX = -m_forwardZ; // Corrected sign
    m_rightY = 0;
    m_rightZ = m_forwardX;  // Corrected sign

    // Normalize the Right vector
    float len = sqrt(m_rightX * m_rightX + m_rightZ * m_rightZ);
    // Add a small check to prevent divide by zero if vector is (0,0,0)
    if (len > 0.0001f) {
        m_rightX /= len;
        m_rightZ /= len;
    }
    else {
        m_rightX = 1.0f;
        m_rightZ = 0.0f;
    }
}

void Camera::toggleMode() {
    m_isDeveloperMode = !m_isDeveloperMode;
    m_firstMouse = true; // Reset mouse on mode toggle

    if (m_isDeveloperMode) {
        printf("Camera: Developer Mode ENABLED\n");
        // Show the cursor
        glutSetCursor(GLUT_CURSOR_INHERIT);
        // Stop any jumping
        m_velY = 0;
    }
    else {
        printf("Camera: Game Mode ENABLED\n");
        // Hide the cursor
        glutSetCursor(GLUT_CURSOR_NONE);
        // Stick to ground
        m_posY = m_groundLevel;
        m_isJumping = false;
        m_velocityY = 0;
        // Center the mouse for a smooth transition
        centerMouse();
    }
}

void Camera::centerMouse() {
    glutWarpPointer(m_windowCenterX, m_windowCenterY);
}

void Camera::applyView() {
    // Tell gluLookAt where the camera is, and where to look
    // We look 1 unit in front of the camera's position
    gluLookAt(
        m_posX, m_posY, m_posZ,                      // Camera position (eye)
        m_posX + m_forwardX, m_posY + m_forwardY, m_posZ + m_forwardZ, // Look-at point (center)
        0.0f, 1.0f, 0.0f                       // World Up vector
    );
}

void Camera::onKeyDown(unsigned char key) {
    switch (key) {
    case 'w': m_inputForward = true;    break;
    case 's': m_inputBackward = true;   break;
    case 'a': m_inputStrafeLeft = true; break;
    case 'd': m_inputStrafeRight = true; break;
    case 'p': toggleMode();             break;
    case ' ': // Spacebar
        if (!m_isDeveloperMode && !m_isJumping) {
            jump();
        }
        break;
    case 'q': // Dev mode "fly down"
        if (m_isDeveloperMode) m_inputFlyDown = true;
        break;
    case 'e': // Dev mode "fly up"
        if (m_isDeveloperMode) m_inputFlyUp = true;
        break;
    }
}

void Camera::onKeyUp(unsigned char key) {
    switch (key) {
    case 'w': m_inputForward = false;    break;
    case 's': m_inputBackward = false;   break;
    case 'a': m_inputStrafeLeft = false; break;
    case 'd': m_inputStrafeRight = false; break;
    case 'q': m_inputFlyDown = false;    break;
    case 'e': m_inputFlyUp = false;      break;
    }
}

void Camera::onSpecialKeyDown(int key) {
    // Only used for Dev camera look
    if (m_isDeveloperMode) {
        switch (key) {
        case GLUT_KEY_LEFT:  m_inputLookLeft = true;  break;
        case GLUT_KEY_RIGHT: m_inputLookRight = true; break;
        case GLUT_KEY_UP:    m_inputLookUp = true;    break;
        case GLUT_KEY_DOWN:  m_inputLookDown = true;  break;
        }
    }
}

void Camera::onSpecialKeyUp(int key) {
    if (m_isDeveloperMode) {
        switch (key) {
        case GLUT_KEY_LEFT:  m_inputLookLeft = false;  break;
        case GLUT_KEY_RIGHT: m_inputLookRight = false; break;
        case GLUT_KEY_UP:    m_inputLookUp = false;    break;
        case GLUT_KEY_DOWN:  m_inputLookDown = false;  break;
        }
    }
}

void Camera::onMouseMovement(int x, int y) {
    // Only use mouse look in Game Mode
    if (m_isDeveloperMode) {
        return;
    }

    // Prevent massive jump on first frame or after mode toggle
    if (m_firstMouse) {
        centerMouse();
        m_firstMouse = false;
        return;
    }

    // Calculate how much the mouse has moved from the center
    float deltaX = (float)(x - m_windowCenterX);
    float deltaY = (float)(m_windowCenterY - y); // Inverted Y-axis

    // Apply sensitivity and update yaw/pitch
    m_yaw += deltaX * m_mouseSensitivity;
    m_pitch += deltaY * m_mouseSensitivity;

    // Clamp pitch to prevent flipping upside-down
    if (m_pitch > 89.0f) {
        m_pitch = 89.0f;
    }
    if (m_pitch < -89.0f) {
        m_pitch = -89.0f;
    }

    // Re-center the mouse for the next frame
    centerMouse();
}

void Camera::jump() {
    if (m_isJumping) return;
    m_isJumping = true; // <-- TYPO WAS HERE, NOW FIXED
    m_velY = m_jumpForce; // Use m_velY, not m_velocityY
}

// ================================================================
// THIS IS THE FULLY UPDATED "UPDATE" FUNCTION FOR SMOOTH MOVEMENT
// ================================================================
void Camera::update(float dt) {
    // 1. Recalculate orientation vectors (based on yaw/pitch)
    recalculateVectors();

    // 2. Handle Look (Dev Mode)
    if (m_isDeveloperMode) {
        // Arrow key looking
        if (m_inputLookLeft)  m_yaw -= m_lookSpeed * dt;
        if (m_inputLookRight) m_yaw += m_lookSpeed * dt;
        if (m_inputLookUp)    m_pitch += m_lookSpeed * dt;
        if (m_inputLookDown)  m_pitch -= m_lookSpeed * dt;

        // Clamp pitch
        if (m_pitch > 89.0f)  m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;
    }

    // 3. Determine Target Velocity (from input)
    float speed = m_isDeveloperMode ? m_devMoveSpeed : m_moveSpeed;
    float targetVelX = 0.0f;
    float targetVelY = 0.0f;
    float targetVelZ = 0.0f;

    // --- Calculate XZ target velocity (from WASD) ---
    float dirX = 0.0f, dirZ = 0.0f;
    if (m_inputForward) { dirX += m_forwardX; dirZ += m_forwardZ; }
    if (m_inputBackward) { dirX -= m_forwardX; dirZ -= m_forwardZ; }
    if (m_inputStrafeLeft) { dirX -= m_rightX;   dirZ -= m_rightZ; }
    if (m_inputStrafeRight) { dirX += m_rightX;   dirZ += m_rightZ; }

    // Normalize XZ direction
    float len = sqrt(dirX * dirX + dirZ * dirZ);
    if (len > 0.0001f) {
        dirX /= len;
        dirZ /= len;
    }
    targetVelX = dirX * speed;
    targetVelZ = dirZ * speed;

    // --- Calculate Y target velocity (Q/E or Jump Physics) ---
    if (m_isDeveloperMode) {
        // Dev mode: Q/E sets target Y velocity
        if (m_inputFlyUp)   targetVelY = m_devMoveSpeed;
        else if (m_inputFlyDown) targetVelY = -m_devMoveSpeed;
    }
    else {
        // Game mode: Jump physics controls Y velocity
        if (m_isJumping) {
            m_velY += m_gravity * dt; // Apply gravity
            if (m_posY <= m_groundLevel && m_velY < 0) { // Check for landing
                m_posY = m_groundLevel;
                m_isJumping = false;
                m_velY = 0;
            }
        }
        else {
            m_posY = m_groundLevel; // Stick to ground
            m_velY = 0;
        }
        // This is the key: targetVelY is set by physics (m_velY), not input.
        // But in jump(), we set m_velY directly.
        targetVelY = m_velY;
    }

    // 4. Apply Smoothing (Acceleration/Damping)
    // Check if there is XZ movement input
    bool hasXZInput = m_inputForward || m_inputBackward || m_inputStrafeLeft || m_inputStrafeRight;
    float currentAccel = hasXZInput ? m_acceleration : m_damping;

    // Interpolate current XZ velocity towards target XZ velocity
    m_velX += (targetVelX - m_velX) * currentAccel * dt;
    m_velZ += (targetVelZ - m_velZ) * currentAccel * dt;

    // 5. Apply Smoothing for Y (Dev Mode Only)
    if (m_isDeveloperMode) {
        bool hasYInput = m_inputFlyUp || m_inputFlyDown;
        currentAccel = hasYInput ? m_acceleration : m_damping;
        m_velY += (targetVelY - m_velY) * currentAccel * dt;
    }
    // In Game Mode, m_velY is controlled by physics (gravity) directly.
    // So we don't smooth it, we just use the value calculated in step 3.
    else {
        m_velY = targetVelY;
    }

    // 6. Apply Final Velocity to Position
    m_posX += m_velX * dt;
    m_posZ += m_velZ * dt;
    m_posY += m_velY * dt; // Y position is updated by physics (game) or smoothed velocity (dev)
}