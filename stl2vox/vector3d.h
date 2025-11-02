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

#ifndef __VECTOR3d_H__
#define __VECTOR3d_H__

#include <iostream>
#include <cmath>

struct Vector3d
{
    double x, y, z;
    Vector3d(double x, double y, double z) : x(x), y(y), z(z) {}
    Vector3d() : x(0), y(0), z(0) {}
    ~Vector3d() {}
    int operator[](int i) const {
        if(i == 0) return x;
        if(i == 1) return y;
        if(i == 2) return z;
        throw std::runtime_error("Index out of range");
        exit(1);
    }
    Vector3d operator-(const Vector3d& other) const {
        return Vector3d(x - other.x, y - other.y, z - other.z);
    }

    Vector3d operator+(const Vector3d& other) const {
        return Vector3d(x + other.x, y + other.y, z + other.z);
    }

    Vector3d operator*(float scalar) const {
        return Vector3d(x * scalar, y * scalar, z * scalar);
    }
    
    Vector3d operator/(float scalar) const {
        return Vector3d(x / scalar, y / scalar, z / scalar);
    }

    float dot(const Vector3d& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3d cross(const Vector3d& other) const {
        return Vector3d(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    Vector3d max(const Vector3d& other) const {
        return Vector3d(
            std::max(x, other.x),
            std::max(y, other.y),
            std::max(z, other.z)
        );
    }

    Vector3d min(const Vector3d& other) const {
        return Vector3d(
            std::min(x, other.x),
            std::min(y, other.y),
            std::min(z, other.z)
        );
    }
};

#endif