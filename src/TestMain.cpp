#include <tinybvh/tiny_bvh.h>

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <unordered_set>
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

  uint32_t numTriangles() const {
    assert(indices.empty() || indices.size() % 3 == 0);
    return static_cast<uint32_t>(indices.size() / 3);
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

void calculateAabb(glm::vec3& aabbMin, glm::vec3& aabbMax, const MeshData& meshData) {
  if (meshData.numTriangles() == 0) {
    throw std::invalid_argument("Error: no triangles in `meshData`.");
  }

  const std::vector<tinybvh::bvhvec4>& vertices = meshData.vertices;
  const std::vector<uint32_t>& indices = meshData.indices;

  const std::unordered_set<uint32_t> uniqueIndices{
      std::begin(indices),
      std::end(indices)};
  assert(!uniqueIndices.empty());

  aabbMin = glm::vec3{std::numeric_limits<float>::max()};
  aabbMax = glm::vec3{std::numeric_limits<float>::lowest()};

  for (int iVertex = 0; iVertex < vertices.size(); ++iVertex) {
    // Ignore unused vertices
    if (uniqueIndices.find(iVertex) == std::end(uniqueIndices)) {
      continue;
    }

    const tinybvh::bvhvec4& vertex = vertices[iVertex];
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

bool nearlyEqual(const glm::vec3& lhs, const glm::vec3& rhs, const float epsilon) {
  const glm::bvec3 equalTestResult = glm::epsilonEqual(lhs, rhs, epsilon);
  const bool allNearlyEqual = glm::all(equalTestResult);

  return allNearlyEqual;
}

constexpr float FLOAT_EPSILON = std::numeric_limits<float>::epsilon();

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
  calculateAabb(aabbMin, aabbMax, meshData);

  tinybvh::BVH bvh;
  bvh.Build(meshData.vertices.data(), meshData.indices.data(), meshData.numTriangles());
  const glm::vec3 bvhAabbMin = toGlmVec3(bvh.aabbMin);
  const glm::vec3 bvhAabbMax = toGlmVec3(bvh.aabbMax);

  if (!nearlyEqual(aabbMin, bvhAabbMin, FLOAT_EPSILON)) {
    std::cerr << "AABB mins do not match: " << glm::to_string(aabbMin)
        << " VERSUS " << glm::to_string(bvhAabbMin) << std::endl;
  }

  if (!nearlyEqual(aabbMax, bvhAabbMax, FLOAT_EPSILON)) {
    std::cerr << "AABB maxs do not match: " << glm::to_string(aabbMax)
      << " VERSUS " << glm::to_string(bvhAabbMax) << std::endl;
  }

  return 0;
}