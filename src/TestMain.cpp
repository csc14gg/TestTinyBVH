#include <tinybvh/tiny_bvh.h>

#include <filesystem>
#include <iostream>
#include <sstream>

#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>

namespace {

std::string getMeshTrianglesFilePath() {
  std::string path = PROJECT_SOURCE_DIR;
  path += "/mesh_triangles.txt";

  return path;
}

tinybvh::bvhvec4 toTinyBvhVec4(const glm::vec3& v3) {
  constexpr float w = 0;
  tinybvh::bvhvec4 v4{v3.x, v3.y, v3.z, w};

  return v4;
}

glm::vec3 parseVector3(const std::string& vector3String) {
  std::vector<float> values;
  values.reserve(3);

  std::stringstream ss(vector3String);
  char c;
  float f;

  // Remove parenthesis
  ss >> c;
  while (ss >> f) {
    values.push_back(f);
    if (ss >> c) {
      if (c != ',') {
        break;
      }
    } else {
      break;
    }
  }

  if (values.size() == 3) {
    return glm::vec3(values[0], values[1], values[2]);
  } else {
    throw std::invalid_argument("Error: Invalid glm::vec3 string format.");
  }
}

} // namespace

int main() {
  const std::string meshTrianglesFileFullPath = getMeshTrianglesFilePath();
  std::cout << "Mesh Triangles File Path: " << meshTrianglesFileFullPath << std::endl;
  std::cout << glm::to_string(parseVector3("(1.0, 2.3, 33.0)")) << std::endl;

  return 0;
}