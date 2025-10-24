// Cameras.cpp : Defines the functions for the static library.

#include "pch.h" // Must be the first include

// --- ADDED FOR INSTANT SPRINT (WINDOWS ONLY) ---
#ifdef _WIN32
#include <windows.h> // For GetAsyncKeyState
#endif
// ---------------------------------------------

#include "Cameras.h" // Include your own header
#include <stdio.h> // For printf
#include <math.h> // For sqrt

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
    // m_velocityY = 0.0f; // Removed
    m_isJumping = false;
    m_gravity = -20.0f; // A good gameplay gravity value
    m_jumpForce = 8.0f;

    // === ADDED FOR SMOOTHING ===
    m_acceleration = 12.0f;  // Tune this value
    m_damping = 10.0f;       // Tune this value
    m_velX = 0.0f;
    m_velY = 0.0f;
    m_velZ = 0.0f;

    // === ADDED FOR SPRINTING ===
    m_sprintMultiplier = 2.5f;       // The speed boost (Set to 5x)
    m_currentSpeedMultiplier = 1.0f; // Start at normal speed (1.0x)
    m_sprintAcceleration = 10.0f;    // How fast to speed up/slow down sprint
    m_inputSprint = false; // Will be updated by GetAsyncKeyState

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
        centerMouse(); // Center mouse initially in game mode
        m_firstMouse = true; // Ensure centering logic works correctly on start
    }
}


void Camera::setPosition(float x, float y, float z) {
    m_posX = x;
    m_posY = y;
    m_posZ = z;
    // Reset velocity when position is set externally
    m_velX = m_velY = m_velZ = 0.0f;
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
    if (len > 0.0001f) {
        m_rightX /= len;
        m_rightZ /= len;
    }
    else {
        // Handle case where forward is pointing straight up or down
        m_rightX = cos(yawRad);
        m_rightZ = sin(yawRad);
    }
}

void Camera::toggleMode() {
    m_isDeveloperMode = !m_isDeveloperMode;
    m_firstMouse = true; // Reset mouse on mode toggle

    if (m_isDeveloperMode) {
        printf("Camera: Developer Mode ENABLED\n");
        glutSetCursor(GLUT_CURSOR_INHERIT);
        m_isJumping = false;
        m_velY = 0;
    }
    else {
        printf("Camera: Game Mode ENABLED\n");
        glutSetCursor(GLUT_CURSOR_NONE);
        m_posY = m_groundLevel;
        m_isJumping = false;
        m_velY = 0;
        centerMouse();
    }
}

void Camera::centerMouse() {
    if (!m_isDeveloperMode) {
        glutWarpPointer(m_windowCenterX, m_windowCenterY);
    }
}

void Camera::applyView() {
    gluLookAt(
        m_posX, m_posY, m_posZ,
        m_posX + m_forwardX, m_posY + m_forwardY, m_posZ + m_forwardZ,
        0.0f, 1.0f, 0.0f
    );
}

void Camera::onKeyDown(unsigned char key) {
    switch (key) {
    case 'w': case 'W': m_inputForward = true;    break;
    case 's': case 'S': m_inputBackward = true;   break;
    case 'a': case 'A': m_inputStrafeLeft = true; break;
    case 'd': case 'D': m_inputStrafeRight = true; break;
    case 'p': case 'P': toggleMode();             break;
    case ' ': // Spacebar
        if (!m_isDeveloperMode && !m_isJumping) {
            jump();
        }
        break;
    case 'q': case 'Q': // Dev mode "fly down"
        if (m_isDeveloperMode) m_inputFlyDown = true;
        break;
    case 'e': case 'E': // Dev mode "fly up"
        if (m_isDeveloperMode) m_inputFlyUp = true;
        break;
    }
}

void Camera::onKeyUp(unsigned char key) {
    switch (key) {
    case 'w': case 'W': m_inputForward = false;    break;
    case 's': case 'S': m_inputBackward = false;   break;
    case 'a': case 'A': m_inputStrafeLeft = false; break;
    case 'd': case 'D': m_inputStrafeRight = false; break;
    case 'q': case 'Q': m_inputFlyDown = false;    break;
    case 'e': case 'E': m_inputFlyUp = false;      break;
    }
}

void Camera::onSpecialKeyDown(int key) {
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
    if (m_isDeveloperMode) {
        return;
    }
    if (m_firstMouse) {
        if (x == m_windowCenterX && y == m_windowCenterY) {
            m_firstMouse = false;
        }
        return;
    }

    float deltaX = (float)(x - m_windowCenterX);
    float deltaY = (float)(m_windowCenterY - y);

    if (fabs(deltaX) < 0.1f && fabs(deltaY) < 0.1f) {
        centerMouse();
        return;
    }

    m_yaw += deltaX * m_mouseSensitivity;
    m_pitch += deltaY * m_mouseSensitivity;

    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    centerMouse();
}

// This function is now OPTIONAL if using GetAsyncKeyState in update()
void Camera::updateModifiers(int modifiers) {
    // This function can be kept for compatibility or removed if
    // GetAsyncKeyState is handling the sprint check in update().
    // bool newSprintState = (modifiers & GLUT_ACTIVE_SHIFT);
    // m_inputSprint = newSprintState; // This would be overridden by update()
    //printf("DEBUG: updateModifiers called (optional).\n"); // Add this if you want to see if it's still being called
}


