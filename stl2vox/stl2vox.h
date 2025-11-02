////////////////////////////////////////////////////////////////////////////////////////    
// Copyright (c) 2024 Hajer Zhang, IDEAS, DLUT.
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy  of this 
// software and associated documentation files (the "Software"), to deal in the Software 
// without restriction, including without limitation the rights to use, copy, modify, 
// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
// permit persons to whom the Software is furnished to do so, subject to the following 
// conditions:  
//  
// The above copyright notice and this permission notice shall be included in all  
// copies or substantial portions of the Software.  
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,  
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A  
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT  
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  
// CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  
//  
// Author: Hajer Zhang 
// Date: 2024-12-17 
// Description: A voxelization library to process STL files into voxel grids, supporting 
//              academic, commercial, and various other purposes. Contributions and 
//              citations are welcome.  
//////////////////////////////////////////////////////////////////////////////////////
#ifndef __STL2VOX_H__
#define __STL2VOX_H__

#include <cstdint>
#include <cmath>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "stlMesh.h"
#include "voxGrid.h"

class stl2vox{
private:
    struct Int3 { int x, y, z; };

    static void printProgress(const std::string &prefix, size_t done, size_t total) {
        if (total == 0) return;
        int perc = int((double)done / (double)total * 100.0);
        std::cout << '\r' << prefix << " " << perc << "% (" << done << "/" << total << ")" << std::flush;
    }

    static void InputDimension(voxGrid &voxgrid){
        int numX, numY, numZ;
        std::cout << "Please Enter the Number of Voxels in X, Y, Z direction: ";
        std::cin >> numX >> numY >> numZ;

        voxgrid.dim[0] = numX;
        voxgrid.dim[1] = numY;
        voxgrid.dim[2] = numZ;

        voxgrid.value.resize((size_t)numX * numY * numZ, 1);
    }

    static void InitBackGrid(STLMesh &stlmesh, voxGrid &voxgrid){
        Vector3d maxGrid = stlmesh.triangleList[0].max();
        Vector3d minGrid = stlmesh.triangleList[0].min();

        for(const auto &triangle : stlmesh.triangleList)
        {
            auto maxTriangle = triangle.max();
            auto minTriangle = triangle.min();

            maxGrid = maxGrid.max(maxTriangle);
            minGrid = minGrid.min(minTriangle);
        }

        auto voxelSize = Vector3d(
            (maxGrid.x - minGrid.x) / voxgrid.dim[0],
            (maxGrid.y - minGrid.y) / voxgrid.dim[1],
            (maxGrid.z - minGrid.z) / voxgrid.dim[2]
        );

        maxGrid = maxGrid + voxelSize + voxelSize;
        minGrid = minGrid - voxelSize - voxelSize;

        voxelSize = Vector3d(
            (maxGrid.x - minGrid.x) / voxgrid.dim[0],
            (maxGrid.y - minGrid.y) / voxgrid.dim[1],
            (maxGrid.z - minGrid.z) / voxgrid.dim[2]
        );

        voxgrid.origin[0] = minGrid.x;
        voxgrid.origin[1] = minGrid.y;
        voxgrid.origin[2] = minGrid.z;

        voxgrid.spacing[0] = voxelSize.x;
        voxgrid.spacing[1] = voxelSize.y;
        voxgrid.spacing[2] = voxelSize.z;
    }

    static bool isSegmentIntersectTriangle(const Vector3d& p1, const Vector3d& p2, const Triangle& triangle)
    {
        Vector3d edge1 = triangle.v1 - triangle.v0;
        Vector3d edge2 = triangle.v2 - triangle.v0;
        Vector3d dir = p2 - p1;
        Vector3d h = dir.cross(edge2);

        double a = edge1.dot(h);
        if (std::abs(a) < 1e-6)
            return false;

        double f = 1.0 / a;
        Vector3d s = p1 - triangle.v0;
        double u = f * s.dot(h);
        if (u < 0.0 || u > 1.0)
            return false;

        Vector3d q = s.cross(edge1);
        double v = f * dir.dot(q);
        if (v < 0.0 || u + v > 1.0)
            return false;

        double t = f * edge2.dot(q);
        return t >= 0.0 && t <= 1.0;
    }

