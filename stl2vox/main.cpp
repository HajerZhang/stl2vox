#include "stlReader.h"
#include "stl2vox.h"
#include "voxWriter.h"

#include <string>

std::string replaceExtension(const std::string& path, const std::string& newExt) {
    size_t slashPos = path.find_last_of("/\\");
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos || (slashPos != std::string::npos && dotPos < slashPos)) {
        return path + newExt;
    }
    return path.substr(0, dotPos) + newExt;
}

int main(int argc, char** argv)
{
    if(argc < 2){
        std::cout << "Usage: " << argv[0] << " <stl file>" << std::endl;
        return 1;
    }

    STLMesh mesh;
    stlReader::ReadStlFile(argv[1], mesh);

    voxGrid voxel;
    stl2vox::Convert(mesh, voxel);

    std::string outputFile = replaceExtension(argv[1], ".vtk");
    voxWriter::WriteVTKFile(outputFile, voxel);
}