void Camera::jump() {
    if (m_isJumping) return;
    m_isJumping = true;
    m_velY = m_jumpForce; // Start upward velocity
}

// ================================================================
// Camera Update Function (Uses GetAsyncKeyState for Instant Sprint)
// ================================================================
void Camera::update(float dt) {
    // Prevent huge jumps if dt is abnormally large
    if (dt > 0.1f) dt = 0.1f;

    // --- 1. CHECK SPRINT KEY (WINDOWS VERSION) ---
    m_inputSprint = false; // Assume not sprinting by default
#ifdef _WIN32
    // Check if the Left Shift key is currently held down
    // (0x8000 is the bit flag for "currently pressed")
    if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {
        m_inputSprint = true;
    }
    // Optional: Check Right Shift too
    // if (GetAsyncKeyState(VK_RSHIFT) & 0x8000) {
    //     m_inputSprint = true;
    // }
#endif
    // ---------------------------------------------

    // 2. SMOOTH THE SPEED MULTIPLIER (Sprint)
    float targetMultiplier = m_inputSprint ? m_sprintMultiplier : 1.0f;
    m_currentSpeedMultiplier += (targetMultiplier - m_currentSpeedMultiplier) * m_sprintAcceleration * dt;

    // 3. Recalculate orientation vectors (based on yaw/pitch)
    recalculateVectors();

    // 4. Handle Look (Dev Mode - Arrow Keys)
    if (m_isDeveloperMode) {
        if (m_inputLookLeft)  m_yaw -= m_lookSpeed * dt;
        if (m_inputLookRight) m_yaw += m_lookSpeed * dt;
        if (m_inputLookUp)    m_pitch += m_lookSpeed * dt;
        if (m_inputLookDown)  m_pitch -= m_lookSpeed * dt;

        // Clamp pitch
        if (m_pitch > 89.0f)  m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;
    }

    // 5. Determine Target Velocity (Based on Input & Mode)
    float baseSpeed = m_isDeveloperMode ? m_devMoveSpeed : m_moveSpeed;
    float currentSpeed = baseSpeed * m_currentSpeedMultiplier; // Apply sprint

    float targetVelX = 0.0f;
    float targetVelY = 0.0f;
    float targetVelZ = 0.0f;

    // --- XZ Target Velocity (WASD) ---
    float moveDirX = 0.0f, moveDirZ = 0.0f;
    if (m_inputForward)   { moveDirX += m_forwardX; moveDirZ += m_forwardZ; }
    if (m_inputBackward)  { moveDirX -= m_forwardX; moveDirZ -= m_forwardZ; }
    if (m_inputStrafeLeft)  { moveDirX -= m_rightX;   moveDirZ -= m_rightZ;   }
    if (m_inputStrafeRight) { moveDirX += m_rightX;   moveDirZ += m_rightZ;   }

    // Normalize XZ direction vector if there's movement
    float moveLen = sqrt(moveDirX * moveDirX + moveDirZ * moveDirZ);
    if (moveLen > 0.0001f) {
        moveDirX /= moveLen;
        moveDirZ /= moveLen;
    }
    targetVelX = moveDirX * currentSpeed;
    targetVelZ = moveDirZ * currentSpeed;

    // --- Y Target Velocity (Q/E or Jump Physics) ---
    if (m_isDeveloperMode) {
        // Dev mode: Q/E sets target Y velocity directly
        if (m_inputFlyUp)       targetVelY = currentSpeed; // Sprint affects flying speed
        else if (m_inputFlyDown) targetVelY = -currentSpeed;
        else                     targetVelY = 0.0f;
    } else {
        // Game mode: Jump physics controls Y velocity
        if (m_isJumping) {
            m_velY += m_gravity * dt; // Apply gravity to current velocity
            // Check for landing (only if moving downwards)
            if (m_posY <= m_groundLevel && m_velY < 0.0f) {
                m_posY = m_groundLevel; // Snap to ground
                m_isJumping = false;
                m_velY = 0;         // Stop vertical velocity
            }
        } else {
            // Not jumping: ensure player stays on ground
            m_posY = m_groundLevel;
            m_velY = 0;
        }
        targetVelY = m_velY; // Target Y velocity *is* the physics velocity
    }

    // 6. Apply Smoothing (Lerp current velocity towards target velocity)
    bool hasXZInput = moveLen > 0.0001f;
    float currentAccel = hasXZInput ? m_acceleration : m_damping;

    m_velX += (targetVelX - m_velX) * currentAccel * dt;
    m_velZ += (targetVelZ - m_velZ) * currentAccel * dt;

    // Apply Y smoothing only in Dev Mode
    if (m_isDeveloperMode) {
        bool hasYInput = m_inputFlyUp || m_inputFlyDown;
        currentAccel = hasYInput ? m_acceleration : m_damping;
        m_velY += (targetVelY - m_velY) * currentAccel * dt;
    }
    // In Game Mode, m_velY is already set by physics, no smoothing needed.
    else {
        m_velY = targetVelY;
    }


    // 7. Apply Final Velocity to Position
    m_posX += m_velX * dt;
    m_posZ += m_velZ * dt;
    m_posY += m_velY * dt; // Apply calculated Y velocity
}