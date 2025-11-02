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


#ifndef __STLREADER_H__
#define __STLREADER_H__

#include <iostream>
#include <vector>
#include <fstream>

#include "stlMesh.h"

class stlReader
{
private:
    static void OutputStlInfo(STLMesh &stlmesh){
        std::cout << "STL File Information: " << std::endl;
        std::cout << "Number of Triangles: " << stlmesh.numTriangles << std::endl;
    
        Vector3d minGrid = Vector3d(stlmesh.triangleList[0].v0.x, stlmesh.triangleList[0].v0.y, stlmesh.triangleList[0].v0.z);
        Vector3d maxGrid = Vector3d(stlmesh.triangleList[0].v0.x, stlmesh.triangleList[0].v0.y, stlmesh.triangleList[0].v0.z);

        for(auto triangle : stlmesh.triangleList)
        {
            if(triangle.v0.x < minGrid.x) minGrid.x = triangle.v0.x;
            if(triangle.v0.y < minGrid.y) minGrid.y = triangle.v0.y;
            if(triangle.v0.z < minGrid.z) minGrid.z = triangle.v0.z;

            if(triangle.v0.x > maxGrid.x) maxGrid.x = triangle.v0.x;
            if(triangle.v0.y > maxGrid.y) maxGrid.y = triangle.v0.y;
            if(triangle.v0.z > maxGrid.z) maxGrid.z = triangle.v0.z;
        }

        std::cout << "X Range: " << minGrid.x << " - " << maxGrid.x << std::endl;
        std::cout << "Y Range: " << minGrid.y << " - " << maxGrid.y << std::endl;
        std::cout << "Z Range: " << minGrid.z << " - " << maxGrid.z << std::endl;

        double lengthX = maxGrid.x - minGrid.x;
        double lengthY = maxGrid.y - minGrid.y;
        double lengthZ = maxGrid.z - minGrid.z;

        int minDirect = (std::abs(lengthX) < std::abs(lengthY)) ? 
            ((std::abs(lengthX) < std::abs(lengthZ)) ? 0 : 2) : 
            ((std::abs(lengthY) < std::abs(lengthZ)) ? 1 : 2);
        
        double rateX, rateY, rateZ;
        if(minDirect == 0)
        {
            rateX = 1;
            rateY = (maxGrid.y - minGrid.y) / (maxGrid.x - minGrid.x);
            rateZ = (maxGrid.z - minGrid.z) / (maxGrid.x - minGrid.x);
        }else if (minDirect == 1)
        {
            rateX = (maxGrid.x - minGrid.x) / (maxGrid.y - minGrid.y);
            rateY = 1;
            rateZ = (maxGrid.z - minGrid.z) / (maxGrid.y - minGrid.y);
        }else{
            rateX = (maxGrid.x - minGrid.x) / (maxGrid.z - minGrid.z);
            rateY = (maxGrid.y - minGrid.y) / (maxGrid.z - minGrid.z);
            rateZ = 1;
        }

        std::cout << "X : Y : Z = " << rateX << " : " << rateY << " : " << rateZ << std::endl;
    };

public:
    static void ReadStlFile(const std::string& filename, STLMesh &stlmesh){
        std::ifstream file(filename, std::ios::binary);
        if(!file.is_open()){
            throw std::runtime_error("Failed to open file");
        }

        file.seekg(80);
        int num_triangles;
        file.read(reinterpret_cast<char*>(&num_triangles), sizeof(num_triangles));
        stlmesh.numTriangles = num_triangles;
        stlmesh.triangleList.reserve(num_triangles);

        for(int i = 0; i < num_triangles; ++i) {
            Triangle tri;
            float normal[3], v0[3], v1[3], v2[3];
            file.read(reinterpret_cast<char*>(normal), 3 * sizeof(float));
            file.read(reinterpret_cast<char*>(v0), 3 * sizeof(float));
            file.read(reinterpret_cast<char*>(v1), 3 * sizeof(float));
            file.read(reinterpret_cast<char*>(v2), 3 * sizeof(float));
            file.ignore(2);

            tri.normal = Vector3d(normal[0], normal[1], normal[2]);
            tri.v0 = Vector3d(v0[0], v0[1], v0[2]);
            tri.v1 = Vector3d(v1[0], v1[1], v1[2]);
            tri.v2 = Vector3d(v2[0], v2[1], v2[2]);
            stlmesh.triangleList.push_back(tri);
        }

        std::cout << "Already Read " << num_triangles << " triangles from " << filename << std::endl;

        OutputStlInfo(stlmesh);
    }
};


#endif