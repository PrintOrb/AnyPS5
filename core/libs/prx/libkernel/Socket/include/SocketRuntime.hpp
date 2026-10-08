#pragma once
#include <cstddef>
#include <cstdint>
namespace GuestSockets {
constexpr int FirstDescriptor = 0x10000000;
int Close(int descriptor);
bool IsOpen(int descriptor);
std::int64_t Read(int descriptor, void* buffer, std::size_t count);
std::int64_t Write(int descriptor, const void* buffer, std::size_t count);
}
