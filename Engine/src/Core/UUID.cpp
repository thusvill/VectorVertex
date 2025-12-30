#include "UUID.hpp"
namespace VectorVertex
{
    UUID::UUID()
    {
        GenerateUUID();
    }

    UUID::UUID(uint64_t uuid):m_UUID(uuid)
    {
    }

    void UUID::GenerateUUID()
    {
        std::random_device rd;
        std::mt19937_64 rng(rd());
        std::uniform_int_distribution<uint64_t> dist;

        m_UUID = dist(rng);
    }

} // namespace VectorVertex
