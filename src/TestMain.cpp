#include <tinybvh/tiny_bvh.h>

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/string_cast.hpp>

namespace TestBVH {
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

glm::vec3 toGlmVec3(const tinybvh::bvhvec3& v3) {
  return {v3.x, v3.y, v3.z};
}

glm::vec3 parseVec3(const std::string& vector3String) {
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

struct MeshData {
  std::vector<tinybvh::bvhvec4> vertices;
  std::vector<unsigned int> indices;

  size_t numTriangles() const {
    assert(indices.empty() || indices.size() % 3 == 0);
    return (indices.size() / 3);
  }
};

MeshData loadMeshData(const std::string& filename) {
  MeshData mesh;
  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Error: Could not open file " << filename << std::endl;
    return mesh;
  }

  // Parse vertices
  std::string line;
  std::getline(file, line); // Skip "Vertices:"
  while (std::getline(file, line)) {
    if (line == "Indices:") {
      break;
    }

    if (line.empty()) {
      continue;
    }

    glm::vec3 vertex = parseVec3(line);
    mesh.vertices.push_back(toTinyBvhVec4(vertex));
  }

  // Parse indices
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    unsigned int index;
    char comma;

    while (ss >> index) {
      mesh.indices.push_back(index);
      if (ss >> comma && comma != ',') {
        break; // Stop if not a comma
      }
    }
  }

  file.close();
  return mesh;
}

bool isValidMesh(const MeshData& meshData) {
  const size_t numVertices = meshData.vertices.size();
  for (const size_t iVertex : meshData.indices) {
    if (iVertex >= numVertices) {
      return false;
    }
  }

  return true;
}

void calculateAabbFromVertices(
    glm::vec3& aabbMin,
    glm::vec3& aabbMax,
    const std::vector<tinybvh::bvhvec4>& vertices) {
  aabbMin = glm::vec3{std::numeric_limits<float>::max()};
  aabbMax = glm::vec3{std::numeric_limits<float>::lowest()};

  for (const tinybvh::bvhvec4& vertex : vertices) {
    for (int component = 0; component < 3; ++component) {
      const float vi = vertex.cell[component];

      float& aabbMinI = aabbMin[component];
      if (vi < aabbMinI) {
        aabbMinI = vi;
      }

      float& aabbMaxI = aabbMax[component];
      if (vi > aabbMaxI) {
        aabbMaxI = vi;
      }
    }
  }
}

constexpr float getFloatEpsilon() {
  return std::numeric_limits<float>::epsilon();
}

} // namespace
} // namespace TestBVH

int main() {
  using namespace TestBVH;

  const std::string meshTrianglesFileFullPath = getMeshTrianglesFilePath();
  std::cout << "File: " << meshTrianglesFileFullPath << std::endl;
  const MeshData meshData = loadMeshData(meshTrianglesFileFullPath);

  if (!isValidMesh(meshData)) {
    std::cerr << "ERROR: invalid mesh data." << std::endl;
    return -100;
  }

  glm::vec3 aabbMin, aabbMax;
  calculateAabbFromVertices(aabbMin, aabbMax, meshData.vertices);

  tinybvh::BVH bvh;
  bvh.Build(meshData.vertices.data(), meshData.indices.data(), meshData.numTriangles());
  const glm::vec3 bvhAabbMin = toGlmVec3(bvh.aabbMin);
  const glm::vec3 bvhAabbMax = toGlmVec3(bvh.aabbMax);

  if (!glm::all(glm::epsilonEqual(aabbMin, bvhAabbMin, getFloatEpsilon()))) {
    std::cerr << "AABB mins do not match: " << std::endl;
  }

  if (!glm::all(glm::epsilonEqual(aabbMax, bvhAabbMax, getFloatEpsilon()))) {
    std::cerr << "AABB maxs do not match: " << std::endl;
  }

  return 0;
}