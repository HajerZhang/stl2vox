# MyVoxelizer
This is a Simple Voxelizer Program for convert the file type between .stl and .vtk(voxel).
## Prerequisites

* using the CMake to build the project
* using ParaView to visualize the .vtk file

## Getting Started

First, clone the repository and configure the build directory:
```
cd stl2vox
g++ main.cpp -O3 -o main
```
Then, you can run the program with the following command:
```
./main ../model/sofa.stl
```

## Results

<div style="display: flex; justify-content: space-between;">
    <img src="./doc/stl.png" alt="Image 1" style="width: 48%;">
    <img src="./doc/512.png" alt="Image 2" style="width: 48%;">
</div>
<p style="text-align: center;">Figure 1: The original model (left) and the voxelized model (right) of sofa model.</p>


## Authors
* Hajer Zhang, IDEAS, DLUT, China
* Email: howizezhang@gmail.com
* Date: 2024.12.18

## Update
* 2024.12.23：Add a [Viewer](./Viewer.py) based on the VTK, pyvista, pyvistaqt, and PyQt5. But the viewer is not stable now.
<div style="display: flex; justify-content: center; margin: auto; width: 80%;">
    <img src="./doc/gui.png" alt="Image 1" style="width: 96%;">
</div>

* 2024.12.25：Update the new algorithm for voxelization, and the new algorithm is more efficient than the previous one.
<div style="display: flex; justify-content: space-between;">
    <img src="./doc/before.png" alt="Image 1" style="width: 48%;">
    <img src="./doc/after.png" alt="Image 2" style="width: 48%;">
</div>

<p style="text-align: center;">Figure 2: Comparison of the algorithm before (left) and after (right) of bunny model.</p>

* 2025-11-2：Change Project InsideVoxels to Flood Fill.


