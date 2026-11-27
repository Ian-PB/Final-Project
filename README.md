# Project

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