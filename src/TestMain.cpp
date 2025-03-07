#include <filesystem>
#include <iostream>

#include <glm/vec3.hpp>

namespace {

std::string getMeshTrianglesFilePath() {
  std::string path = PROJECT_SOURCE_DIR;
  path += "/mesh_triangles.txt";

  return path;
}



} // namespace

int main() {
  const std::string meshTrianglesFileFullPath = getMeshTrianglesFilePath();
  std::cout << "Mesh Triangles File Path: " << meshTrianglesFileFullPath << std::endl;

  return 0;
}