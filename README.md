# Escape Room Game

A first-person 3D puzzle exploration game built with C++ and OpenGL. Navigate through rooms, find clues in interactive notes, and unlock secret doors using PIN codes to escape.

## 🚀 Features

- **FPS Camera System:** Smooth first-person movement with gravity, ground-level detection, and a collision system.
- **Interactive Environment:**
  - **Secret Books/Notes:** Read notes scattered around the room to find lore and PIN code hints.
  - **Locked Doors:** Advance through the game by entering the correct PIN codes on secure doors.
  - **Room Decorations:** Detailed environment with chairs, tables, cupboards, plants, lamps, and more.
- **Dynamic Lighting:**
  - **Flashlight (Torch):** Toggleable spotlight to navigate dark areas.
  - **Player Aura:** A soft lantern-like light that follows the player.
  - **Developer Mode:** Full brightness mode for debugging.
- **Heads-Up Display (HUD):**
  - Real-time coordinates and game mode status.
  - Interaction hints (e.g., "Press 'E' to Read").
  - Center-screen UI for reading notes and entering PINs.
- **Collision System:** Grid-based collision detection to prevent walking through walls and furniture.
- **Optimized Rendering:** Uses OpenGL Display Lists and Mipmapped textures (via SOIL2) for performance.

## 🎮 Controls

| Key | Action |
|-----|--------|
| **W / A / S / D** | Move Forward / Left / Backward / Right |
| **Mouse** | Look around |
| **E** | Interact (Read Book / Unlock Door / Cancel PIN entry) |
| **F** | Toggle Flashlight |
| **Tab** | Toggle Help Menu |
| **Shift** | Run (In Developer Mode) |
| **Space** | Fly Up (In Developer Mode) |
| **V** | Fly Down (In Developer Mode) |
| **M** | Toggle Developer Mode |
| **Esc** | Exit Game |

*In Developer Mode:*
- **T:** Toggle Coordinate Axes
- **C:** Toggle Navigation Grid

## 📂 Project Structure

The project is organized into several modular sub-projects (Visual Studio Static Libraries):

- **EscapeRoomGame:** The main entry point. Handles window creation, main loop, and orchestrates all modules.
- **TheRoom:** Manages the primary environment geometry (floor, walls, ceiling) and texture mapping.
- **Cameras:** Implements the First-Person camera, input handling, and collision logic.
- **SecretDoor:** Handles the logic, animation, and PIN-code validation for interactive doors.
- **SecretBook:** Manages interactive notes and books that display text messages when read.
- **RoomDecorations:** Adds various 3D models (built from primitives) like furniture and decorative objects.
- **InsideWall & CornerTower:** Specialized modules for room layout and structural elements.
- **Labels:** A 2D HUD system for displaying text, hints, and menus.
- **GraphicsUtils:** Utility functions for collision grid management and debug visualizations (axes/grids).

## 🛠️ Technical Details

- **Language:** C++
- **Graphics API:** OpenGL (using GLUT and GLU)
- **Texture Loading:** SOIL2 (Simple OpenGL Image Library)
- **Asset Formats:** `.dds` (DirectDraw Surface) for optimized textures and `.jpg` for standard images.

## 🏗️ Building and Running

1.  Open `EscapeRoomGame.sln` in **Visual Studio 2022** (or compatible).
2.  Ensure the configuration is set to `Debug` or `Release` for your preferred architecture (x86 recommended based on existing DLLs).
3.  The project includes all necessary headers and libraries in the `Dependencies/` folder.
4.  DLLs (`glut32.dll`, `glu32.dll`, `opengl32.dll`) are provided in the project folders to ensure it runs out-of-the-box.
5.  Build the solution and run the `EscapeRoomGame` project.

## 📜 Credits

Created as a 3D Graphics project exploring interactive environments and game mechanics in C++.

## 🧩 Puzzles & Clues

The game features several notes and clues to help you unlock doors. Here are some of the hints you might find:

- **Note #1:** "The first number is the loneliest number." (1)
- **Note #2:** "Look at your hand. Count the fingers." (5)
- **Note #3:** "Days in a week. Colors in a rainbow." (7)
- **PIN Hint:** "157" (Derived from the notes above).
- **TV Room PIN:** The year the first electronic TV was demonstrated (1927).
- **Space Clue:** "Apollo 11 mission", "three astronauts", "four inner planets" (1134).
- **Bedroom Clue:** "Look at the sofa", "pillows in my bedroom".
