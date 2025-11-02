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


#ifndef __STLMESH_H__
#define __STLMESH_H__

#include <vector>

#include "vector3d.h"

struct Triangle
{
    Vector3d normal;
    Vector3d v0;
    Vector3d v1;
    Vector3d v2;
    ~Triangle() {}
    Vector3d min() const {
        Vector3d min = v0;
        min.x = std::min(v0.x, std::min(v1.x, v2.x));
        min.y = std::min(v0.y, std::min(v1.y, v2.y));
        min.z = std::min(v0.z, std::min(v1.z, v2.z));
        return min;
    }
    Vector3d max() const {
        Vector3d max = v0;
        max.x = std::max(v0.x, std::max(v1.x, v2.x));
        max.y = std::max(v0.y, std::max(v1.y, v2.y));
        max.z = std::max(v0.z, std::max(v1.z, v2.z));
        return max;
    }
    int mainDirect() const {
        return std::abs(normal.x) > std::abs(normal.y) ? 
            (std::abs(normal.x) > std::abs(normal.z) ? 0 : 2) : 
            (std::abs(normal.y) > std::abs(normal.z) ? 1 : 2);
    }
}; 

struct STLMesh
{   
    int numTriangles;
    std::vector<Triangle> triangleList;
    ~STLMesh() {
        triangleList.clear();
        triangleList.shrink_to_fit();
    }
};

#endif