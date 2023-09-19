# My opengl template

## Setup
```
git clone https://github.com/xsjk/my_opengl_template.git --recurse-submodules
cd my_opengl_template
mkdir build
cd build
cmake ..
cmake --build . -j8
```

To compile with [MinGW](https://winlibs.com/) on Windows, use ```cmake .. -G "MinGW Makefiles"``` instead of ```cmake ..```.


## Run

### Assignment 1

The scene with three `Bunny` models with different shaders and light sources.
```
./assignment-1.exe
```

### Assignment 2

The scene with Bezier surfaces and draggable control points.
```
./assignment-2.exe
```

### Assignment 5

The simulation of cloth with wind.
```
./assignment-5.exe
```


## Key bindings

### General
| Key | Description |
| --- | --- |
| `Esc` | Exit |

### Camera control

The same as Minecraft.

| Key | Description |
| --- | --- |
| `W` | Move forward |
| `S` | Move backward |
| `A` | Move left |
| `D` | Move right |
| `Space` | Move up |
| `Left Shift` | Move down |
| `Ctrl (hold)` | Speed up |
| `mouse wheel` | Zoom |

### Light control

| Key | Description |
| --- | --- |
| `F1` | Grab/release the light source where the secondary camera is placed |
| `=/-` | Increase/Decrease cone light angle |

### Display control

| Key | Description |
| --- | --- |
| `1` | Show/hide vertex |
| `2` | Show/hide wireframe |
| `3` | Show/hide face |
| `4` | Show/hide control points of Bezier/B-spline surface |
| `5` | Show/hide control grid of Bezier/B-spline surface |
| `6` | Show/hide control faces of Bezier/B-spline surface |

### Special

#### Assignment 1

| Key | Description |
| --- | --- |
| `Tab` | Switch the view of the secondary camera |

#### Assignment 2

| Key | Description |
| --- | --- |
| `T` | Toggle/untoggle adaptive tessellation |
| `R` | Reset the control points of the Bezier surface to the initial state |

#### Assignment 5

| Key | Description |
| --- | --- |
| `P` | Pause/unpause the simulation |
| `F1` | Grab/release the wind source |

The control points of the Bezier surface can be moved by dragging with the mouse.


