#pragma once
#include "steam_profile_data.hpp"
#include "cs2_icon.hpp"
#include "hyper_gui.hpp"
#include <windows.h>
#include <d3d11.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <iterator>
namespace steam_local {
using Microsoft::WRL::ComPtr;
class Profile {
    ComPtr<IWICImagingFactory> factory;
    ComPtr<ID3D11ShaderResourceView> avatar, gameIcon;
    std::chrono::steady_clock::time_point next{};
    std::filesystem::path previousFile;
    std::filesystem::file_time_type previousTime{};
    static std::filesystem::path SteamPath() {
        wchar_t path[32768]{};
        DWORD bytes = sizeof(path);
        if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Valve\\Steam", L"SteamPath", RRF_RT_REG_SZ, nullptr,
                         path, &bytes) == ERROR_SUCCESS)
            return path;
        return {};
    }
    bool Decode(ID3D11Device *device, IWICBitmapDecoder *decoder, ComPtr<ID3D11ShaderResourceView> &output) {
        ComPtr<IWICBitmapFrameDecode> frame;
        ComPtr<IWICFormatConverter> converter;
        if (FAILED(decoder->GetFrame(0, &frame)) || FAILED(factory->CreateFormatConverter(&converter)))
            return false;
        if (FAILED(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone,
                                         nullptr, 0, WICBitmapPaletteTypeCustom)))
            return false;
        UINT w = 0, h = 0;
        if (FAILED(converter->GetSize(&w, &h)) || !w || !h || w > 2048 || h > 2048)
            return false;
        std::vector<BYTE> pixels(size_t(w) * h * 4);
        if (FAILED(converter->CopyPixels(nullptr, w * 4, UINT(pixels.size()), pixels.data())))
            return false;
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = w;
        desc.Height = h;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        D3D11_SUBRESOURCE_DATA data{pixels.data(), w * 4, 0};
        ComPtr<ID3D11Texture2D> texture;
        return SUCCEEDED(device->CreateTexture2D(&desc, &data, &texture)) &&
               SUCCEEDED(device->CreateShaderResourceView(texture.Get(), nullptr, &output));
    }

  public:
    void Initialize(ID3D11Device *device, hypergui::State &state) {
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(&factory))))
            return;
        ComPtr<IWICStream> stream;
        ComPtr<IWICBitmapDecoder> decoder;
        if (SUCCEEDED(factory->CreateStream(&stream)) &&
            SUCCEEDED(stream->InitializeFromMemory(const_cast<BYTE *>(cs2_icon), sizeof(cs2_icon))) &&
            SUCCEEDED(factory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad,
                                                       &decoder)))
            Decode(device, decoder.Get(), gameIcon);
        state.cs2Icon = (ImTextureID)(intptr_t)gameIcon.Get();
        Update(device, state);
    }
    void Update(ID3D11Device *device, hypergui::State &state) {
        auto now = std::chrono::steady_clock::now();
        if (now < next)
            return;
        next = now + std::chrono::seconds(10);
        try {
            auto root = SteamPath();
            if (root.empty()) {
                state.steamName.clear();
                state.steamAvatar = 0;
                avatar.Reset();
                return;
            }
            std::ifstream file(root / L"config" / L"loginusers.vdf", std::ios::binary);
            std::string source((std::istreambuf_iterator<char>(file)), {});
            DWORD active = 0, bytes = sizeof(active);
            RegGetValueW(HKEY_CURRENT_USER, L"Software\\Valve\\Steam\\ActiveProcess", L"ActiveUser",
                         RRF_RT_REG_DWORD, nullptr, &active, &bytes);
            User user = Select(Parse(source), active);
            state.steamName = user.name;
            auto photo = root / L"config" / L"avatarcache" / (std::to_wstring(user.id) + L".png");
            std::error_code ec;
            auto modified = std::filesystem::last_write_time(photo, ec);
            if (photo != previousFile || (!ec && modified != previousTime) || !avatar) {
                avatar.Reset();
                state.steamAvatar = 0;
                previousFile = photo;
                previousTime = modified;
                ComPtr<IWICBitmapDecoder> decoder;
                if (!ec && factory &&
                    SUCCEEDED(factory->CreateDecoderFromFilename(photo.c_str(), nullptr, GENERIC_READ,
                                                                 WICDecodeMetadataCacheOnLoad, &decoder)))
                    Decode(device, decoder.Get(), avatar);
                state.steamAvatar = (ImTextureID)(intptr_t)avatar.Get();
            }
        } catch (...) {
            state.steamName.clear();
            state.steamAvatar = 0;
            avatar.Reset();
        }
    }
    void Shutdown(hypergui::State &state) {
        state.steamAvatar = state.cs2Icon = 0;
        avatar.Reset();
        gameIcon.Reset();
        factory.Reset();
    }
};
} // namespace steam_local
