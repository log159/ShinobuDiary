/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppView.hpp"
#include <cmath>
#include <string>
#include <sstream>
#include "LAppPal.hpp"
#include "LAppDelegate.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp"
#include "LAppDefine.hpp"
#include "TouchManager.hpp"
#include "LAppSprite.hpp"
#include "LAppSpriteShader.hpp"
#include "LAppModel.hpp"
#include "../shinobugui_src/global.h"

using namespace std;
using namespace LAppDefine;

Csm::CubismMatrix44* LAppView::GetDeviceToScreen() const
{
    return _deviceToScreen;
}

LAppView::LAppView():
    _back(NULL),
    _gear(NULL),
    _power(NULL),
    _renderSprite(NULL),
    _renderTarget(SelectTarget_None),
    _shader(NULL)
{
    _clearColor[0] = 1.0f;
    _clearColor[1] = 1.0f;
    _clearColor[2] = 1.0f;
    _clearColor[3] = 0.0f;

    // タッチ関係のイベント管理
    _touchManager = new TouchManager();

    // デバイス座標からスクリーン座標に変換するための
    _deviceToScreen = new CubismMatrix44();

    // 画面の表示の拡大縮小や移動の変換を行う行列
    _viewMatrix = new CubismViewMatrix();

    // スプライト用シェーダー
    _shader = new LAppSpriteShader();
}

LAppView::~LAppView()
{
    _renderBuffer.DestroyOffscreenSurface();

    ReleaseSprite();
    _shader->ReleaseShader();

    delete _shader;
    delete _viewMatrix;
    delete _deviceToScreen;
    delete _touchManager;
}