    static bool voxelIntersectsTriangle(const Vector3d& voxelMin, const Vector3d& voxelMax, const Triangle& triangle) 
    {
        Vector3d vertices[8] = {
            Vector3d(voxelMin.x, voxelMin.y, voxelMin.z),
            Vector3d(voxelMax.x, voxelMin.y, voxelMin.z),
            Vector3d(voxelMin.x, voxelMax.y, voxelMin.z),
            Vector3d(voxelMax.x, voxelMax.y, voxelMin.z),
            Vector3d(voxelMin.x, voxelMin.y, voxelMax.z),
            Vector3d(voxelMax.x, voxelMin.y, voxelMax.z),
            Vector3d(voxelMin.x, voxelMax.y, voxelMax.z),
            Vector3d(voxelMax.x, voxelMax.y, voxelMax.z)
        };

        int edges[12][2] = {
            {0, 1}, {1, 3}, {3, 2}, {2, 0}, 
            {4, 5}, {5, 7}, {7, 6}, {6, 4}, 
            {0, 4}, {1, 5}, {2, 6}, {3, 7}  
        };

        for (int i = 0; i < 12; i++) {
            Vector3d p1 = vertices[edges[i][0]];
            Vector3d p2 = vertices[edges[i][1]];
            if (isSegmentIntersectTriangle(p1, p2, triangle)) {
                return true;
            }
        }

        return false;
    }   

    static void MarkInsideVoxels(int x, int y, int z, voxGrid &voxgrid){
        voxgrid.value[z * voxgrid.dim[0] * voxgrid.dim[1] + y * voxgrid.dim[0] + x] = 1;
    }

    static void ComfirmSurfaceVoxels(STLMesh &stlmesh, voxGrid &voxgrid){
        const int numX = voxgrid.dim[0]; 
        const int numY = voxgrid.dim[1]; 
        const int numZ = voxgrid.dim[2];

        const Vector3d minGrid = Vector3d(voxgrid.origin[0], voxgrid.origin[1], voxgrid.origin[2]);
        const Vector3d voxelSize = Vector3d(voxgrid.spacing[0], voxgrid.spacing[1], voxgrid.spacing[2]);

        const size_t triCount = stlmesh.triangleList.size();
        size_t processed = 0;

#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic)
#endif
        for (size_t t = 0; t < triCount; ++t) {
            const Triangle &triangle = stlmesh.triangleList[t];
            Vector3d minTri = triangle.min();
            Vector3d maxTri = triangle.max();

            int tri_minGrid_x = std::max(0, (int)std::floor((minTri.x - minGrid.x) / voxelSize.x));
            int tri_minGrid_y = std::max(0, (int)std::floor((minTri.y - minGrid.y) / voxelSize.y));
            int tri_minGrid_z = std::max(0, (int)std::floor((minTri.z - minGrid.z) / voxelSize.z));

            int tri_maxGrid_x = std::min(numX - 1, (int)std::ceil((maxTri.x - minGrid.x) / voxelSize.x));
            int tri_maxGrid_y = std::min(numY - 1, (int)std::ceil((maxTri.y - minGrid.y) / voxelSize.y));
            int tri_maxGrid_z = std::min(numZ - 1, (int)std::ceil((maxTri.z - minGrid.z) / voxelSize.z));

            for(int z = tri_minGrid_z; z <= tri_maxGrid_z; ++z){
                for(int y = tri_minGrid_y; y <= tri_maxGrid_y; ++y){
                    for(int x = tri_minGrid_x; x <= tri_maxGrid_x; ++x){
                        Vector3d minVoxel = Vector3d(
                            minGrid.x + x * voxelSize.x,
                            minGrid.y + y * voxelSize.y,
                            minGrid.z + z * voxelSize.z
                        );
                        Vector3d maxVoxel = Vector3d(
                            minGrid.x + (x + 1) * voxelSize.x,
                            minGrid.y + (y + 1) * voxelSize.y,
                            minGrid.z + (z + 1) * voxelSize.z
                        );

                        if(voxelIntersectsTriangle(minVoxel, maxVoxel, triangle))
                        {
                            voxgrid.value[(size_t)z * numX * numY + (size_t)y * numX + (size_t)x] = 0;
                        }
                    }
                }
            }

#ifdef _OPENMP
#pragma omp atomic
#endif
            ++processed;

            if ((processed % std::max<size_t>(1, triCount / 100)) == 0) {
#ifndef _OPENMP
                printProgress("ComfirmSurfaceVoxels:", processed, triCount);
#endif
            }
        }

#ifndef _OPENMP
        printProgress("ComfirmSurfaceVoxels:", triCount, triCount);
        std::cout << std::endl;
#endif
    }

