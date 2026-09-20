#include "hyper_gui.hpp"
#include "imgui.h"
#include <cstdio>
#include <string>
namespace hypergui {
namespace {
const char *tr(const State &s, const char *en, const char *tur) {
    return s.language == Language::Turkish ? tur : en;
}
const char *names[] = {"Legitbot", "Ragebot", "Anti-Aim", "Visuals", "World", "Skins", "Misc", "Config"};
const char *namesTR[] = {"Legitbot", "Ragebot",    "Anti-Aim", "Görseller",
                         "Dünya",    "Kaplamalar", "Diğer",    "Yapılandırma"};
struct Labels {
    const char *en[6];
    const char *tur[6];
    const char *a;
    const char *at;
    const char *b;
    const char *bt;
};
const Labels labels[] = {
    {{"Enable legitbot", "Visibility check", "Recoil control", "Target teammates", "Aim smoothing",
      "Triggerbot"},
     {"Legitbot etkin", "Görünürlük kontrolü", "Geri tepme kontrolü", "Takım arkadaşı hedefi",
      "Yumuşak nişan", "Tetik botu"},
     "Field of view",
     "Görüş açısı",
     "Smoothing",
     "Yumuşatma"},
    {{"Enable ragebot", "Automatic fire", "Automatic scope", "Multipoint", "Prefer safe points",
      "Damage override"},
     {"Ragebot etkin", "Otomatik ateş", "Otomatik dürbün", "Çoklu nokta", "Güvenli nokta tercihi",
      "Hasar geçersiz kılma"},
     "Minimum damage",
     "Minimum hasar",
     "Hit chance",
     "İsabet olasılığı"},
    {{"Enable anti-aim", "Pitch override", "Yaw override", "Manual direction", "Freestanding", "Jitter"},
     {"Anti-aim etkin", "Dikey açı", "Yatay açı", "Manuel yön", "Serbest yön", "Titreşim"},
     "Yaw offset",
     "Yatay açı farkı",
     "Jitter range",
     "Titreşim aralığı"},
    {{"Player boxes", "Player names", "Health bars", "Skeleton", "Weapon labels", "Offscreen arrows"},
     {"Oyuncu kutuları", "Oyuncu isimleri", "Can çubukları", "İskelet", "Silah etiketleri",
      "Ekran dışı oklar"},
     "Render distance",
     "Çizim mesafesi",
     "Opacity",
     "Opaklık"},
    {{"Night mode", "Custom sky", "Remove fog", "World tint", "Full bright", "Ambient particles"},
     {"Gece modu", "Özel gökyüzü", "Sisi kaldır", "Dünya rengi", "Tam aydınlık", "Ortam parçacıkları"},
     "Brightness",
     "Parlaklık",
     "Exposure",
     "Pozlama"},
    {{"Preview skin", "Custom knife", "Custom gloves", "StatTrak label", "Custom name", "Pearlescent finish"},
     {"Kaplama önizlemesi", "Özel bıçak", "Özel eldiven", "StatTrak etiketi", "Özel isim", "Sedefli yüzey"},
     "Wear",
     "Aşınma",
     "Pattern seed",
     "Desen değeri"},
    {{"Bunny hop", "Auto strafe", "Watermark", "Spectator list", "Hit marker", "Event log"},
     {"Otomatik zıplama", "Otomatik yön", "Filigran", "İzleyici listesi", "İsabet işareti", "Olay kaydı"},
     "Viewmodel FOV",
     "Model görüş açısı",
     "Menu opacity",
     "Menü opaklığı"}};

constexpr ImU32 ink = IM_COL32(224, 231, 241, 255), muted = IM_COL32(135, 140, 155, 255),
                edge = IM_COL32(48, 49, 58, 255);
ImVec2 origin;
ImVec2 pos(float x, float y) {
    return ImVec2(origin.x + x, origin.y + y);
}
void text(float x, float y, const char *value, float size = 15, ImU32 color = ink) {
    ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), size, pos(x, y), color, value);
}
void rect(float x, float y, float w, float h, ImU32 color, float r = 7) {
    ImGui::GetWindowDrawList()->AddRectFilled(pos(x, y), pos(x + w, y + h), color, r);
}
void line(float x, float y, float x2, float y2, ImU32 color = edge, float thickness = 1) {
    ImGui::GetWindowDrawList()->AddLine(pos(x, y), pos(x2, y2), color, thickness);
}
ImU32 accent(const State &s, int alpha = 255) {
    return IM_COL32(int(s.settings.accent[0] * 255), int(s.settings.accent[1] * 255),
                    int(s.settings.accent[2] * 255), alpha);
}
bool hit(const char *id, float x, float y, float w, float h) {
    ImGui::SetCursorPos(ImVec2(x, y));
    return ImGui::InvisibleButton(id, ImVec2(w, h));
}
void icon(float x, float y, int type, ImU32 col) {
    auto *d = ImGui::GetWindowDrawList();
    if (type < 3) {
        d->AddCircle(pos(x + 8, y + 8), 6, col, 20, 1.4f);
        line(x + 8, y - 2, x + 8, y + 3, col);
        line(x + 8, y + 13, x + 8, y + 18, col);
        line(x - 2, y + 8, x + 3, y + 8, col);
        line(x + 13, y + 8, x + 18, y + 8, col);
        if (type == 1)
            d->AddCircleFilled(pos(x + 8, y + 8), 2, col);
        if (type == 2)
            line(x + 3, y + 13, x + 13, y + 3, col);
    } else if (type == 3) {
        d->AddQuad(pos(x, y + 8), pos(x + 8, y + 2), pos(x + 17, y + 8), pos(x + 8, y + 14), col, 1.4f);
        d->AddCircle(pos(x + 8, y + 8), 3, col, 16, 1.3f);
    } else if (type == 4) {
        d->AddCircle(pos(x + 8, y + 8), 8, col, 24, 1.3f);
        line(x, y + 8, x + 16, y + 8, col);
        d->AddEllipse(pos(x + 8, y + 8), ImVec2(3, 8), col, 0, 20, 1);
    } else if (type == 5) {
        d->AddQuad(pos(x + 8, y), pos(x + 17, y + 8), pos(x + 8, y + 17), pos(x, y + 8), col, 1.5f);
        line(x + 3, y + 12, x + 12, y + 3, col);
    } else if (type == 6) {
        for (int i = 0; i < 3; i++) {
            line(x, y + 3 + i * 6, x + 17, y + 3 + i * 6, col);
            d->AddCircleFilled(pos(x + (i == 1 ? 12 : 5), y + 3 + i * 6), 2.5f, col);
        }
    } else {
        d->AddRect(pos(x + 1, y + 1), pos(x + 16, y + 16), col, 3, 0, 1.4f);
        line(x + 5, y + 6, x + 12, y + 6, col);
        line(x + 5, y + 11, x + 10, y + 11, col);
    }
}
void card(float x, float y, float w, float h, const char *name, int ico, const State &s) {
    rect(x, y + 3, w, h, IM_COL32(8, 8, 12, 45), 7);
    rect(x, y, w, h, IM_COL32(29, 29, 35, 255), 7);
    auto *d = ImGui::GetWindowDrawList();
    d->AddRect(pos(x, y), pos(x + w, y + h), IM_COL32(46, 46, 55, 255), 7);
    rect(x + 1, y + 1, w - 2, 35, IM_COL32(36, 36, 44, 255), 6);
    line(x + 1, y + 36, x + w - 1, y + 36, IM_COL32(45, 45, 54, 255));
    icon(x + 12, y + 10, ico, accent(s));
    text(x + 38, y + 10, name, 16);
}