void LAppView::Initialize()
{
    int width, height;
    LAppDelegate::GetClientSize(width, height);

    if(width==0 || height==0)
    {
        return;
    }

    // 縦サイズを基準とする
    float ratio = static_cast<float>(width) / static_cast<float>(height);
    float left = -ratio;
    float right = ratio;
    float bottom = ViewLogicalLeft;
    float top = ViewLogicalRight;

    _viewMatrix->SetScreenRect(left, right, bottom, top); // デバイスに対応する画面の範囲。 Xの左端, Xの右端, Yの下端, Yの上端
    _viewMatrix->Scale(ViewScale, ViewScale);

    _deviceToScreen->LoadIdentity(); // サイズが変わった際などリセット必須
    if (width > height)
    {
        float screenW = fabsf(right - left);
        _deviceToScreen->ScaleRelative(screenW / width, -screenW / width);
    }
    else
    {
        float screenH = fabsf(top - bottom);
        _deviceToScreen->ScaleRelative(screenH / height, -screenH / height);
    }
    _deviceToScreen->TranslateRelative(-width * 0.5f, -height * 0.5f);

    // 表示範囲の設定
    _viewMatrix->SetMaxScale(ViewMaxScale); // 限界拡大率
    _viewMatrix->SetMinScale(ViewMinScale); // 限界縮小率

    // 表示できる最大範囲
    _viewMatrix->SetMaxScreenRect(
        ViewLogicalMaxLeft,
        ViewLogicalMaxRight,
        ViewLogicalMaxBottom,
        ViewLogicalMaxTop
    );

    // シェーダー作成
    _shader->CreateShader();
}
void LAppView::Render()
{
    LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
    if (!live2DManager)
    {
        return;
    }

    // スプライト描画
    int width, height;
    LAppDelegate::GetInstance()->GetClientSize(width, height);

    // デバイスコンテキスト取得
    ID3D11DeviceContext* renderContext = LAppDelegate::GetD3dContext();

    if (_back)
    {
        ID3D11ShaderResourceView* textureView = NULL;
        LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(_back->GetTextureId(), textureView);
        _back->RenderImmidiate(width, height, textureView, renderContext);
    }
    if (_gear)
    {
        ID3D11ShaderResourceView* textureView = NULL;
        LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(_gear->GetTextureId(), textureView);
        _gear->RenderImmidiate(width, height, textureView, renderContext);
    }
    if (_power)
    {
        ID3D11ShaderResourceView* textureView = NULL;
        LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(_power->GetTextureId(), textureView);
        _power->RenderImmidiate(width, height, textureView, renderContext);
    }

    // Cubism更新・描画
    live2DManager->OnUpdate();

    live2DManager->SetViewMatrix(_viewMatrix);

    // 各モデルが持つ描画ターゲットをテクスチャとする場合
    if (_renderTarget == SelectTarget_ModelFrameBuffer && _renderSprite)
    {
        int i = 0;
        for (auto& pair : live2DManager->GetModel()) {
            LAppModel* model = pair.second;
            float alpha = i < 1 ? 1.0f : model->GetOpacity(); // サンプルとしてαに適当な差をつける
            ++i;
            _renderSprite->SetColor(1.0f, 1.0f, 1.0f, alpha);

            if (model)
            {
                _renderSprite->RenderImmidiate(width, height, model->GetRenderBuffer().GetTextureView(), renderContext);
            }
        }
    }
    // 渲染红色边框
    std::unordered_map<int, LAppModel*>& models = LAppLive2DManager::GetInstance()->GetModel();
    for (auto& lampair : models) 
    {
        LAppModel* lam = lampair.second;
        if (lam->previewHitareas == false)
            continue;
        for (auto& val : lampair.second->hitareas) {
            float tx_t = lam->GetModelMatrix()->GetArray()[12];
            float ty_t = lam->GetModelMatrix()->GetArray()[13];
            float ts_x = lam->GetModelMatrix()->GetArray()[0];
            float ts_y = lam->GetModelMatrix()->GetArray()[5];
            Csm::CubismMatrix44* cm4 = LAppDelegate::GetInstance()->GetView()->GetDeviceToScreen();
            float offx = cm4->InvertTransformX(tx_t)- cm4->InvertTransformX(0);
            float offy = -(cm4->InvertTransformY(ty_t)- cm4->InvertTransformY(0));
            float cub_x = cm4->InvertTransformX(tx_t);
            float cub_y = height - cm4->InvertTransformY(ty_t);
            for (auto& las : val.second) {
                LAppSprite*& _hitarea = las;
                ID3D11ShaderResourceView* textureView = NULL;
                LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(_hitarea->GetTextureId(), textureView);
                _hitarea->ResetOffsetRect(offx, offy, ts_x/(lam->def_cubism_cg.cubism_ts_x), ts_y / (lam->def_cubism_cg.cubism_ts_y));
                _hitarea->RenderImmidiate(width, height, textureView, renderContext);
            }
        }
    }
}
void LAppView::InitializeSprite()
{
    // 描画領域サイズ
    int width, height;
    LAppDelegate::GetInstance()->GetClientSize(width, height);

    LAppTextureManager* textureManager = LAppDelegate::GetInstance()->GetTextureManager();
    const string resourcesPath = ResourcesPath;

    ID3D11Device* device = LAppDelegate::GetInstance()->GetD3dDevice();

    float x = 0.0f;
    float y = 0.0f;
    float fWidth = 0.0f;
    float fHeight = 0.0f;

    //string imageName = "";
    //delete
    /*resourcesPath + BackImageName*/
    //LAppTextureManager::TextureInfo* backgroundTexture = textureManager->CreateTextureFromPngFile(imageName, false);
    //x = width * 0.5f;
    //y = height * 0.5f;
    //fWidth = static_cast<float>(backgroundTexture->width * 2);
    //fHeight = static_cast<float>(height) * 0.95f;
    //_back = new LAppSprite(x, y, fWidth, fHeight, backgroundTexture->id, _shader, device);

    //imageName = resourcesPath + GearImageName;
    //LAppTextureManager::TextureInfo* gearTexture = textureManager->CreateTextureFromPngFile(imageName, false);
    //x = static_cast<float>(width - gearTexture->width * 0.5f);
    //y = static_cast<float>(height - gearTexture->height * 0.5f);
    //fWidth = static_cast<float>(gearTexture->width);
    //fHeight = static_cast<float>(gearTexture->height);
    //_gear = new LAppSprite(x, y, fWidth, fHeight, gearTexture->id, _shader, device);

    //imageName = resourcesPath + PowerImageName;
    //LAppTextureManager::TextureInfo* powerTexture = textureManager->CreateTextureFromPngFile(imageName, false);
    //x = static_cast<float>(width - powerTexture->width * 0.5f);
    //y = static_cast<float>(powerTexture->height * 0.5f);
    //fWidth = static_cast<float>(powerTexture->width);
    //fHeight = static_cast<float>(powerTexture->height);
    //_power = new LAppSprite(x, y, fWidth, fHeight, powerTexture->id, _shader, device);

    _back = nullptr;
    _gear = nullptr;
    _power = nullptr;

    x = width * 0.5f;
    y = height * 0.5f;
    _renderSprite = new LAppSprite(x, y, static_cast<float>(width), static_cast<float>(height), 0, _shader, device);
}

void LAppView::ReleaseSprite()
{
    LAppTextureManager* textureManager = LAppDelegate::GetInstance()->GetTextureManager();

    if (_renderSprite)
    {
        textureManager->ReleaseTexture(_renderSprite->GetTextureId());
    }
    delete _renderSprite;
    _renderSprite = NULL;

    if (_gear)
    {
        textureManager->ReleaseTexture(_gear->GetTextureId());
    }
    delete _gear;
    _gear = NULL;

    if (_power)
    {
        textureManager->ReleaseTexture(_power->GetTextureId());
    }
    delete _power;
    _power = NULL;

    if (_back)
    {
        textureManager->ReleaseTexture(_back->GetTextureId());
    }
    delete _back;
    _back = NULL;

}

