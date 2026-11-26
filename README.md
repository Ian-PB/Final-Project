# Project

## About Project
This project is my current final year project, because of this it is very plain currently.
What the project currently has:
- Player Movement
- Collision Detection / Resolution
- A Dynamic World
- A Debug Menu (pressing escape)

For this project I would like to turn this into a multiplayer shooter style game where the following is synced between players:
- Players (position, size, health)
- The World (Any edits made by players will be synced up to match every player playing)
- Projectiles (Currently there is only a bomb projectile but this will also include bullets)

## Run Instructions
Just pressing run in visual studio should work.
If it doesn't work please go into the Project properties add this:

**C/C++ -> General -> Additional Include Directories:**
```$(SolutionDir)Project Requirements\raylib-6.0_win64_msvc16\include;$(SolutionDir)Project Requirements\raygui-5.0\src```

**Linker -> General -> Additional Library Directories:** 
```$(SolutionDir)Project Requirements\raylib-6.0_win64_msvc16\lib```

**Linker -> Input -> Additional Dependencies:**
```raylib.lib;winmm.lib;$(CoreLibraryDependencies);%(AdditionalDependencies)```

## Controls
- **WASD** - Movement
- **Mouse** - Look around
- **Left Mouse** - Shoot
- **Right Mouse** - Swap between destruction and building
- **Space** - Jump
- **Left Shift** - Slide / crouch
- **Escape** - Debug / Pause Menu
- **F10** - Close game
- **F9** - Toggle fullscreen