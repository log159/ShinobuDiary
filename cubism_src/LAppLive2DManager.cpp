﻿/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include "LAppLive2DManager.hpp"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <Rendering/D3D11/CubismRenderer_D3D11.hpp>
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppDelegate.hpp"
#include "LAppModel.hpp"
#include "LAppView.hpp"
#include <iostream>
#include "../global.h"

using namespace Csm;
using namespace LAppDefine;

namespace {
    LAppLive2DManager* s_instance = NULL;

    void BeganMotion(ACubismMotion* self)
    {
        LAppPal::PrintLogLn("Motion Began: %x", self);
    }

    void FinishedMotion(ACubismMotion* self)
    {
        LAppPal::PrintLogLn("Motion Finished: %x", self);
    }

    int CompareCsmString(const void* a, const void* b)
    {
        return strcmp(reinterpret_cast<const Csm::csmString*>(a)->GetRawString(),
            reinterpret_cast<const Csm::csmString*>(b)->GetRawString());
    }
}

LAppLive2DManager* LAppLive2DManager::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new LAppLive2DManager();
    }

    return s_instance;
}

void LAppLive2DManager::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}

LAppLive2DManager::LAppLive2DManager()
    : _viewMatrix(NULL)
    //, _sceneIndex(0)
{
    _viewMatrix = new CubismMatrix44();
    SetUpModel();

    //ChangeScene(_sceneIndex);
    //ChangeScene(0);
}

LAppLive2DManager::~LAppLive2DManager()
{
    ReleaseAllModel();
    // カウンターを待たず速攻破棄
    for (int i = _releaseModel.GetSize() - 1; i >= 0; i--)
    {
        if (_releaseModel[i]._model)
        {
            delete _releaseModel[i]._model;
        }
    }
    _releaseModel.Clear();
    _modelDir.Clear();
    delete _viewMatrix;

    CubismFramework::Dispose();
}

void LAppLive2DManager::ReleaseAllModel()
{
    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        //delete _models[i]; ここでは消さない

        // 削除予定の印
        _models[i]->DeleteMark();

        // 2フレーム後削除
        ReleaseModel rel;
        rel._model = _models[i];
        rel._counter = 2;
        _releaseModel.PushBack(rel);
    }

    _models.Clear();
}

void LAppLive2DManager::SetUpModel()
{
    // ResourcesPathの中にあるフォルダ名を全てクロールし、モデルが存在するフォルダを定義する。
    // フォルダはあるが同名の.model3.jsonが見つからなかった場合はリストに含めない。
    // 一部文字が受け取れないためワイド文字で受け取ってUTF8に変換し格納する。

    csmString crawlPath(ResourcesPath);
    crawlPath += "*.*";

    wchar_t wideStr[MAX_PATH];
    csmChar name[MAX_PATH];
    LAppPal::ConvertMultiByteToWide(crawlPath.GetRawString(), wideStr, MAX_PATH);

    struct _wfinddata_t fdata;
    intptr_t fh = _wfindfirst(wideStr, &fdata);
    if (fh == -1)
    {
        return;
    }

    _modelDir.Clear();

    while (_wfindnext(fh, &fdata) == 0)
    {
        if ((fdata.attrib & _A_SUBDIR) && wcscmp(fdata.name, L"..") != 0)
        {
            LAppPal::ConvertWideToMultiByte(fdata.name, name, MAX_PATH);

            // フォルダと同名の.model3.jsonがあるか探索する
            csmString model3jsonPath(ResourcesPath);
            model3jsonPath += name;
            model3jsonPath.Append(1, '/');
            {
                csmString crawlPath(model3jsonPath);
                crawlPath += "*.*";
                wchar_t wideStr[MAX_PATH];
                csmChar name[MAX_PATH];
                LAppPal::ConvertMultiByteToWide(crawlPath.GetRawString(), wideStr, MAX_PATH);
                struct _wfinddata_t fdata;
                intptr_t fh = _wfindfirst(wideStr, &fdata);
                if (fh == -1)
                {
                    return;
                }
                while (_wfindnext(fh, &fdata) == 0)
                {
                    if (!(fdata.attrib & _A_SUBDIR))
                    {
                        if (wcslen(fdata.name) >= 11 && wcscmp(fdata.name + wcslen(fdata.name) - 11, L"model3.json") == 0)
                        {
                            LAppPal::ConvertWideToMultiByte(fdata.name, name, MAX_PATH);
                            _modelDir.PushBack(csmString(model3jsonPath)+ csmString(name));
                        }
                    }

                }
            }
        }
    }
    for (csmUint32 i = 0; i < _modelDir.GetSize(); ++i) {
        printf("%hs\n", _modelDir[i].GetRawString());
    }
    std::cout << u8"Cubism find end " << u8"共加载 " << _modelDir.GetSize() << u8" 个模型" << std::endl;
    
    qsort(_modelDir.GetPtr(), _modelDir.GetSize(), sizeof(csmString), CompareCsmString);
    
}

