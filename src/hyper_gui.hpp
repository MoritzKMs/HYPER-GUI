#pragma once
#include <array>
#include <string>
#include "imgui.h"
namespace hypergui {
enum class Language { English, Turkish };
struct FeatureOptions {
    float color[4]{0.25f, 0.60f, 1.f, 1.f};
    float strength = 50.f, detail = 1.5f;
    int style = 0, key = 0, activation = 0;
    bool extra = true;
};
struct Page {
    std::array<FeatureOptions, 6> features{};
    std::array<bool, 6> toggles{true, false, true, false, false, false};
    float amount = 35.f;
    float secondary = 65.f;
    int mode = 0;
};
struct Settings {
    std::array<Page, 7> pages{};
    float accent[3]{0.16f, 0.49f, 1.f};
};
struct State {
    std::string steamName;
    ImTextureID steamAvatar = 0, cs2Icon = 0; // Optional host-owned textures.
    Language language = Language::English;
    int tab = 3, profile = 0;
    Settings settings{};
    std::array<Settings, 3> profiles{};
    std::array<bool, 3> saved{};
    int notice = 0;
};
// Call between ImGui::NewFrame and ImGui::Render, on the host render thread.
// The host owns the ImGui context, fonts, platform/renderer backends and State.
void Render(State &state);
} // namespace hypergui
