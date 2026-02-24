#pragma once
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <cstdint>

// --- TYPE REGISTRY ALIASES ---

#define DEFINE_TYPE_GROUP(Scalar, Vec, Map, UMap) \
    using Vec = std::vector<Scalar>; \
    using Map = std::map<std::string, Scalar>; \
    using UMap = std::unordered_map<std::string, Scalar>;

// Define Aliases for every type set (char removed to prevent string_view conversion errors)
DEFINE_TYPE_GROUP(bool,         BoolVec,   BoolMap,   BoolUMap)
DEFINE_TYPE_GROUP(float,        FloatVec,  FloatMap,  FloatUMap)
DEFINE_TYPE_GROUP(double,       DoubleVec, DoubleMap, DoubleUMap)
DEFINE_TYPE_GROUP(std::string,  StringVec, StringMap, StringUMap)

DEFINE_TYPE_GROUP(int8_t,       Int8Vec,   Int8Map,   Int8UMap)
DEFINE_TYPE_GROUP(int16_t,      Int16Vec,  Int16Map,  Int16UMap)
DEFINE_TYPE_GROUP(int32_t,      Int32Vec,  Int32Map,  Int32UMap)
DEFINE_TYPE_GROUP(int64_t,      Int64Vec,  Int64Map,  Int64UMap)

DEFINE_TYPE_GROUP(uint8_t,      UInt8Vec,  UInt8Map,  UInt8UMap)
DEFINE_TYPE_GROUP(uint16_t,     UInt16Vec, UInt16Map, UInt16UMap)
DEFINE_TYPE_GROUP(uint32_t,     UInt32Vec, UInt32Map, UInt32UMap)
DEFINE_TYPE_GROUP(uint64_t,     UInt64Vec, UInt64Map, UInt64UMap)

// --- MASTER VISITOR MACRO ---

#define PROPERTIES_SUPPORTED_TYPES(Visitor) \
    Visitor(bool)          Visitor(BoolVec)   Visitor(BoolMap)   Visitor(BoolUMap)   \
    Visitor(float)         Visitor(FloatVec)  Visitor(FloatMap)  Visitor(FloatUMap)  \
    Visitor(double)        Visitor(DoubleVec) Visitor(DoubleMap) Visitor(DoubleUMap) \
    Visitor(std::string)   Visitor(StringVec) Visitor(StringMap) Visitor(StringUMap) \
    Visitor(int8_t)        Visitor(Int8Vec)   Visitor(Int8Map)   Visitor(Int8UMap)   \
    Visitor(int16_t)       Visitor(Int16Vec)  Visitor(Int16Map)  Visitor(Int16UMap)  \
    Visitor(int32_t)       Visitor(Int32Vec)  Visitor(Int32Map)  Visitor(Int32UMap)  \
    Visitor(int64_t)       Visitor(Int64Vec)  Visitor(Int64Map)  Visitor(Int64UMap)  \
    Visitor(uint8_t)       Visitor(UInt8Vec)  Visitor(UInt8Map)  Visitor(UInt8UMap)  \
    Visitor(uint16_t)      Visitor(UInt16Vec) Visitor(UInt16Map) Visitor(UInt16UMap) \
    Visitor(uint32_t)      Visitor(UInt32Vec) Visitor(UInt32Map) Visitor(UInt32UMap) \
    Visitor(uint64_t)      Visitor(UInt64Vec) Visitor(UInt64Map) Visitor(UInt64UMap)

