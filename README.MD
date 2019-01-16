## Computer Vision - Object Tracking

Authors: Jan Hanke, Sebastian Damrath

## Dependencies
- https://github.com/zaphoyd/websocketpp/ (available in Arch community as `websocketpp`)
- boost/asio (websocketpp depends on it)

## Install dependencies

```bash
sudo pacman -S boost boost-libs websocketpp
```

## OpenCV Installation Linux

- GCC 4.4.x or later
- CMake 2.8.7 or higher
- Git
- GTK+2.x or higher, including headers (libgtk2.0-dev)
- pkg-config
- Python 2.6 or later and Numpy 1.5 or later with developer packages (python-dev, python-numpy)
- ffmpeg or libav development packages: libavcodec-dev, libavformat-dev, libswscale-dev

Download [OpenCV](http://opencv.org/releases.html) or use Git

```bash
git clone https://github.com/opencv/opencv.git
cd ~/opencv
mkdir build
``` 
Run `cmake-gui` and set the absolute source path to `~/opencv`.
Set the build path to `~/opencv/build`. Add the following entries:

| Name                    | Type   | Value        | 
|-------------------------|--------|--------------|
| `CMAKE_INSTALL_PREFIX`  | PATH   | `/usr/local` |
| `CMAKE_BUILD_TYPE`      | String | `Release`    |
| (optional) `BUILD_DOCS` | BOOL   | `true`       |

Click _Configure_ and then _Generate_. 

```bash
cd ~/opencv/build

# build using 7 parallel jobs
make -j7            

#install libraries to system
sudo make install   
```

_Optional - Docs:_
```
cd ~/opencv/build/doc/
make -j7 doxygen
```

Done.
