#include "PCH.h"
#include "NativeProfiles.h"
#include "EquipmentBridge.h"
#include "FreeWear.h"
#include "MuAppearance.h"
#include "PreviewAnimations.h"

namespace {
    struct Fonts { ImFont* body; ImFont* medium; ImFont* bold; ImFont* heading; };
    using DrawSettings = bool (*)(const Fonts&, float, const void*, void*);
    DrawSettings originalDraw = nullptr;
    bool chinese = false;
    bool warnedAboutContext = false;
    using Malloc = void* (*)(std::size_t);
    using Free = void (*)(void*);
    Malloc tailorMalloc = nullptr;
    Free tailorFree = nullptr;
    void* SharedAlloc(std::size_t size, void*) { return tailorMalloc(size); }
    void SharedFree(void* pointer, void*) { tailorFree(pointer); }
    std::filesystem::path ConfigPath() { return "Data/SKSE/Plugins/TailorDDBridge.json"; }

    const char* Text(const char* english, const char* ownChinese) { return chinese ? ownChinese : english; }
    void LoadConfig() {
        try {
            std::ifstream file(ConfigPath());
            if (!file) return;
            const auto config = nlohmann::json::parse(file);
            chinese = config.value("language", "en") == "zh-CN";
            Equipment::activateOnExit = config.value("activateDDOnExit", false);
        } catch (const std::exception& e) { logger::warn("Cannot read bridge config; defaults retained: {}", e.what()); }
    }
    void SaveConfig() {
        // Only the bridge's own configuration is written; Tailor's settings are untouched.
        try {
            const auto destination = ConfigPath();
            const auto temporary = std::filesystem::path(destination.string() + ".tmp");
            std::ofstream file(temporary, std::ios::trunc);
            file << nlohmann::json{{"language", chinese ? "zh-CN" : "en"},
                {"activateDDOnExit", Equipment::activateOnExit.load()}}.dump(2);
            file.flush();
            if (!file) throw std::runtime_error("writing settings failed");
            file.close();
            if (!MoveFileExW(temporary.c_str(), destination.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
                throw std::runtime_error("replacing settings failed");
            logger::info("Bridge preference changed: activateOnExit={}", Equipment::activateOnExit.load());
        } catch (const std::exception& e) { logger::error("Bridge settings not saved: {}", e.what()); }
    }

    bool Draw(const Fonts& fonts, float scale, const void* switches, void* change) {
        // Capture the same content origin that Tailor uses before its settings page consumes the cursor.
        auto* contextBefore = fonts.body && fonts.body->OwnerAtlas ? fonts.body->OwnerAtlas->OwnerContext : nullptr;
        auto* previousBefore = ImGui::GetCurrentContext();
        ImVec2 origin{}, size{};
        const bool validBefore = contextBefore && contextBefore->WithinFrameScope && contextBefore->CurrentWindow &&
            contextBefore->IO.Fonts == fonts.body->OwnerAtlas;
        if (validBefore) {
            ImGui::SetCurrentContext(contextBefore);
            origin = ImGui::GetCursorScreenPos();
            size = ImGui::GetContentRegionAvail();
            ImGui::SetCurrentContext(previousBefore);
        }
        const bool back = originalDraw(fonts, scale, switches, change);
        if (back || !validBefore || !fonts.body || !fonts.body->OwnerAtlas) return back;
        auto* context = fonts.body->OwnerAtlas->OwnerContext;
        if (!context || !context->WithinFrameScope || context->IO.Fonts != fonts.body->OwnerAtlas || !context->CurrentWindow) {
            if (!warnedAboutContext) {
                logger::warn("Tailor context is not compatible; DD settings UI was skipped");
                warnedAboutContext = true;
            }
            return back;
        }
        auto* previous = ImGui::GetCurrentContext();
        ImGui::SetCurrentContext(context);
        const auto oldCursor = ImGui::GetCursorScreenPos();
        scale = std::clamp(scale, 0.5f, 3.0f);
        ImGui::PushID("TailorDDBridge.Settings");
        ImGui::PushFont(fonts.medium ? fonts.medium : fonts.body, 14 * scale * (4.0f / 3.0f));
        ImGui::SetCursorScreenPos({origin.x + std::max(12.0f, size.x - 180 * scale), origin.y + 64 * scale});
        const float buttonHeight = 30 * scale;
        // Tailor's inherited padding is for 40px controls. Match this button's
        // own height so that padding cannot push its centered label downward.
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
            {ImGui::GetStyle().FramePadding.x, std::max(0.0f, (buttonHeight - ImGui::GetFontSize()) * 0.5f)});
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0.5f, 0.5f});
        const bool openOptions = ImGui::Button(Text("DD Settings", "DD 设置"), {150 * scale, buttonHeight});
        ImGui::PopStyleVar(2);
        if (openOptions) ImGui::OpenPopup("DD options");
        ImGui::SetNextWindowSize({440 * scale, 0}, ImGuiCond_Always);
        if (ImGui::BeginPopup("DD options")) {
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 405 * scale);
            ImGui::TextUnformatted(Equipment::Ready() ? Text("DD fitting compatibility (experimental)", "DD 试衣兼容（实验版）") : Text("Equipment bridge unavailable. Check TailorDDBridge.log and generated data.", "装备兼容未启用，请检查 TailorDDBridge.log 与生成的装备数据。"));
            ImGui::Separator();
            bool enabled = Equipment::activateOnExit.load();
            ImGui::BeginDisabled(!Equipment::Ready());
            if (ImGui::Checkbox(Text("Keep DD clothing locks after exit", "退出后保留 DD 穿脱限制"), &enabled)) { Equipment::activateOnExit = enabled; SaveConfig(); }
            ImGui::EndDisabled();
            ImGui::Spacing();
            ImGui::TextUnformatted(Text(
                "Off: Restraint clothing can still be freely equipped and removed after exiting. On: Restraint clothing remains unchanged after exiting.",
                "关闭：退出后拘束类衣服仍可以随意穿脱，开启：退出后拘束类衣服没有变化"));
            ImGui::TextUnformatted(Text("Both modes retain genuine DD effects after confirmed exit. Off also permits clothing changes in the normal inventory.", "两种模式均保留确认退出后的 DD 原有效果。关闭时也可在普通物品栏穿脱衣服。"));
            ImGui::TextUnformatted(Text("Tailor and its optional translation remain installed separately.", "Tailor 与其汉化仍由用户另行安装。"));
            ImGui::PopTextWrapPos();
            ImGui::EndPopup();
        }
        ImGui::PopFont(); ImGui::PopID();
        ImGui::SetCursorScreenPos(oldCursor);
        ImGui::SetCurrentContext(previous);
        return back;
    }

    std::string Sha256(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) return {};
        const std::vector<unsigned char> bytes{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
        if (bytes.empty() || bytes.size() > 64 * 1024 * 1024) return {};
        std::array<unsigned char, 32> digest{};
        if (BCryptHash(BCRYPT_SHA256_ALG_HANDLE, nullptr, 0, const_cast<unsigned char*>(bytes.data()),
            static_cast<ULONG>(bytes.size()), digest.data(), static_cast<ULONG>(digest.size())) < 0) return {};
        constexpr char hex[] = "0123456789abcdef";
        std::string result;
        for (const auto byte : digest) { result.push_back(hex[byte >> 4]); result.push_back(hex[byte & 15]); }
        return result;
    }
    void Install() {
        const auto module = GetModuleHandleW(L"Tailor.dll");
        if (!module) { logger::error("Tailor.dll is not loaded; UI probe disabled"); return; }
        std::array<wchar_t, 32768> path{};
        if (!GetModuleFileNameW(module, path.data(), static_cast<DWORD>(path.size()))) return;
        const auto hash = Sha256(path.data());
        const NativeProfile* profile = nullptr;
        for (const auto& candidate : profiles) if (hash == candidate.hash) { profile = &candidate; break; }
        if (!profile) { logger::error("Unsupported Tailor DLL hash {}; no hook installed", hash); return; }
        const auto* image = reinterpret_cast<const unsigned char*>(module);
        const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
        const auto* pe = reinterpret_cast<const IMAGE_NT_HEADERS64*>(image + dos->e_lfanew);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE || pe->Signature != IMAGE_NT_SIGNATURE ||
            pe->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 || pe->OptionalHeader.SizeOfImage != profile->imageSize ||
            profile->settingsRva + profile->prologue.size() > profile->imageSize) {
            logger::error("Tailor image identity check failed; no hook installed"); return;
        }
        auto* address = const_cast<unsigned char*>(image + profile->settingsRva);
        if (!std::equal(profile->prologue.begin(), profile->prologue.end(), address)) {
            logger::error("Tailor settings entry was changed by another hook; no hook installed"); return;
        }
        // Dear ImGui requires allocator sharing across DLLs as well as context sharing.
        // These exact Tailor builds use default malloc/free (no SetAllocatorFunctions in the source).
        // Use Tailor's loaded imports, not assumptions about our own CRT or heap.
        const auto imports = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
        if (!imports) return;
        for (auto* descriptor = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(image + imports);
             descriptor->Name; ++descriptor) {
            if (!descriptor->OriginalFirstThunk) continue;
            const auto* names = reinterpret_cast<const IMAGE_THUNK_DATA64*>(image + descriptor->OriginalFirstThunk);
            const auto* bound = reinterpret_cast<const IMAGE_THUNK_DATA64*>(image + descriptor->FirstThunk);
            for (; names->u1.AddressOfData; ++names, ++bound) {
                if (IMAGE_SNAP_BY_ORDINAL64(names->u1.Ordinal)) continue;
                const auto* name = reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(image + names->u1.AddressOfData);
                if (std::string_view(name->Name) == "malloc") tailorMalloc = reinterpret_cast<Malloc>(bound->u1.Function);
                if (std::string_view(name->Name) == "free") tailorFree = reinterpret_cast<Free>(bound->u1.Function);
            }
        }
        if (!tailorMalloc || !tailorFree) { logger::error("Tailor allocator imports not found; no hook installed"); return; }
        ImGui::SetAllocatorFunctions(SharedAlloc, SharedFree, nullptr);
        const auto initialized = MH_Initialize();
        if (initialized != MH_OK && initialized != MH_ERROR_ALREADY_INITIALIZED) return;
        auto result = MH_CreateHook(address, reinterpret_cast<void*>(&Draw), reinterpret_cast<void**>(&originalDraw));
        if (result != MH_OK) { logger::error("Settings hook creation failed: {}", MH_StatusToString(result)); return; }
        result = MH_EnableHook(address);
        if (result != MH_OK) { MH_RemoveHook(address); originalDraw = nullptr; logger::error("Settings hook activation failed: {}", MH_StatusToString(result)); return; }
        logger::info("Independent settings UI hook enabled for {} at RVA {:X}", profile->name, profile->settingsRva);
        Equipment::Install(reinterpret_cast<std::uintptr_t>(module), static_cast<std::size_t>(profile - profiles));
    }
    void OnMessage(SKSE::MessagingInterface::Message* message) {
        if (!message) return;
        if (message->type == SKSE::MessagingInterface::kPostLoad) PreviewAnimations::Init();
        if (message->type == SKSE::MessagingInterface::kDataLoaded) { LoadConfig(); Install(); }
        if (message->type == SKSE::MessagingInterface::kPreLoadGame || message->type == SKSE::MessagingInterface::kNewGame) Equipment::Clear();
        if (message->type == SKSE::MessagingInterface::kNewGame) MuAppearance::Clear(true);
        if (message->type == SKSE::MessagingInterface::kPostLoadGame || message->type == SKSE::MessagingInterface::kNewGame) Equipment::AfterLoad();
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    auto path = SKSE::log::log_directory();
    if (!path) return false;
    *path /= "TailorDDBridge.log";
    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
    spdlog::set_default_logger(std::make_shared<spdlog::logger>("TailorDDBridge", sink));
    spdlog::set_level(spdlog::level::info); spdlog::flush_on(spdlog::level::info);
    logger::info("Tailor DD bridge 0.6.14 local test; scoped PAR stale-pose revalidation; retained Pet PickNewIdle source guard; runtime verification pending");
    SKSE::GetPapyrusInterface()->Register(FreeWear::RegisterPapyrus);
    SKSE::GetPapyrusInterface()->Register(MuAppearance::RegisterPapyrus);
    Equipment::RegisterSerialization();
    return SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
}
