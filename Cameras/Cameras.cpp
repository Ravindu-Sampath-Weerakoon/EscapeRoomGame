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

    // Calculate initial vectors
    recalculateVectors();

    // DO NOT call glutSetCursor here. GLUT is not initialized yet.
    // glutSetCursor(GLUT_CURSOR_NONE); // <-- This line was removed.
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
    m_rightX = m_forwardZ; // (m_forwardY * 0 - m_forwardZ * 1)
    m_rightY = 0;          // (m_forwardZ * 0 - m_forwardX * 0)
    m_rightZ = -m_forwardX; // (m_forwardX * 1 - m_forwardY * 0)

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
    //m_isJJumping = true;
    m_velocityY = m_jumpForce;
}

void Camera::update(float dt) {
    // This function is the "engine"
    // It runs every frame from your idle() function

    // 1. Recalculate orientation vectors (based on yaw/pitch)
    recalculateVectors();

    // 2. Get current speed
    float speed = m_isDeveloperMode ? m_devMoveSpeed : m_moveSpeed;

    // 3. Handle Movement (WASD)
    // We calculate a "wish" direction based on input
    float moveX = 0.0f;
    float moveY = 0.0f;
    float moveZ = 0.0f;

    if (m_inputForward) {
        moveX += m_forwardX;
        moveZ += m_forwardZ;
    }
    if (m_inputBackward) {
        moveX -= m_forwardX;
        moveZ -= m_forwardZ;
    }
    if (m_inputStrafeLeft) {
        moveX -= m_rightX;
        moveZ -= m_rightZ;
    }
    if (m_inputStrafeRight) {
        moveX += m_rightX;
        moveZ += m_rightZ;
    }

    // Apply movement
    m_posX += moveX * speed * dt;
    m_posZ += moveZ * speed * dt;


    // 4. Handle Mode-Specific Logic
    if (m_isDeveloperMode) {
        // --- DEVELOPER MODE ---
        // Arrow key looking
        if (m_inputLookLeft)  m_yaw -= m_lookSpeed * dt;
        if (m_inputLookRight) m_yaw += m_lookSpeed * dt;
        if (m_inputLookUp)    m_pitch += m_lookSpeed * dt;
        if (m_inputLookDown)  m_pitch -= m_lookSpeed * dt;

        // Clamp pitch
        if (m_pitch > 89.0f)  m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;

        // Q/E flying
        if (m_inputFlyUp)   m_posY += m_devMoveSpeed * dt;
        if (m_inputFlyDown) m_posY -= m_devMoveSpeed * dt;

    }
    else {
        // --- GAME MODE ---
        // In game mode, WASD only moves on XZ plane. Y is controlled by physics.
        // This was already handled above, as we didn't add m_forwardY to the move vector.

        // Handle Jump Physics
        if (m_isJumping) {
            m_posY += m_velocityY * dt;      // Move vertically
            m_velocityY += m_gravity * dt; // Apply gravity

            // Check for landing
            if (m_posY <= m_groundLevel) {
                m_posY = m_groundLevel;
                m_isJumping = false;
                m_velocityY = 0;
            }
        }
        else {
            // Stick to ground
            m_posY = m_groundLevel;
        }
    }
}