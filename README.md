## NEAT
NEar-Axis opTimisation

## Requirements
gsl, boost, gcc10, pybind11, cmake, python (with qsc and simsopt)

## Download NEAT
gyronimo and pybind11 added to NEAT using git submodules
To download gyronimo and pybind11 when cloning NEAT clone using the following command:
```bash
git clone --recurse-submodules https://github.com/rogeriojorge/NEAT.git
```
or, alternatively, after cloning NEAT, run:
```bash
git submodule init
git submodule update
```

## Install gyronimo
In NEAT's root folder, run
```bash
mkdir build
cd build
CXX=g++ cmake ../external/gyronimo
make
```
If you want to build documentation, run
```bash
make doc
```

## Compile NEAT
(soon to be moved to a docker container)
Example on MacOS
```bash
g++ -O2 -Wall -shared -std=c++20 -undefined dynamic_lookup  NEAT.cc -o NEAT.so $(python3 -m pybind11 --includes) -I/opt/local/include -L/opt/local/lib -lgsl -L../build -lgyronimo -I../external/gyronimo/ -Wl,-rpath ../build
```
Example on Linux
RUN g++-10 -std=c++2a -fPIC -shared NEAT.cc -o NEAT.so $(python -m pybind11 --includes) -L/usr/lib -lgsl -L../build -lgyronimo -I../external/gyronimo -Wl,-rpath ../build

## NEAT Docker Container
This document explains how to build the docker container for simsopt.
This process yields an image with roughly 2.32 GB and may a few minutes to load.

How to build the container:
0. Install docker
1. Build the docker image by running the `docker build` command in the repo root directory:
   ```bash
   docker build -t neat -f docker/Dockerfile.NEAT .
   ```
2. Run the docker image using the `docker run` command and your inputs file:
    ``` bash
    docker run -v "$(pwd)/inputs.py:/usr/src/app/inputs.py" neat
    <!-- docker run --env="DISPLAY" --env="QT_X11_NO_MITSHM=1" --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" --rm -it -v "$(pwd)/inputs.py:/usr/src/app/inputs.py" neat -->
    ```
Optional: If you want to log into the container, first run
    ```bash
    docker run -dit neat
    ```
    then attach your terminal's input to the running container using the CONTAINER ID specified by the `docker ps` command
    ``` bash
    docker attach CONTAINER ID
    ```