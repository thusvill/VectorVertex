#pragma once

#include <functional>

namespace VectorVertex
{
  namespace Utils
    {

    std::string ReadFile(const std::string filepath)
    {

      std::string result;
      std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
      if (in)
      {
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        if (size != -1)
        {
          result.resize(size);
          in.seekg(0, std::ios::beg);
          in.read(&result[0], size);
        }
        else
        {
          VV_CORE_ERROR("Could not read from file '{0}'", filepath);
        }
      }
      else
      {
        VV_CORE_ERROR("Could not open file '{0}'", filepath);
      }

      return result;
    }
  

  // from: https://stackoverflow.com/a/57595105
  template <typename T, typename... Rest>
  void hashCombine(std::size_t &seed, const T &v, const Rest &...rest)
  {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
  };
  }
} // namespace VectorVertex