csmVector<csmString> LAppLive2DManager::GetModelDir() const
{
    return _modelDir;
}

csmInt32 LAppLive2DManager::GetModelDirSize() const
{
    return _modelDir.GetSize();
}

LAppModel* LAppLive2DManager::GetModel(csmUint32 no) const
{
    if (no < _models.GetSize())
    {
        return _models[no];
    }

    return NULL;
}

void LAppLive2DManager::OnDrag(csmFloat32 x, csmFloat32 y) const
{
    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        LAppModel* model = GetModel(i);

        model->SetDragging(x, y);
    }
}

void LAppLive2DManager::OnTap(csmFloat32 x, csmFloat32 y)
{
    if (DebugLogEnable)
    {
        LAppPal::PrintLogLn("[APP]tap point: {x:%.2f y:%.2f}", x, y);
    }

    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        if (_models[i]->HitTest(HitAreaNameHead, x, y))
        {
            if (DebugLogEnable)
            {
                LAppPal::PrintLogLn("[APP]hit area: [%s]", HitAreaNameHead);
            }
            _models[i]->SetRandomExpression();
        }
        else if (_models[i]->HitTest(HitAreaNameBody, x, y))
        {
            if (DebugLogEnable)
            {
                LAppPal::PrintLogLn("[APP]hit area: [%s]", HitAreaNameBody);
            }
            _models[i]->StartRandomMotion(MotionGroupTapBody, PriorityNormal, FinishedMotion, BeganMotion);
        }
    }
}

void LAppLive2DManager::OnUpdate() const
{
    int windowWidth, windowHeight;
    LAppDelegate::GetInstance()->GetClientSize(windowWidth, windowHeight);

    // D3D11 フレーム先頭処理
    // 各フレームでの、Cubism SDK の処理前にコール
    Rendering::CubismRenderer_D3D11::StartFrame(LAppDelegate::GetInstance()->GetD3dDevice(), LAppDelegate::GetInstance()->GetD3dContext(), windowWidth, windowHeight);

    const csmUint32 modelCount = _models.GetSize();
    for (csmUint32 i = 0; i < modelCount; ++i)
    {
        // 投影用マトリックス
        CubismMatrix44 projection;
        LAppModel* model = GetModel(i);

        if (model->GetModel() == NULL)
        {
            LAppPal::PrintLogLn("Failed to model->GetModel().");
            continue;
        }
        if (model->GetModel()->GetCanvasWidth() > 1.0f && windowWidth < windowHeight)
        {
            // 横に長いモデルを縦長ウィンドウに表示する際モデルの横サイズでscaleを算出する
            model->GetModelMatrix()->SetWidth(2.0f);
            projection.Scale(1.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight));
        }
        else
        {
            projection.Scale(static_cast<float>(windowHeight) / static_cast<float>(windowWidth), 1.0f);
        }

        // 必要があればここで乗算
        if (_viewMatrix != NULL)
        {
            projection.MultiplyByMatrix(_viewMatrix);
        }

        // モデル1体描画前コール
        LAppDelegate::GetInstance()->GetView()->PreModelDraw(*model);

        // Cubismモデルの描画
        model->Update();
        model->Draw(projection);///< 参照渡しなのでprojectionは変質する

        // モデル1体描画後コール
        LAppDelegate::GetInstance()->GetView()->PostModelDraw(*model);
    }

    // D3D11 フレーム終了処理
    // 各フレームでの、Cubism SDK の処理後にコール
    Rendering::CubismRenderer_D3D11::EndFrame(LAppDelegate::GetInstance()->GetD3dDevice());
}


