// Cameras.cpp : Defines the functions for the static library.

#include "pch.h" // Must be the first include

// --- ADDED FOR INSTANT SPRINT (WINDOWS ONLY) ---
#ifdef _WIN32
#include <windows.h> // For GetAsyncKeyState
// FIX: Tell the linker to automatically include user32.lib for input functions
#pragma comment(lib, "user32.lib") 
#endif
// ---------------------------------------------

#include "Cameras.h" 
#include "GraphicsUtils.h" // <-- Include for collision functions
#include <stdio.h> 
#include <math.h> 

// --- Collision Padding ---
const float CAMERA_COLLISION_PADDING = 0.2f; // How far camera stops from blocked cell edge


Camera::Camera(int windowWidth, int windowHeight) {
    onWindowResize(windowWidth, windowHeight);

    m_isDeveloperMode = false; // Start in Game Mode
    m_firstMouse = true;

    // Set physics/ground variables first
    m_groundLevel = 0.0f;
    m_isJumping = false;
    m_gravity = -20.0f;
    m_jumpForce = 8.0f;

    // Set initial position
    // We set Y to ground level initially
    m_posY = m_groundLevel;
    setPosition(0.0f, 5.0f); // UPDATED: Only X and Z

    m_yaw = 90.0f; // Look forward (along -Z)
    m_pitch = 0.0f;

    // Clear all input states
    m_inputForward = m_inputBackward = m_inputStrafeLeft = m_inputStrafeRight = false;
    m_inputLookLeft = m_inputLookRight = m_inputLookUp = m_inputLookDown = false;
    m_inputFlyUp = m_inputFlyDown = false;

    // Set speeds
    m_moveSpeed = 2.5f;   // units per second
    m_devMoveSpeed = 8.0f; // Dev mode is faster
    m_lookSpeed = 90.0f;  // degrees per second
    m_mouseSensitivity = 0.1f;

    // === SMOOTHING ===
    m_acceleration = 12.0f;
    m_damping = 10.0f;
    m_velX = 0.0f;
    m_velY = 0.0f;
    m_velZ = 0.0f;

    // === SPRINTING ===
    m_sprintMultiplier = 2.5f;
    m_currentSpeedMultiplier = 1.0f;
    m_sprintAcceleration = 8.0f;
    m_inputSprint = false;

    // Calculate initial vectors
    recalculateVectors();
}

// This is the new function that gets called from main()
void Camera::init() {
    // Hide cursor for game mode
    if (!m_isDeveloperMode) {
        glutSetCursor(GLUT_CURSOR_NONE);
        centerMouse(); // Center mouse initially in game mode
        m_firstMouse = true; // Ensure centering logic works correctly on start
    }
}

// UPDATED: Takes 2 arguments (X, Z) as requested. Y is handled by GroundLevel/Physics.
void Camera::setPosition(float x, float z) {
    m_posX = x;
    m_posZ = z;
    // We do NOT change m_posY here. It stays at current height or ground level.

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
    float yawRad = m_yaw * DEG_TO_RAD;
    float pitchRad = m_pitch * DEG_TO_RAD;

    m_forwardX = cos(pitchRad) * sin(yawRad);
    m_forwardY = sin(pitchRad);
    m_forwardZ = -cos(pitchRad) * cos(yawRad);

    m_rightX = -m_forwardZ;
    m_rightY = 0;
    m_rightZ = m_forwardX;

    float len = sqrt(m_rightX * m_rightX + m_rightZ * m_rightZ);
    if (len > 0.0001f) {
        m_rightX /= len;
        m_rightZ /= len;
    }
    else {
        m_rightX = cos(yawRad);
        m_rightZ = sin(yawRad);
    }
}

void Camera::toggleMode() {
    m_isDeveloperMode = !m_isDeveloperMode;
    m_firstMouse = true;

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
    case 'q': case 'Q':
        if (m_isDeveloperMode) m_inputFlyDown = true;
        break;
    case 'e': case 'E':
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
        if (abs(x - m_windowCenterX) < 5 && abs(y - m_windowCenterY) < 5) {
            m_firstMouse = false;
        }
        else {
            centerMouse();
        }
        return;
    }

    float deltaX = (float)(x - m_windowCenterX);
    float deltaY = (float)(m_windowCenterY - y);

    if (fabs(deltaX) < 1.0f && fabs(deltaY) < 1.0f) {
        centerMouse();
        return;
    }

    m_yaw += deltaX * m_mouseSensitivity;
    m_pitch += deltaY * m_mouseSensitivity;

    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    centerMouse();
}

void Camera::updateModifiers(int modifiers) {
    // Optional
}


void Camera::jump() {
    if (m_isJumping) return;
    m_isJumping = true;
    m_velY = m_jumpForce; // Start upward velocity
}