    // Flood fill from boundary
    static void ComfirmOutsideVoxels(voxGrid &voxgrid){
        const int numX = voxgrid.dim[0]; 
        const int numY = voxgrid.dim[1]; 
        const int numZ = voxgrid.dim[2];

        auto idx = [&](int x, int y, int z) -> size_t {
            return (size_t)z * numX * numY + (size_t)y * numX + (size_t)x;
        };

        std::queue<Int3> q;
        std::vector<char> visited((size_t)numX * numY * numZ, 0);

        size_t boundaryCount = 0;
        for (int z = 0; z < numZ; ++z) {
            for (int y = 0; y < numY; ++y) {
                for (int x = 0; x < numX; ++x) {
                    bool isBoundary = (x == 0 || y == 0 || z == 0 || x == numX-1 || y == numY-1 || z == numZ-1);
                    if (!isBoundary) continue;
                    size_t id = idx(x,y,z);
                    if (voxgrid.value[id] != 0 && !visited[id]) { 
                        voxgrid.value[id] = -1;
                        visited[id] = 1;
                        q.push({x,y,z});
                        ++boundaryCount;
                    }
                }
            }
        }

        size_t processed = 0;
        const int dirs[6][3] = {
            {1,0,0},{-1,0,0},
            {0,1,0},{0,-1,0},
            {0,0,1},{0,0,-1}
        };

        // BFS
        while (!q.empty()) {
            Int3 cur = q.front(); q.pop();
            ++processed;
            if ((processed & 0x3FF) == 0) {
                printProgress("ComfirmOutsideVoxels BFS:", processed, (size_t)numX * numY * numZ);
            }

            for (int k = 0; k < 6; ++k) {
                int nx = cur.x + dirs[k][0];
                int ny = cur.y + dirs[k][1];
                int nz = cur.z + dirs[k][2];
                if (nx < 0 || nx >= numX || ny < 0 || ny >= numY || nz < 0 || nz >= numZ) continue;
                size_t nid = idx(nx, ny, nz);

                if (!visited[nid] && voxgrid.value[nid] != 0) {
                    visited[nid] = 1;
                    voxgrid.value[nid] = -1;
                    q.push({nx, ny, nz});
                }
            }
        }

        printProgress("ComfirmOutsideVoxels BFS:", processed, (size_t)numX * numY * numZ);
        std::cout << std::endl;
    }

public:
    static void Convert(STLMesh &stlmesh, voxGrid &voxgrid){
        // 0. Get VoxelGrid Dimension
        InputDimension(voxgrid);

        // 1. Initial Background Grid
        InitBackGrid(stlmesh, voxgrid);

        // 2. Comfirm Surface Voxels
        ComfirmSurfaceVoxels(stlmesh, voxgrid);

        // 3. Mark Outside Voxels via flood-fill from boundary
        ComfirmOutsideVoxels(voxgrid);
    }
};

#endif