void featurePopup(State &s, int id, const char *title) {
    auto &page = s.settings.pages[s.tab];
    auto &f = page.features[id];
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14, 13));
    ImGui::SetNextWindowSizeConstraints(ImVec2(288, 0), ImVec2(288, 480));
    if (ImGui::BeginPopupContextItem("options", ImGuiPopupFlags_MouseButtonRight)) {
        ImGui::TextColored(ImVec4(s.settings.accent[0], s.settings.accent[1], s.settings.accent[2], 1), "%s",
                           title);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        auto select = [&](const char *en, const char *tur, int &v, const char *opts) {
            ImGui::TextUnformatted(tr(s, en, tur));
            ImGui::SetNextItemWidth(-1);
            ImGui::Combo((std::string("##") + en).c_str(), &v, opts);
            ImGui::Spacing();
        };
        auto number = [&](const char *en, const char *tur, float &v, float lo, float hi, const char *format) {
            ImGui::TextUnformatted(tr(s, en, tur));
            ImGui::SetNextItemWidth(-1);
            ImGui::SliderFloat((std::string("##") + en).c_str(), &v, lo, hi, format,
                               ImGuiSliderFlags_AlwaysClamp);
            ImGui::Spacing();
        };
        if (s.tab == 3) {
            if (id == 0) {
                select("Box type", "Kutu tipi", page.mode,
                       s.language == Language::English ? "Full box\0Corner box\0Minimal\0"
                                                       : "Tam kutu\0Köşe kutusu\0Minimal\0");
                number("Thickness", "Kalınlık", f.detail, 1, 4, "%.1f px");
                ImGui::Checkbox(tr(s, "Soft fill", "Hafif dolgu"), &f.extra);
            }
            if (id == 1 || id == 4) {
                select("Position", "Konum", f.style,
                       s.language == Language::English ? "Above\0Below\0Right\0" : "Üst\0Alt\0Sağ\0");
                number("Text scale", "Yazı ölçeği", f.detail, 1, 2, "%.1fx");
                ImGui::Checkbox(tr(s, "Text shadow", "Yazı gölgesi"), &f.extra);
            }
            if (id == 2) {
                select("Side", "Taraf", f.style,
                       s.language == Language::English ? "Left\0Right\0" : "Sol\0Sağ\0");
                number("Bar width", "Çubuk genişliği", f.detail, 1, 6, "%.1f px");
                ImGui::Checkbox(tr(s, "Show value", "Değeri göster"), &f.extra);
            }
            if (id == 3) {
                number("Line thickness", "Çizgi kalınlığı", f.detail, 1, 4, "%.1f px");
                ImGui::Checkbox(tr(s, "Joint markers", "Eklem noktaları"), &f.extra);
            }
            if (id == 5) {
                number("Arrow size", "Ok boyutu", f.detail, 1, 3, "%.1fx");
                ImGui::Checkbox(tr(s, "Outline", "Dış çizgi"), &f.extra);
            }
            ImGui::Spacing();
            ImGui::TextUnformatted(tr(s, "Color", "Renk"));
            ImGui::SetNextItemWidth(-1);
            ImGui::ColorEdit4("##color", f.color,
                              ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
        } else if (s.tab < 3) {
            const char *aEN[3][6] = {
                {"Aim strength", "Check interval", "Recoil amount", "Priority", "Smoothing amount", "Delay"},
                {"Target priority", "Fire delay", "Scope delay", "Point scale", "Safety threshold",
                 "Override damage"},
                {"Rotation speed", "Pitch angle", "Yaw angle", "Direction offset", "Edge distance",
                 "Jitter amplitude"}};
            const char *aTR[3][6] = {
                {"Nişan gücü", "Kontrol aralığı", "Geri tepme oranı", "Öncelik", "Yumuşatma oranı",
                 "Gecikme"},
                {"Hedef önceliği", "Ateş gecikmesi", "Dürbün gecikmesi", "Nokta ölçeği", "Güvenlik eşiği",
                 "Özel hasar"},
                {"Dönüş hızı", "Dikey açı", "Yatay açı", "Yön farkı", "Kenar mesafesi", "Titreşim genliği"}};
            select("Activation", "Etkinleştirme", f.activation,
                   s.language == Language::English ? "Always\0Hold\0Toggle\0"
                                                   : "Her zaman\0Basılı tut\0Aç / kapat\0");
            ImGui::BeginDisabled(f.activation == 0);
            select("Key", "Tuş", f.key, "None\0Mouse 4\0Mouse 5\0Shift\0Alt\0Ctrl\0");
            ImGui::EndDisabled();
            number(aEN[s.tab][id], aTR[s.tab][id], f.strength, 0, 100, "%.0f");
            select("Behavior", "Davranış", f.style,
                   s.language == Language::English ? "Default\0Adaptive\0Smooth\0"
                                                   : "Varsayılan\0Uyarlanır\0Yumuşak\0");
        } else {
            const char *aEN[3][6] = {
                {"Darkness", "Sky intensity", "Fog distance", "Tint strength", "Light intensity",
                 "Particle density"},
                {"Wear", "Blade finish", "Glove wear", "Counter value", "Label scale", "Pearlescence"},
                {"Jump chance", "Strafe strength", "Opacity", "List opacity", "Marker size",
                 "Display duration"}};
            const char *aTR[3][6] = {{"Karanlık", "Gökyüzü yoğunluğu", "Sis mesafesi", "Renk yoğunluğu",
                                      "Işık yoğunluğu", "Parçacık yoğunluğu"},
                                     {"Aşınma", "Bıçak yüzeyi", "Eldiven aşınması", "Sayaç değeri",
                                      "Etiket ölçeği", "Sedef oranı"},
                                     {"Zıplama oranı", "Yön gücü", "Opaklık", "Liste opaklığı",
                                      "İşaret boyutu", "Gösterim süresi"}};
            number(aEN[s.tab - 4][id], aTR[s.tab - 4][id], f.strength, 0, 100, "%.0f");
            select("Style", "Biçim", f.style,
                   s.language == Language::English ? "Default\0Minimal\0Detailed\0"
                                                   : "Varsayılan\0Minimal\0Ayrıntılı\0");
            ImGui::TextUnformatted(tr(s, "Color", "Renk"));
            ImGui::ColorEdit4("##color", f.color,
                              ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
        }
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button(tr(s, "Reset this feature", "Bu özelliği sıfırla"), ImVec2(-1, 27))) {
            f = FeatureOptions{};
            if (s.tab == 3 && id == 0)
                page.mode = 0;
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}
void toggle(State &s, const char *label, bool &value, int id, float x, float y, float w) {
    ImGui::PushID(id);
    if (hit("toggle", x, y, w, 36))
        value = !value;
    if (ImGui::IsItemHovered())
        rect(x - 6, y, w + 12, 36, IM_COL32(255, 255, 255, 5), 5);
    text(x, y + 10, label, 14, value ? ink : muted);
    float tx = x + w - 33;
    rect(tx, y + 10, 33, 18, value ? accent(s, 180) : IM_COL32(58, 72, 89, 255), 9);
    ImGui::GetWindowDrawList()->AddCircleFilled(pos(tx + (value ? 24 : 9), y + 19), 6,
                                                value ? IM_COL32(228, 246, 255, 255)
                                                      : IM_COL32(130, 145, 162, 255));
    featurePopup(s, id, label);
    ImGui::PopID();
}
void slider(const char *id, const char *label, float &value, float x, float y, float w) {
    text(x, y, label, 14, muted);
    ImGui::SetCursorPos(ImVec2(x, y + 25));
    ImGui::SetNextItemWidth(w);
    ImGui::SliderFloat(id, &value, 0, 100, "%.0f", ImGuiSliderFlags_AlwaysClamp);
}
void combo(const char *id, const char *label, int &value, const char *choices, float x, float y, float w) {
    text(x, y, label, 14, muted);
    ImGui::SetCursorPos(ImVec2(x, y + 25));
    ImGui::SetNextItemWidth(w);
    ImGui::Combo(id, &value, choices);
}
void soldier(State &s, float x, float y, float w, float h) {
    auto *d = ImGui::GetWindowDrawList();
    rect(x, y, w, h, IM_COL32(22, 32, 43, 255), 7);
    d->AddRectFilledMultiColor(pos(x, y), pos(x + w, y + h), IM_COL32(40, 60, 72, 130),
                               IM_COL32(26, 40, 54, 80), IM_COL32(20, 28, 38, 0), IM_COL32(27, 42, 54, 60));
    for (int i = 1; i < 9; i++) {
        line(x + i * w / 9, y, x + i * w / 9, y + h, IM_COL32(84, 107, 128, 15));
        line(x, y + i * h / 9, x + w, y + i * h / 9, IM_COL32(84, 107, 128, 15));
    }
    float cx = x + w * .5f, top = y + 48;
    d->AddEllipse(pos(cx, y + h - 35), ImVec2(65, 13), IM_COL32(8, 16, 24, 130), 0, 40, 8);
    // Original vector illustration of a tactical mannequin, not a game asset.
    auto poly = [&](std::initializer_list<ImVec2> points, ImU32 color) {
        ImVec2 v[20];
        int n = 0;
        for (auto a : points)
            v[n++] = pos(cx + a.x, top + a.y);
        d->AddConvexPolyFilled(v, n, color);
    };
    ImU32 cloth = IM_COL32(80, 99, 112, 255), dark = IM_COL32(43, 58, 72, 255),
          light = IM_COL32(109, 130, 141, 255);
    poly({{-26, 124}, {-1, 126}, {-5, 182}, {-15, 226}, {-34, 223}, {-29, 177}}, cloth);
    poly({{3, 127}, {28, 124}, {29, 181}, {40, 222}, {20, 226}, {7, 184}}, cloth);
    poly({{-34, 214}, {-14, 217}, {-12, 234}, {-41, 234}, {-42, 228}}, dark);
    poly({{20, 217}, {39, 215}, {48, 230}, {46, 235}, {18, 233}}, dark);
    poly({{-31, 48}, {-14, 36}, {16, 36}, {34, 51}, {27, 131}, {-26, 131}}, cloth);
    poly({{-22, 49}, {21, 49}, {25, 117}, {-25, 117}}, dark);
    rect(cx - 17, top + 62, 34, 34, IM_COL32(62, 79, 89, 255), 3);
    for (int i = 0; i < 3; i++)
        rect(cx - 19 + i * 14, top + 101, 11, 21, IM_COL32(93, 109, 112, 255), 2);
    line(cx - 19, top + 52, cx - 16, top + 98, light, 3);
    line(cx + 19, top + 52, cx + 16, top + 98, light, 3);
    poly({{-29, 45}, {-41, 54}, {-54, 93}, {-34, 104}, {-22, 79}}, cloth);
    poly({{30, 45}, {43, 59}, {51, 93}, {33, 103}, {23, 72}}, cloth);
    poly({{-54, 92}, {-36, 88}, {-4, 101}, {-8, 114}, {-36, 111}}, light);
    poly({{35, 88}, {51, 89}, {24, 117}, {9, 109}}, light);
    d->AddEllipse(pos(cx, top + 18), ImVec2(17, 23), dark, 0, 24, 17);
    rect(cx - 20, top - 3, 40, 24, cloth, 10);
    rect(cx - 18, top + 16, 36, 12, IM_COL32(13, 24, 32, 255), 4);
    line(cx - 13, top + 18, cx + 9, top + 18, IM_COL32(114, 154, 167, 255), 2);
    // Simplified rifle silhouette.
    rect(cx - 24, top + 89, 68, 10, IM_COL32(18, 25, 33, 255), 2);
    rect(cx + 40, top + 91, 23, 4, IM_COL32(15, 23, 30, 255), 1);
    poly({{-5, 96}, {8, 96}, {15, 119}, {4, 124}}, dark);
    line(cx - 32, top + 84, cx - 15, top + 96, dark, 8);
    auto &v = s.settings.pages[3];

    float bx = cx - 64, by = top - 13, bw = 128, bh = 253;
    auto col = [&](int id, float opacity = 1.f) {
        const auto &f = v.features[id];
        return ImGui::ColorConvertFloat4ToU32(
            ImVec4(f.color[0], f.color[1], f.color[2], f.color[3] * opacity * v.secondary / 100.f));
    };
    auto &box = v.features[0];
    if (v.toggles[0]) {
        if (box.extra)
            rect(bx, by, bw, bh, col(0, .07f), 2);
        if (v.mode == 0)
            d->AddRect(pos(bx, by), pos(bx + bw, by + bh), col(0), 2, 0, box.detail);
        else {
            float k = v.mode == 1 ? 18.f : 7.f;
            for (int j = 0; j < 2; j++)
                for (int i = 0; i < 2; i++) {
                    float xx = bx + i * bw, yy = by + j * bh;
                    line(xx, yy, xx + (i ? -k : k), yy, col(0), box.detail);
                    line(xx, yy, xx, yy + (j ? -k : k), col(0), box.detail);
                }
        }
    }
    auto label = [&](int id, const char *str) {
        auto &f = v.features[id];
        float size = 10 * f.detail;
        float xx =
            f.style == 2 ? bx + bw + 4 : cx - ImGui::GetFont()->CalcTextSizeA(size, 1000, 0, str).x / 2;
        float yy = f.style == 0 ? by - size - 4 : f.style == 1 ? by + bh + 5 : by + 20;
        if (f.extra)
            text(xx + 1, yy + 1, str, size, IM_COL32(0, 0, 0, 200));
        text(xx, yy, str, size, col(id));
    };
    if (v.toggles[1])
        label(1, "PLAYER 01");
    if (v.toggles[2]) {
        auto &f = v.features[2];
        float hx = f.style == 0 ? bx - 5 - f.detail : bx + bw + 5;
        rect(hx, by, f.detail, bh, IM_COL32(39, 55, 62, 255), 1);
        rect(hx, by + bh * .18f, f.detail, bh * .82f, col(2), 1);
        if (f.extra)
            text(hx - 16, by + 36, "82", 11, col(2));
    }
    if (v.toggles[3]) {
        auto &f = v.features[3];
        ImU32 sk = col(3);
        line(cx, top + 25, cx, top + 128, sk, f.detail);
        line(cx - 30, top + 58, cx + 30, top + 58, sk, f.detail);
        line(cx - 30, top + 58, cx - 43, top + 98, sk, f.detail);
        line(cx + 30, top + 58, cx + 40, top + 98, sk, f.detail);
        line(cx, top + 128, cx - 22, top + 223, sk, f.detail);
        line(cx, top + 128, cx + 29, top + 223, sk, f.detail);
        if (f.extra) {
            d->AddCircleFilled(pos(cx, top + 58), 3, sk);
            d->AddCircleFilled(pos(cx, top + 128), 3, sk);
        }
    }
    if (v.toggles[4])
        label(4, "AK-47");
    if (v.toggles[5]) {
        auto &f = v.features[5];
        float ar = 4 * f.detail;
        ImVec2 a = pos(x + 5, y + h * .5f), b = pos(x + 5 + ar * 1.5f, y + h * .5f - ar),
               c = pos(x + 5 + ar * 1.5f, y + h * .5f + ar);
        d->AddTriangleFilled(a, b, c, col(5));
        if (f.extra)
            d->AddTriangle(a, b, c, IM_COL32(0, 0, 0, 220));
    }
}
} // namespace

void Render(State &s) {
    const auto old = ImGui::GetStyle();
    auto &st = ImGui::GetStyle();
    st.WindowPadding = ImVec2(0, 0);
    st.FramePadding = ImVec2(8, 5);
    st.FrameRounding = 4;
    st.PopupRounding = 6;
    st.WindowRounding = 8;
    st.WindowBorderSize = 0;
    st.FrameBorderSize = 1;
    st.ItemSpacing = ImVec2(7, 6);
    auto *c = st.Colors;
    c[ImGuiCol_WindowBg] = ImVec4(.08f, .08f, .10f, 1);
    c[ImGuiCol_Text] = ImVec4(.91f, .91f, .95f, 1);
    c[ImGuiCol_TextDisabled] = ImVec4(.5f, .51f, .57f, 1);
    c[ImGuiCol_Border] = ImVec4(.2f, .2f, .25f, 1);
    c[ImGuiCol_FrameBg] = ImVec4(.075f, .075f, .095f, 1);
    c[ImGuiCol_FrameBgHovered] = ImVec4(.13f, .14f, .19f, 1);
    c[ImGuiCol_Button] = ImVec4(.15f, .16f, .21f, 1);
    c[ImGuiCol_ButtonHovered] = ImVec4(.18f, .23f, .34f, 1);
    c[ImGuiCol_ButtonActive] = ImVec4(.12f, .32f, .64f, 1);
    c[ImGuiCol_CheckMark] = ImVec4(s.settings.accent[0], s.settings.accent[1], s.settings.accent[2], 1);
    c[ImGuiCol_SliderGrab] = c[ImGuiCol_CheckMark];
    c[ImGuiCol_SliderGrabActive] = c[ImGuiCol_CheckMark];
    c[ImGuiCol_PopupBg] = ImVec4(.105f, .105f, .13f, 1);
    c[ImGuiCol_Header] = c[ImGuiCol_ButtonActive];
    ImGui::SetNextWindowSize(ImVec2(740, 490));
    if (ImGui::Begin("HYPER GUI", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
                         ImGuiWindowFlags_NoScrollWithMouse)) {
        origin = ImGui::GetWindowPos();
        auto *d = ImGui::GetWindowDrawList();
        d->AddRectFilledMultiColor(pos(0, 0), pos(740, 490), IM_COL32(29, 29, 38, 255),
                                   IM_COL32(25, 27, 37, 255), IM_COL32(20, 20, 26, 255),
                                   IM_COL32(23, 23, 29, 255));
        rect(0, 0, 174, 490, IM_COL32(22, 22, 27, 255), 0);
        line(174, 0, 174, 490, IM_COL32(47, 47, 58, 255));
        if (s.cs2Icon)
            d->AddImageRounded(s.cs2Icon, pos(15, 19), pos(39, 44), ImVec2(0, 0), ImVec2(1, 1),
                               IM_COL32_WHITE, 4);
        else {
            rect(15, 19, 24, 25, accent(s, 30), 4);
            text(17, 26, "CS2", 11, accent(s));
        }
        text(49, 20, "HYPER GUI", 19);
        text(49, 41, "COUNTER-STRIKE 2", 10, muted);
        const char *gen[] = {"AIM", "VISUALS", "MISC", "CONFIGS"};
        const char *gtr[] = {"NİŞAN", "GÖRSELLER", "DİĞER", "PROFİLLER"};
        float gy[] = {76, 199, 293, 387};
        float yy[] = {95, 125, 155, 218, 248, 312, 342, 406};
        for (int i = 0; i < 4; i++)
            text(18, gy[i], s.language == Language::English ? gen[i] : gtr[i], 12, muted);
        for (int i = 0; i < 8; i++) {
            ImGui::PushID(i);
            bool selected = s.tab == i;
            if (hit("nav", 10, yy[i], 153, 27))
                s.tab = i;
            if (selected) {
                rect(10, yy[i], 153, 27, accent(s, 16), 4);
                d->AddRect(pos(10, yy[i]), pos(163, yy[i] + 27), accent(s, 185), 4);
            } else if (ImGui::IsItemHovered())
                rect(10, yy[i], 153, 27, IM_COL32(255, 255, 255, 6), 4);
            icon(20, yy[i] + 5, i, selected ? accent(s) : muted);
            text(47, yy[i] + 6, s.language == Language::English ? names[i] : namesTR[i], 14,
                 selected ? ink : muted);
            ImGui::PopID();
        }
        line(14, 447, 160, 447, IM_COL32(44, 44, 52, 255));
        if (s.steamAvatar)
            d->AddImageRounded(s.steamAvatar, pos(16, 455), pos(44, 483), ImVec2(0, 0), ImVec2(1, 1),
                               IM_COL32_WHITE, 5);
        else {
            rect(16, 455, 28, 28, IM_COL32(40, 47, 58, 255), 5);
            d->AddCircle(pos(35, 463), 4, ink, 16, 1.5f);
            line(21, 474, 32, 466, ink, 2);
            d->AddCircle(pos(23, 473), 3, ink, 16, 1.3f);
        }
        d->PushClipRect(pos(51, 450), pos(162, 486), true);
        text(52, 456, s.steamName.empty() ? "Steam" : s.steamName.c_str(), 14, ink);
        text(52, 474, s.steamName.empty() ? tr(s, "No local profile", "Yerel profil yok") : "Steam", 10,
             muted);
        d->PopClipRect();
        text(194, 22, tr(s, "CS2 / ", "CS2 / "), 13, muted);
        text(236, 22, s.language == Language::English ? names[s.tab] : namesTR[s.tab], 13, ink);
        if (hit("language", 654, 16, 66, 25))
            s.language = s.language == Language::English ? Language::Turkish : Language::English;
        rect(654, 16, 66, 25, IM_COL32(39, 39, 49, 255), 4);
        text(665, 22, s.language == Language::English ? "EN / TR" : "TR / EN", 12);
        if (s.tab < 7) {
            auto &page = s.settings.pages[s.tab];
            const auto &l = labels[s.tab];
            card(194, 57, 256, 288, s.language == Language::English ? names[s.tab] : namesTR[s.tab], s.tab,
                 s);
            ImGui::PushID(s.tab);
            for (int i = 0; i < 6; i++)
                toggle(s, s.language == Language::English ? l.en[i] : l.tur[i], page.toggles[i], i, 207,
                       101 + i * 38.f, 229);
            ImGui::PopID();
            card(462, 57, 258, 288, tr(s, "Customize", "Özelleştir"), 6, s);
            const char *choices =
                s.language == Language::English ? "Balanced\0Precision\0Custom\0" : "Dengeli\0Hassas\0Özel\0";
            if (s.tab == 3)
                choices = s.language == Language::English ? "Full box\0Corner box\0Minimal\0"
                                                          : "Tam kutu\0Köşe kutusu\0Minimal\0";
            combo("##mode", s.tab == 3 ? tr(s, "Box type", "Kutu tipi") : tr(s, "Preset", "Ön ayar"),
                  page.mode, choices, 476, 107, 230);
            slider("##a", tr(s, l.a, l.at), page.amount, 476, 177, 230);
            slider("##b", tr(s, l.b, l.bt), page.secondary, 476, 253, 230);
            card(194, 357, 526, 77, tr(s, "Style", "Stil"), 5, s);
            text(208, 406, tr(s, "Accent", "Vurgu"), 13, muted);
            const float palette[4][3] = {
                {.16f, .49f, 1.f}, {.56f, .38f, 1.f}, {.1f, .76f, .64f}, {1.f, .39f, .48f}};
            for (int i = 0; i < 4; i++) {
                ImGui::PushID(i + 20);
                if (hit("swatch", 272 + i * 29.f, 401, 20, 20)) {
                    for (int j = 0; j < 3; j++)
                        s.settings.accent[j] = palette[i][j];
                }
                rect(272 + i * 29.f, 401, 20, 20,
                     IM_COL32(int(palette[i][0] * 255), int(palette[i][1] * 255), int(palette[i][2] * 255),
                              255),
                     5);
                ImGui::PopID();
            }
            ImGui::SetCursorPos(ImVec2(401, 399));
            ImGui::ColorEdit3("##accent", s.settings.accent,
                              ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            if (s.tab == 3) {
                ImGui::SetCursorPos(ImVec2(550, 398));
                if (ImGui::Button(tr(s, "Preview", "Önizleme"), ImVec2(155, 25)))
                    ImGui::OpenPopup("preview");
            }
            if (ImGui::BeginPopup("preview")) {
                ImVec2 previous = origin;
                origin = ImGui::GetCursorScreenPos();
                soldier(s, 0, 0, 228, 344);
                ImGui::Dummy(ImVec2(228, 344));
                origin = previous;
                ImGui::EndPopup();
            }
        } else {
            card(194, 57, 256, 377, tr(s, "My configs", "Profillerim"), 7, s);
            combo("##profile", tr(s, "Profile", "Profil"), s.profile,
                  s.language == Language::English ? "Default\0Precision\0Showcase\0"
                                                  : "Varsayılan\0Hassas\0Vitrin\0",
                  208, 110, 228);
            ImGui::SetCursorPos(ImVec2(208, 185));
            if (ImGui::Button(tr(s, "Save snapshot", "Anlık kaydet"), ImVec2(228, 31))) {
                s.profiles[s.profile] = s.settings;
                s.saved[s.profile] = true;
                s.notice = 1;
            }
            ImGui::SetCursorPos(ImVec2(208, 228));
            ImGui::BeginDisabled(!s.saved[s.profile]);
            if (ImGui::Button(tr(s, "Load snapshot", "Kaydı yükle"), ImVec2(228, 31))) {
                s.settings = s.profiles[s.profile];
                s.notice = 2;
            }
            ImGui::EndDisabled();
            ImGui::SetCursorPos(ImVec2(208, 271));
            if (ImGui::Button(tr(s, "Reset settings", "Ayarları sıfırla"), ImVec2(228, 31))) {
                s.settings = Settings{};
                s.notice = 3;
            }
            text(208, 328, tr(s, "Session only. No disk storage.", "Oturumluk. Diske kaydedilmez."), 12,
                 muted);
            if (s.notice)
                text(208, 362,
                     s.notice == 1   ? tr(s, "Saved", "Kaydedildi")
                     : s.notice == 2 ? tr(s, "Loaded", "Yüklendi")
                                     : tr(s, "Reset", "Sıfırlandı"),
                     14, accent(s));
            card(462, 57, 258, 377, tr(s, "Theme", "Tema"), 5, s);
            ImGui::SetCursorPos(ImVec2(478, 110));
            ImGui::SetNextItemWidth(226);
            ImGui::ColorPicker3("##palette", s.settings.accent,
                                ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview);
        }
        rect(391, 452, 131, 25, accent(s, 24), 5);
        text(407, 458, s.language == Language::English ? names[s.tab] : namesTR[s.tab], 13, accent(s));
        text(638, 461, "UI ONLY", 11, muted);
    }
    ImGui::End();
    ImGui::GetStyle() = old;
}
} // namespace hypergui
