## CMake Instructions

CMake is an alternate build system to QMake that doesn't require as much setup.  The only prerequisite is to get a copy of cmake.  This can be done with various package managers (e.g. chocolatey on Windows, HomeBrew on Mac, apt-get / yum / whatever on Linux).  Windows is a bit more tricky than the others which is mostly the reason for this readme file.  Here is a list of steps that are verified to work on Windows and Mac, and a theoretical one on Linux.

### CMake Definitions

Here are the options for the CMake project (to be defined on the command line when invoking CMake, or through the CMake GUI):

- `PPL_PRIVATE_NAMESPACE`: defines the namespace for use when compiling PPL.  This is a requirement for building the library.
- `PPL_VERSION`: defines the version of the library (only useful on unix systems where dynamic libraries are versioned)
- `PPL_BUILD_STANDALONE`: If enabled, builds without a dependency on the X-Plane SDK
- `PPL_WITH_SOUND`: If enabled, builds with OpenAL support for loading WAV files, etc
- `PPL_WITH_FREETYPE`: If enabled, builds with the Freetype library to enable font management
- `PPL_WITH_SERIALIZATION`: If enabled, builds with support for serializing things to disk

### Common

If `PPL_BUILD_STANDALONE` is not specified, the X-Plane SDK must be installed to a directory adjacent to the PPL project in a folder called "SDK":

| 
| > PPL
| > SDK
| >> CHeaders
| >> etc
### Windows
 
 Windows is the most complicated of the bunch but it is still not too difficult.  It just needs a bit more attention to the location of things than the others.  If you don't enable any of the optional features (like sound support) then you won't actually need to do anything except, of course, have Visual Studio installed.  However there are some extra requirements for the optional features:

- `PPL_WITH_SOUND`:  This requires OpenAL, which is not installed by default.  You can install it from the [OpenAL website](https://www.openal.org/downloads/).  Choose the default folder (C:\Program Files (x86)\OpenAL 1.1 SDK or whatever your system drive is) and install it there.  CMake can detect it as long as it is in that folder.
- `PPL_WITH_SERIALIZATION`:  This requires boost, which you can get in a precompiled from from [this site](https://sourceforge.net/projects/boost/files/boost-binaries/).  Choose the version, and then the appropriate msvc version that corresponds to the version of Visual Studio that you have installed.  In my case I am using Visual Studio 2017 and Boost 1.6.8, so I choose the (misnamed) *boost_1_68_0-msvc-14.1-64.exe* (should be msvc-15.0).  Install this into C:\local\boost1_68_0 (or whatever version you chose).  If your system drive is not C: then install it to your system drive (unconfirmed if CMake can still find it but probably)

After you have installed the prerequisites you can then build, which is the easy part.  CMake builds in two steps:  project generation and project build.  The former is handled entirely by CMake while the latter is deferred to the appropriate build system (The MSVC developer tools in this case, but you can also use nmake if you feel like it with a different set of options).  The following set of invocations (in PowerShell) will handle the build from start to finish.  If CMake is not in your path, you will need to specify the full path to cmake.exe.

```powershell
New-Item -Type Directory build # mkdir build also works
cd build
cmake -G "Visual Studio 15 2017 Win64" -DPPL_PRIVATE_NAMESPACE=borrrden .. # Other versions of Visual Studio are also ok, but must have Win64
cmake --build . --config Release # Or open ppl.sln with Visual Studio
```

Now the library will be in a folder matching the config you specified (Debug if not specified) as `ppl.dll`

### Mac

The only prerequisite here is if `PPL_WITH_SERIALIZATION` is enabled.  libboost will need to be installed on your system.  You can use HomeBrew to install it.  After that you can then build.  CMake builds in two steps:  project generation and project build.  The former is handled entirely by CMake while the latter is deferred to the appropriate build system (Unix makefiles, but you can also use other systems if you feel like it with a different set of options).  The following set of invocations will handle the build from start to finish.

```sh
mkdir build
cd build
cmake -DPPL_PRIVATE_NAMESPACE=borrrden ..
make -j8 # jobs should be about equal to the number of virtual cores
```

This will create a debug build which will show up as `libppl.dylib` in the current direcory.  To make a release build you can invoke `cmake -DCMAKE_BUILD_TYPE=Release -DPPL_PRIVATE_NAMESPACE=borrrden` instead before calling `make`.

### Linux

There are a few (or no) prerequisites here based on what you have enabled.  

- `PPL_WITH_SOUND`: This requires OpenAL.  Most package managers should have a package for it (e.g. Ubuntu -> `apt-get install openal-dev`)
- `PPL_WITH_SERIALIZATION`: This requires libboost (e.g. Ubuntu -> `apt-get install libboost-dev`)
 
After that you can then build.  CMake builds in two steps:  project generation and project build.  The former is handled entirely by CMake while the latter is deferred to the appropriate build system (Unix makefiles, but you can also use other systems if you feel like it with a different set of options).  The following set of invocations will handle the build from start to finish.

```sh
mkdir build
cd build
cmake -DPPL_PRIVATE_NAMESPACE=borrrden ..
make -j8 # jobs should be about equal to the number of virtual cores
```

This will create a debug build which will show up as `libppl.so` in the current directory.  To make a release build you can invoke `cmake -DCMAKE_BUILD_TYPE=Release -DPPL_PRIVATE_NAMESPACE=borrrden` instead before calling `make`.
