#pragma once
#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <SKSE/SKSE.h>
#include <Windows.h>
#include <bcrypt.h>
#include <nlohmann/json.hpp>
#include <spdlog/sinks/basic_file_sink.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <optional>
#include <imgui.h>
#include <imgui_internal.h>
#include <MinHook.h>
using namespace std::literals;
namespace logger = SKSE::log;
