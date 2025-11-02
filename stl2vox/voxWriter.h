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
#ifndef __VOXWRITER_H__
#define __VOXWRITER_H__

#include <iostream>
#include <fstream>
#include <string>

#include "voxGrid.h"
class voxWriter
{
public:
    static void WriteVTKFile(const std::string outputfile, voxGrid &voxGrid){
        std::ofstream file(outputfile);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file");
        }

        int numVoxels = voxGrid.dim[0]*voxGrid.dim[1]*voxGrid.dim[2];
        file << "# vtk DataFile Version 3.0" << std::endl;
        file << "Voxel Grid" << std::endl;
        file << "ASCII" << std::endl;
        file << "DATASET STRUCTURED_POINTS" << std::endl;
        file << "DIMENSIONS " << voxGrid.dim[0] + 1 << " " << voxGrid.dim[1] + 1 << " " << voxGrid.dim[2] + 1 << std::endl;
        file << "SPACING " << voxGrid.spacing[0] << " " << voxGrid.spacing[1] << " " << voxGrid.spacing[2] << std::endl;
        file << "ORIGIN " << voxGrid.origin[0] << " " << voxGrid.origin[1] << " " << voxGrid.origin[2] << std::endl;
        file << "CELL_DATA " << numVoxels << std::endl;
        file << "SCALARS cell_type double" << std::endl;
        file << "LOOKUP_TABLE default" << std::endl;
        for(int i = 0; i < numVoxels; i++)
        {
            file << double(voxGrid.value[i]) << std::endl;
        }

        file.close();
    }
};

#endif