void LAppView::ResizeSprite()
{
    LAppTextureManager* textureManager = LAppDelegate::GetInstance()->GetTextureManager();
    if (!textureManager)
    {
        return;
    }

    // 描画領域サイズ
    int width, height;
    LAppDelegate::GetInstance()->GetClientSize(width, height);

    float x = 0.0f;
    float y = 0.0f;
    float fWidth = 0.0f;
    float fHeight = 0.0f;

    if(_back)
    {
        Csm::csmUint64 id = _back->GetTextureId();
        LAppTextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
        if(texInfo)
        {
            x = width * 0.5f;
            y = height * 0.5f;
            fWidth = static_cast<float>(texInfo->width * 2);
            fHeight = static_cast<float>(height) * 0.95f;
            _back->ResetRect(x, y, fWidth, fHeight);
        }
    }

    if (_power)
    {
        Csm::csmUint64 id = _power->GetTextureId();
        LAppTextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
        if (texInfo)
        {
            x = static_cast<float>(width - texInfo->width * 0.5f);
            y = static_cast<float>(texInfo->height * 0.5f);
            fWidth = static_cast<float>(texInfo->width);
            fHeight = static_cast<float>(texInfo->height);
            _power->ResetRect(x, y, fWidth, fHeight);
        }
    }

    if (_gear)
    {
        Csm::csmUint64 id = _gear->GetTextureId();
        LAppTextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
        if (texInfo)
        {
            x = static_cast<float>(width - texInfo->width * 0.5f);
            y = static_cast<float>(height - texInfo->height * 0.5f);
            fWidth = static_cast<float>(texInfo->width);
            fHeight = static_cast<float>(texInfo->height);
            _gear->ResetRect(x, y, fWidth, fHeight);
        }
    }
    if (_renderSprite)
    {
        x = width * 0.5f;
        y = height * 0.5f;
        _renderSprite->ResetRect(x, y, static_cast<float>(width), static_cast<float>(height));
    }
}

void LAppView::OnTouchesBegan(float px, float py) const
{
    _touchManager->TouchesBegan(px, py);
}

void LAppView::OnTouchesMoved(float px, float py) const
{
    float viewX = this->TransformViewX(_touchManager->GetX());
    float viewY = this->TransformViewY(_touchManager->GetY());
    _touchManager->TouchesMoved(px, py);

    LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
    live2DManager->OnDrag(viewX, viewY);
}

void LAppView::OnTouchesEnded(float px, float py) const
{
    // タッチ終了
    LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
    live2DManager->OnDrag(0.0f, 0.0f);
    {
        int width, height;
        LAppDelegate::GetInstance()->GetClientSize(width, height);

        // シングルタップ
        float x = _deviceToScreen->TransformX(px); // 論理座標変換した座標を取得。
        float y = _deviceToScreen->TransformY(py); // 論理座標変換した座標を取得。
        if (DebugTouchLogEnable)
        {
            LAppPal::PrintLogLn("[APP]touchesEnded x:%.2f y:%.2f", x, y);
        }

        live2DManager->OnTap(x, y);

        //delete
        //// 歯車にタップしたか
        //if (_gear->IsHit(px, py, width, height))
        //{
        //    //live2DManager->NextScene();
        //    live2DManager->RefreshScene();
        //}
        // 電源ボタンにタップしたか
        //if (_power->IsHit(px, py, width, height))
        //{
        //    LAppDelegate::GetInstance()->AppEnd();
        //}
        
        //if (GlobalTemp::CubismModelRefreshPos.first) {
        //    live2DManager->RefreshSceneAndUserId(GlobalTemp::CubismModelRefreshPos.second);
        //    GlobalTemp::CubismModelRefreshPos.first = false;
        //}
        
        //动画队列
        std::queue<std::pair<int, std::pair<int, int>>>& cmm_qp  = GlobalTemp::CubismMotionMessage;
        while (!cmm_qp.empty()) {
            if (live2DManager->GetModel().find(cmm_qp.front().first) != live2DManager->GetModel().end()) {
                LAppModel* lam = live2DManager->GetModel()[cmm_qp.front().first];
                Csm::ICubismModelSetting* lcms = lam->GetModelSetting();
                const Csm::csmChar* gid = lam->GetModelSetting()->GetMotionGroupName(cmm_qp.front().second.first);
                int nid = cmm_qp.front().second.second;
                if (lcms !=nullptr && gid != nullptr && lcms->GetMotionCount(gid)> nid && nid>=0) {
                    lam->StartMotion(gid, nid, LAppDefine::PriorityForce);
                }
            }
            cmm_qp.pop();
        }
        //表情队列
        std::queue<std::pair<int, int>>& cem_qp  = GlobalTemp::CubismExpressionMessage;
        while (!cem_qp.empty()) {
            if (live2DManager->GetModel().find(cem_qp.front().first) != live2DManager->GetModel().end()) {
                LAppModel* lam = live2DManager->GetModel()[cem_qp.front().first];
                Csm::ICubismModelSetting* lcms = lam->GetModelSetting();
                int nid = cem_qp.front().second;
                if (lcms != nullptr &&  lam->GetModelSetting()->GetExpressionCount()> nid && nid >= 0) {
                    lam->SetExpression(lcms->GetExpressionName(nid));
                }
            }
            cem_qp.pop();
        }
        //加载删除队列
        std::queue<std::pair<int, std::string>>& qp = GlobalTemp::CubismModelMessage;
        while (!qp.empty()) {
            //-1 归置
            if (qp.front().first == -1) {
                std::string str = qp.front().second;
                int num = -1;
                std::istringstream ss(str); ss >> num;
                live2DManager->RefreshSceneAndUserId(num);
            }
            //删除或者添加
            else {
                live2DManager->RefreshScene(qp.front().first, qp.front().second);
            }
            qp.pop();
        }
    }
}

