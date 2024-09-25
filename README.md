SDL Raycasting Game
This project is a 3D raycasting game created using the SDL2 library. The game features a first-person view, allowing the player to navigate through a 3D maze-like environment. It includes dynamic features like weapon rendering, raindrop effects, and a mini-map.

Table of Contents
Features
Requirements
Installation
Usage
Controls
File Structure
Code Overview
Credits
License
Features
Raycasting Engine: Implements a raycasting algorithm to render a 3D maze-like environment.
Weapon Rendering: Different weapon states (aim, fire, recoil, reload) are visualized.
Raindrop Effect: Realistic raindrop animation on the screen.
Mini-Map: Displays a 2D overhead view of the current map and player position.
Dynamic Textures: Textures for walls, sky, and weapons are dynamically loaded.
Audio Integration: Includes sound effects for weapon firing using SDL_mixer.
Requirements
To run this project, you need:

SDL2 library
SDL2_image library
SDL2_mixer library
A C compiler (e.g., GCC)
Libraries Installation (Ubuntu/Debian)
bash
Copy code
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev
Installation
Clone the repository:

bash
Copy code
git clone https://github.com/username/sdl-raycasting-game.git
cd sdl-raycasting-game
Compile the project:

bash
Copy code
gcc -o raycasting_game main.c -lSDL2 -lSDL2_image -lSDL2_mixer -lm
Run the executable:

bash
Copy code
./raycasting_game
Usage
Running the Game
Launch the game using the compiled executable (./raycasting_game).
Navigate through the 3D environment using the keyboard controls.
Exiting the Game
Press ESC or close the game window to exit.
Controls
W or Up Arrow: Move forward
S or Down Arrow: Move backward
A or Left Arrow: Rotate left
D or Right Arrow: Rotate right
M: Toggle mini-map
R: Toggle raindrop effect
Space: Fire weapon
ESC: Exit the game
File Structure
python
Copy code
├── README.md                   # Project documentation
├── main.c                      # Main source code file
├── map.txt                     # Map data file
├── resources                   # Resources folder for textures and sprites
│   ├── textures                # Textures for walls and sky
│   └── sprites                 # Sprites for weapons and other entities
└── audio                       # Audio resources
    └── shotgun.wav             # Shotgun sound effect
Map File (map.txt)
The map file contains the layout of the world using integers, where each integer represents a different type of wall or empty space.

0 - Empty space
1 to 5 - Wall types
Code Overview
Main Components
Initialization:

Initializes SDL, SDL_image, and SDL_mixer.
Loads textures and audio resources.
Sets up the player’s initial position and direction.
Raycasting Function (performRaycasting):

Casts rays from the player's position to render the 3D environment.
Calculates intersections with walls and determines the distance to draw the appropriate slice of wall texture.
Rendering Functions:

renderWeapon: Handles rendering of different weapon states.
drawMiniMap: Draws a scaled-down version of the map, showing the player's position and direction.
drawRain: Renders raindrop effects.
Player Input Handling:

Manages player movement, rotation, and weapon firing using keyboard inputs.
Resource Management:

Loads and releases textures and audio resources.
Credits
SDL2: For providing a powerful cross-platform development library.
OpenGameArt: For textures and sprites.
SDL2_mixer: For handling audio in the game.
License
This project is licensed under the MIT License. See the LICENSE file for more details.