ModelJsonConfig LAppLive2DManager::GetModelJsonConfig(int index) {
    const csmChar* modelptr = _modelDir[index].GetRawString();
    const csmChar* lastSlash = strrchr(modelptr, '/');
    static csmChar modelPath[512] = { 0 };
    static csmChar modelJsonName[512] = { 0 };
    memset(modelPath, 0, sizeof(modelPath));
    memset(modelJsonName, 0, sizeof(modelJsonName));
    size_t pathLength = lastSlash - modelptr + 1;
    strncpy_s(modelPath, modelptr, pathLength);
    modelPath[pathLength] = '\0';
    // 斜杠之后部分拷贝到modelJsonName
    strncpy_s(modelJsonName, lastSlash + 1, sizeof(modelJsonName) - 1);
    modelJsonName[sizeof(modelJsonName) - 1] = '\0';
    printf("Change Cubism |%hs| -> |%hs|\n", modelPath, modelJsonName);
    ModelJsonConfig mjc;
    mjc.modelPath = modelPath;
    mjc.modelJsonName = modelJsonName;
    return mjc;
}
//#define USE_RENDER_TARGET
//#define USE_MODEL_RENDER_TARGET
void LAppLive2DManager::RefreshScene()
{
    if (_modelDir.GetSize() <= 0)
        return;
    std::cout << u8"Cubism 刷新" << std::endl;
    ReleaseAllModel();
    for (auto it = UserCubismMap.begin(); it != UserCubismMap.end(); ++it) {
        int index = 0;
        for (int i = 0; i < _modelDir.GetSize(); ++i) {
            if (strcmp(it->second.second.c_str(), _modelDir[i].GetRawString()) == 0) {
                index = i;
                break;
            }
        }
        ModelJsonConfig mjc = GetModelJsonConfig(index);
        _models.PushBack(new LAppModel());
        LAppModel* lam = _models[_models.GetSize() - 1];
        lam->LoadAssets(mjc.modelPath.GetRawString(), mjc.modelJsonName.GetRawString());
        //Shinobu Debug
        lam->GetModelMatrix()->TranslateX(-0.5f + it->second.first * 0.1f);
        std::cout << u8"用户 ID：" << it->first << u8" Cubism ID：" << it->second.first << u8" Cubism Model：" << it->second.second.c_str() << std::endl;
    }
  
    /*
     * モデル半透明表示を行うサンプルを提示する。
     * ここでUSE_RENDER_TARGET、USE_MODEL_RENDER_TARGETが定義されている場合
     * 別のレンダリングターゲットにモデルを描画し、描画結果をテクスチャとして別のスプライトに張り付ける。
     */
    {
#if defined(USE_RENDER_TARGET)
        // LAppViewの持つターゲットに描画を行う場合、こちらを選択
        LAppView::SelectTarget useRenderTarget = LAppView::SelectTarget_ViewFrameBuffer;
#elif defined(USE_MODEL_RENDER_TARGET)
        // 各LAppModelの持つターゲットに描画を行う場合、こちらを選択
        LAppView::SelectTarget useRenderTarget = LAppView::SelectTarget_ModelFrameBuffer;
#else
        // デフォルトのメインフレームバッファへレンダリングする(通常)
        LAppView::SelectTarget useRenderTarget = LAppView::SelectTarget_None;
#endif

#if defined(USE_RENDER_TARGET) || defined(USE_MODEL_RENDER_TARGET)
        // モデル個別にαを付けるサンプルとして、もう1体モデルを作成し、少し位置をずらす

#endif
        LAppDelegate::GetInstance()->GetView()->SwitchRenderingTarget(useRenderTarget);

        // 別レンダリング先を選択した際の背景クリア色
        float clearColor[3] = { 0.0f, 0.0f, 0.0f };
        LAppDelegate::GetInstance()->GetView()->SetRenderTargetClearColor(clearColor[0], clearColor[1], clearColor[2]);
    }
}

void LAppLive2DManager::RefreshSceneSpecial()
{
    if (_modelDir.GetSize() <= 0)
        return;
    std::cout << u8"Cubism Simple刷新" << std::endl;
    
    GlobalTemp::RefreshCubismUsers.front();


}

Csm::csmVector<Csm::csmString>& LAppLive2DManager::GetModelDir()
{
    return  _modelDir;
}


csmUint32 LAppLive2DManager::GetModelNum() const
{
    return _models.GetSize();
}

void LAppLive2DManager::EndFrame()
{
    // モデル解放監視
    for( int i= _releaseModel.GetSize()-1; i>=0; i--)
    {
        _releaseModel[i]._counter--;

        if (_releaseModel[i]._counter <= 0)
        {// モデル削除
            if (_releaseModel[i]._model)
            {
                delete _releaseModel[i]._model;
                _releaseModel[i]._model = nullptr;
            }
            // コンテナも削除
            _releaseModel.Remove(i);
            continue;
        }
    }
}

void LAppLive2DManager::ResizedWindow()
{
    const csmUint32 modelCount = _models.GetSize();
    for (csmUint32 i = 0; i < modelCount; ++i)
    {
        _models[i]->GetRenderBuffer().DestroyOffscreenSurface();
    }
}

void LAppLive2DManager::SetViewMatrix(CubismMatrix44* m)
{
    for (int i = 0; i < 16; i++) {
        _viewMatrix->GetArray()[i] = m->GetArray()[i];
    }
}