float LAppView::TransformViewX(float deviceX) const
{
    float screenX = _deviceToScreen->TransformX(deviceX); // 論理座標変換した座標を取得。
    return _viewMatrix->InvertTransformX(screenX); // 拡大、縮小、移動後の値。
}

float LAppView::TransformViewY(float deviceY) const
{
    float screenY = _deviceToScreen->TransformY(deviceY); // 論理座標変換した座標を取得。
    return _viewMatrix->InvertTransformY(screenY); // 拡大、縮小、移動後の値。
}

float LAppView::TransformScreenX(float deviceX) const
{
    return _deviceToScreen->TransformX(deviceX);
}

float LAppView::TransformScreenY(float deviceY) const
{
    return _deviceToScreen->TransformY(deviceY);
}

void LAppView::PreModelDraw(LAppModel& refModel)
{
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismOffscreenSurface_D3D11* useTarget = NULL;

    if (_renderTarget != SelectTarget_None)
    {// 別のレンダリングターゲットへ向けて描画する場合

        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        if (!useTarget->IsValid())
        {// 描画ターゲット内部未作成の場合はここで作成
            int width, height;
            LAppDelegate::GetClientSize(width, height);

            if (width != 0 && height != 0)
            {
                // モデル描画キャンバス
                useTarget->CreateOffscreenSurface(LAppDelegate::GetInstance()->GetD3dDevice(),
                    static_cast<csmUint32>(width), static_cast<csmUint32>(height));
            }
        }

        // レンダリング開始
        useTarget->BeginDraw(LAppDelegate::GetInstance()->GetD3dContext());
        useTarget->Clear(LAppDelegate::GetInstance()->GetD3dContext(), _clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]); // 背景クリアカラー
    }
}

void LAppView::PostModelDraw(LAppModel& refModel)
{
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismOffscreenSurface_D3D11* useTarget = NULL;

    if (_renderTarget != SelectTarget_None)
    {// 別のレンダリングターゲットへ向けて描画する場合

        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        // レンダリング終了
        useTarget->EndDraw(LAppDelegate::GetInstance()->GetD3dContext());

        // LAppViewの持つフレームバッファを使うなら、スプライトへの描画はここ
        if (_renderTarget == SelectTarget_ViewFrameBuffer && _renderSprite)
        {
            // スプライト描画
            int width, height;
            LAppDelegate::GetInstance()->GetClientSize(width, height);

            _renderSprite->SetColor(1.0f, 1.0f, 1.0f, GetSpriteAlpha(0));
            _renderSprite->RenderImmidiate(width, height, useTarget->GetTextureView(), LAppDelegate::GetInstance()->GetD3dContext());
        }
    }
}

void LAppView::SwitchRenderingTarget(SelectTarget targetType)
{
    _renderTarget = targetType;
}

void LAppView::SetRenderTargetClearColor(float r, float g, float b)
{
    _clearColor[0] = r;
    _clearColor[1] = g;
    _clearColor[2] = b;
}

void LAppView::DestroyOffscreenSurface()
{
    _renderBuffer.DestroyOffscreenSurface();
}

LAppSpriteShader* LAppView::GetShader()
{
    return _shader;
}

float LAppView::GetSpriteAlpha(int assign) const
{
    // assignの数値に応じて適当に決定
    float alpha = 0.25f + static_cast<float>(assign) * 0.5f; // サンプルとしてαに適当な差をつける
    if (alpha > 1.0f)
    {
        alpha = 1.0f;
    }
    if (alpha < 0.1f)
    {
        alpha = 0.1f;
    }

    return alpha;
}
