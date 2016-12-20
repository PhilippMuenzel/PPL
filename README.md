The **Plugin Patterns Library** provides classes for common tasks with the XPLM SDK for the X-Plane flight simulator. It is provided under BSD license.

Questions, support and pull request are handled through http://forums.x-plane.org/index.php?showtopic=49958

---

Library contents
================

Grouped by category


Utility
-------

**namespaces.h**
Prevents name collision on 32-bit systems

**pluginpath.h**
Platform-independent paths to various folders

**logwriter.h**
Writes log messages to file

**log.h**
Defines log messages

**settings.h**
For loading and saving settings in a .ini file

**basics.h**
Maths utility functions

**smoothed.h**
Superficial smoothing - returns average of the last n assigned values


Sim interface
-------------

**dataref.h**
Connects with existing datarefs

**owneddata.h**
Creates new datarefs

**commandbase.h**
Abstract base for commands

**processor.h**
Abstract base for flight-loop callbacks

**logichandler.h**
Abstract class for XP > FLCB > custom dataref processing

**sharedobject.h**
Passes large objects as binary data to/from datarefs


User interface
--------------

**messagewindow.h**
Displays pop-up message

**onscreendisplay.h**
Draws a window on the screen

**menuitem.h**
Adds item to plugin menu

**action.h**
Abstract base for menu items


Sound
-----

**alsoundbuffer.h**
Stores a sound file

**alcontextmanager.h**
Manages the playing of sound buffers

**alcontextchanger.h**
Changes AL context using RAII


3D
--

**overlaygauge.h**
??? Draws gauges on the 3D panel texture?

**texture.h**
OpenGL wrapper for .bmp texture data?

**fontmgr.h**
Loads and maps fonts for drawing onto textures

**vertexbuffer.hpp**
For storing objects in VRAM
