/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppTextureManager.hpp"
#include "LAppDefine.hpp"
#include "LAppPal.hpp"
#include "LAppDelegate.hpp"
#include <vector>
using namespace LAppDefine;

LAppTextureManager::LAppTextureManager()
{
    _sequenceId = 0;
}

LAppTextureManager::~LAppTextureManager()
{
    ReleaseTextures();
}
LAppTextureManager::TextureInfo* LAppTextureManager::CreateTextureFromRGBA(float r, float g, float b, float a, UINT width, UINT height)
{
    ID3D11Device* device = LAppDelegate::GetD3dDevice();
    ID3D11DeviceContext* context = LAppDelegate::GetD3dContext();

    if (!device || !context)
    {
        return nullptr;
    }

    ID3D11Texture2D* texture = nullptr;
    ID3D11ShaderResourceView* textureView = nullptr;
    LAppTextureManager::TextureInfo* textureInfo = nullptr;

    // 填充背景颜色
    std::vector<uint8_t> rgbaData(width * height * 4, 0);
    uint8_t rByte = static_cast<uint8_t>(r * 255);
    uint8_t gByte = static_cast<uint8_t>(g * 255);
    uint8_t bByte = static_cast<uint8_t>(b * 255);
    uint8_t aByte = static_cast<uint8_t>(a * 255);

    for (UINT y = 0; y < height; ++y)
    {
        for (UINT x = 0; x < width; ++x)
        {
            size_t index = (y * width + x) * 4;
            rgbaData[index + 0] = rByte; // R
            rgbaData[index + 1] = gByte; // G
            rgbaData[index + 2] = bByte; // B
            rgbaData[index + 3] = aByte; // A
        }
    }

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = rgbaData.data();
    initData.SysMemPitch = width * 4;

    HRESULT hr = device->CreateTexture2D(&desc, &initData, &texture);
    if (FAILED(hr))
    {
        if (DebugLogEnable)
        {
            LAppPal::PrintLogLn("Texture Creation Error");
        }
        return nullptr;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(texture, &srvDesc, &textureView);
    if (FAILED(hr))
    {
        texture->Release();
        return nullptr;
    }

    textureInfo = new LAppTextureManager::TextureInfo();
    if (!textureInfo)
    {
        textureView->Release();
        texture->Release();
        return nullptr;
    }

    textureInfo->width = width;
    textureInfo->height = height;
    textureInfo->id = ++_sequenceId;
    _texturesInfo.PushBack(textureInfo);
    _textures.PushBack(texture);
    _textureView.PushBack(textureView);

    return textureInfo;
}

LAppTextureManager::TextureInfo* LAppTextureManager::CreateTextureFromPngFile(std::string fileName, bool isPreMult, UINT maxSize)
{
    ID3D11Device* device = LAppDelegate::GetD3dDevice();
    ID3D11DeviceContext* context = LAppDelegate::GetD3dContext();

    // wcharに変換
    const int WCHAR_LENGTH = 512;
    wchar_t wchrStr[WCHAR_LENGTH] = L"";
    LAppPal::ConvertMultiByteToWide(fileName.c_str(), wchrStr, sizeof(wchrStr));

    ID3D11Resource* texture = NULL;
    ID3D11ShaderResourceView* textureView = NULL;
    LAppTextureManager::TextureInfo* textureInfo = NULL;

    HRESULT hr = S_OK;

    if (isPreMult)
    {
        hr = DirectX::CreateWICTextureFromFileEx(device,
            NULL,   // NULLにするとMIP=1となる
            wchrStr, maxSize,
            D3D11_USAGE_DYNAMIC,
            D3D11_BIND_SHADER_RESOURCE,
            D3D11_CPU_ACCESS_WRITE,
            0,
            DirectX::WIC_LOADER_DEFAULT,
            &texture, &textureView);
    }
    else
    {
        hr = DirectX::CreateWICTextureFromFileEx(device,
            context,
            wchrStr, maxSize,
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE,
            0,
            0,
            DirectX::WIC_LOADER_DEFAULT,
            &texture, &textureView);
    }

    if (SUCCEEDED(hr))
    {
        do
        {
            Microsoft::WRL::ComPtr<IWICImagingFactory> factoryWic;  ///<
            hr = CoCreateInstance(
                CLSID_WICImagingFactory,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&factoryWic)
            );
            if (FAILED(hr))
            {
                break;
            }

            // decoder作ってファイルを渡す
            Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
            hr = factoryWic->CreateDecoderFromFilename(wchrStr, NULL,
                GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
            if (FAILED(hr))
            {
                break;
            }

            // decoderからframeを取得
            Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
            hr = decoder->GetFrame(0, &frame);
            if (FAILED(hr))
            {
                break;
            }
            UINT texWidth, texHeight;
            hr = frame->GetSize(&texWidth, &texHeight);
            if (FAILED(hr))
            {
                break;
            }

            // テクスチャ情報
            textureInfo = new LAppTextureManager::TextureInfo();

            if (!textureInfo)
            {
                break;
            }

            // 作成成功
            {
                // 次のID
                const Csm::csmUint64 addId = _sequenceId + 1;

                // 情報格納
                textureInfo->fileName = fileName;
                textureInfo->width = static_cast<int>(texWidth);
                textureInfo->height = static_cast<int>(texHeight);
                textureInfo->id = addId;

                _sequenceId = addId;

                _texturesInfo.PushBack(textureInfo);
                _textures.PushBack(texture);
                _textureView.PushBack(textureView);

                if (isPreMult)
                {
                    D3D11_MAPPED_SUBRESOURCE subRes;
                    if (SUCCEEDED(context->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
                    {
                        // テンポラリ領域確保
                        byte* pdd = static_cast<byte*>(malloc(subRes.RowPitch * texHeight));
                        // 全エリアコピー
                        frame->CopyPixels(NULL, subRes.RowPitch, subRes.RowPitch * texHeight, pdd);

                        ULONG* pixel32 = static_cast<ULONG*>(subRes.pData);
                        for (unsigned int htLoop = 0; htLoop < texHeight; htLoop++)
                        {
                            unsigned char* pixel4 = reinterpret_cast<unsigned char*>(pdd) + subRes.RowPitch * htLoop;
                            unsigned int* pixel32 =
                                reinterpret_cast<unsigned int*>(reinterpret_cast<unsigned char*>(subRes.pData) + subRes.RowPitch * htLoop);

                            for (UINT i = 0; i < subRes.RowPitch; i += 4)
                            {
                                unsigned int val = Premultiply(pixel4[i + 0], pixel4[i + 1], pixel4[i + 2], pixel4[i + 3]);
                                pixel32[(i >> 2)] = val;
                            }
                        }

                        // テンポラリ開放
                        free(pdd);
                        // 解除
                        context->Unmap(texture, 0);
                    }
                }

                return textureInfo;
            }
        } while (0);
    }

    // 失敗
    if (DebugLogEnable)
    {
        LAppPal::PrintLogLn("Texture Load Error : %s", fileName.c_str());
    }

    return NULL;
}

void LAppTextureManager::ReleaseTextures()
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        // info除去
        delete _texturesInfo[i];

        // 実体除去
        if (_textureView[i])
        {
            _textureView[i]->Release();
            _textureView[i] = NULL;
        }
        if (_textures[i])
        {
            _textures[i]->Release();
            _textures[i] = NULL;
        }
    }

    _texturesInfo.Clear();
    _textures.Clear();
    _textureView.Clear();
}

void LAppTextureManager::ReleaseTexture(Csm::csmUint64 textureId)
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->id != textureId)
        {
            continue;
        }
        // ID一致

        // info除去
        delete _texturesInfo[i];

        // 実体除去
        if (_textureView[i])
        {
            _textureView[i]->Release();
            _textureView[i] = NULL;
        }
        if (_textures[i])
        {
            _textures[i]->Release();
            _textures[i] = NULL;
        }

        // 器除去
        _texturesInfo.Remove(i);
        _textures.Remove(i);
        _textureView.Remove(i);

        break;
    }

    if (_textureView.GetSize() == 0)
    {
        _textureView.Clear();
    }
    if (_textures.GetSize() == 0)
    {
        _textures.Clear();
    }
    if(_texturesInfo.GetSize()==0)
    {
        _texturesInfo.Clear();
    }
}

