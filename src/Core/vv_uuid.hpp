#pragma once
#include <vvpch.hpp>

namespace VectorVertex
{
    class UUID{
        public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const {return m_UUID;}

        private:
        void GenerateUUID();
        uint64_t m_UUID;
    };
} // namespace VectorVertex

namespace std
{
    template<>
    struct hash<VectorVertex::UUID>
    {
        std::size_t operator()(const VectorVertex::UUID& uuid)const
        {
            return hash<uint64_t>()((uint64_t)uuid);
        }
    };
    
} // namespace std