// ================================================================
// Camera Update Function (Includes Grid Collision)
// ================================================================
void Camera::update(float dt) {
    if (dt > 0.1f) dt = 0.1f;

    // --- 1. CHECK SPRINT KEY ---
    m_inputSprint = false;
#ifdef _WIN32
    if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {
        m_inputSprint = true;
    }
#endif

    // --- 2. SMOOTH SPEED ---
    float targetMultiplier = m_inputSprint ? m_sprintMultiplier : 1.0f;
    m_currentSpeedMultiplier += (targetMultiplier - m_currentSpeedMultiplier) * m_sprintAcceleration * dt;

    // --- 3. Recalculate orientation ---
    recalculateVectors();

    // --- 4. Handle Look (Dev Mode) ---
    if (m_isDeveloperMode) {
        if (m_inputLookLeft)  m_yaw -= m_lookSpeed * dt;
        if (m_inputLookRight) m_yaw += m_lookSpeed * dt;
        if (m_inputLookUp)    m_pitch += m_lookSpeed * dt;
        if (m_inputLookDown)  m_pitch -= m_lookSpeed * dt;
        if (m_pitch > 89.0f)  m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;
    }

    // --- 5. Determine Target Velocity ---
    float baseSpeed = m_isDeveloperMode ? m_devMoveSpeed : m_moveSpeed;
    float currentSpeed = baseSpeed * m_currentSpeedMultiplier;

    float targetVelX = 0.0f;
    float targetVelY = 0.0f;
    float targetVelZ = 0.0f;

    float moveDirX = 0.0f, moveDirZ = 0.0f;
    if (m_inputForward) { moveDirX += m_forwardX; moveDirZ += m_forwardZ; }
    if (m_inputBackward) { moveDirX -= m_forwardX; moveDirZ -= m_forwardZ; }
    if (m_inputStrafeLeft) { moveDirX -= m_rightX;   moveDirZ -= m_rightZ; }
    if (m_inputStrafeRight) { moveDirX += m_rightX;   moveDirZ += m_rightZ; }

    float moveLen = sqrt(moveDirX * moveDirX + moveDirZ * moveDirZ);
    if (moveLen > 0.0001f) {
        moveDirX /= moveLen;
        moveDirZ /= moveLen;
    }
    targetVelX = moveDirX * currentSpeed;
    targetVelZ = moveDirZ * currentSpeed;

    if (m_isDeveloperMode) {
        if (m_inputFlyUp)       targetVelY = currentSpeed;
        else if (m_inputFlyDown) targetVelY = -currentSpeed;
        else                     targetVelY = 0.0f;
    }
    else {
        if (m_isJumping) {
            m_velY += m_gravity * dt;
        }
        else {
            m_velY = 0;
        }
        targetVelY = m_velY;
    }

    // --- 6. Apply Smoothing ---
    bool hasXZInput = moveLen > 0.0001f;
    float currentAccel = hasXZInput ? m_acceleration : m_damping;

    m_velX += (targetVelX - m_velX) * currentAccel * dt;
    m_velZ += (targetVelZ - m_velZ) * currentAccel * dt;

    if (m_isDeveloperMode) {
        bool hasYInput = m_inputFlyUp || m_inputFlyDown;
        currentAccel = hasYInput ? m_acceleration : m_damping;
        m_velY += (targetVelY - m_velY) * currentAccel * dt;
    }
    else {
        m_velY = targetVelY;
    }

    // --- 7. Calculate Potential Position ---
    float potentialNextX = m_posX + m_velX * dt;
    float potentialNextZ = m_posZ + m_velZ * dt;
    float potentialNextY = m_posY + m_velY * dt;

    // --- 8. Check Grid Collision ---
    if (!m_isDeveloperMode) {
        // X-Axis Check
        float checkX = m_posX + (m_velX > 0 ? CAMERA_COLLISION_PADDING : -CAMERA_COLLISION_PADDING) + m_velX * dt;
        if (isGridPositionBlocked(checkX, m_posZ)) {
            m_velX = 0;
            potentialNextX = m_posX;
        }

        // Z-Axis Check
        float checkZ = m_posZ + (m_velZ > 0 ? CAMERA_COLLISION_PADDING : -CAMERA_COLLISION_PADDING) + m_velZ * dt;
        if (isGridPositionBlocked(potentialNextX, checkZ)) {
            m_velZ = 0;
            potentialNextZ = m_posZ;
        }

        // Ground Check
        if (m_isJumping && potentialNextY <= m_groundLevel && m_velY < 0.0f) {
            potentialNextY = m_groundLevel;
            m_isJumping = false;
            m_velY = 0;
        }
        else if (!m_isJumping) {
            potentialNextY = m_groundLevel;
            m_velY = 0;
        }
    }

    // --- 9. Apply Final Position ---
    m_posX = potentialNextX;
    m_posY = potentialNextY;
    m_posZ = potentialNextZ;

    if (!m_isDeveloperMode && m_posY < m_groundLevel) {
        m_posY = m_groundLevel;
        if (m_isJumping) {
            m_isJumping = false;
            m_velY = 0;
        }
    }
}