void LAppTextureManager::ReleaseTexture(std::string fileName)
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->fileName == fileName)
        {
            // info除去
            delete _texturesInfo[i];

            // 実体除去
            if (_textureView[i])
            {
                _textureView[i]->Release();
                _textureView[i] = NULL;
            }
            if (_textures[i])
            {
                _textures[i]->Release();
                _textures[i] = NULL;
            }

            // 器除去
            _texturesInfo.Remove(i);
            _textures.Remove(i);
            _textureView.Remove(i);

            break;
        }
    }

    if (_textureView.GetSize() == 0)
    {
        _textureView.Clear();
    }
    if (_textures.GetSize() == 0)
    {
        _textures.Clear();
    }
    if (_texturesInfo.GetSize() == 0)
    {
        _texturesInfo.Clear();
    }
}

bool LAppTextureManager::GetTexture(Csm::csmUint64 textureId, ID3D11ShaderResourceView*& retTexture) const
{
    retTexture = NULL;
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->id == textureId)
        {
            retTexture = _textureView[i];
            return true;
        }
    }

    return false;
}

LAppTextureManager::TextureInfo* LAppTextureManager::GetTextureInfoByName(std::string& fileName) const
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->fileName == fileName)
        {
            return _texturesInfo[i];
        }
    }

    return NULL;
}

LAppTextureManager::TextureInfo* LAppTextureManager::GetTextureInfoById(Csm::csmUint64 textureId) const
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->id == textureId)
        {
            return _texturesInfo[i];
        }
    }

    return NULL;
}
