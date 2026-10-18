#pragma once
#include <array>
#include <cstdint>
struct NativeProfile { const char* name; const char* hash; std::uint32_t imageSize, settingsRva; std::array<unsigned char, 32> prologue; };
inline constexpr NativeProfile profiles[] = {
    {"Tailor 3.0.1 English","261f415773d550e16809cbdfa39863451ea3eeba247e30b996afabe1e1d47850",3559424,919888,{72,139,196,72,137,88,32,85,86,87,65,84,65,85,65,86,65,87,72,141,168,152,254,255,255,72,129,236,48,2,0,0}},
    {"Tailor 3.0.1 local CHS","83dc9c56627d163e00eb9e5f1cceb1af3edb84deaa286ec828c95fb2a0d0df4d",3571712,923184,{72,139,196,72,137,88,32,85,86,87,65,84,65,85,65,86,65,87,72,141,168,152,254,255,255,72,129,236,48,2,0,0}},
};
