/**
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
{
    _viewMatrix = new CubismMatrix44();
    SetUpModel();
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
    for (auto& pair : _models) {
        pair.second->DeleteMark();
        ReleaseModel rel;
        rel._model = pair.second;
        rel._counter = 2;
        _releaseModel.PushBack(rel);
    }
    _models.clear();
}

void LAppLive2DManager::ReleaseOneModel(int userid)
{
    for (auto& pair : _models) {
        if (pair.first == userid) {
            pair.second->DeleteMark();
            ReleaseModel rel;
            rel._model = pair.second;
            rel._counter = 2;
            _releaseModel.PushBack(rel);
            break;
        }
    }
    _models.erase(userid);
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
        return;

    _modelDir.Clear();
    _modelDir.PushBack("");
    while (_wfindnext(fh, &fdata) == 0)
    {
        if ((fdata.attrib & _A_SUBDIR) && wcscmp(fdata.name, L"..") != 0)
        {
            LAppPal::ConvertWideToMultiByte(fdata.name, name, MAX_PATH);

            // .model3.json 探索
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

std::unordered_map<Csm::csmUint32, LAppModel*, Uint32Hash>& LAppLive2DManager::GetModel()
{
    return _models;
}

void LAppLive2DManager::OnDrag(csmFloat32 x, csmFloat32 y) const
{
    for (auto& pair : _models) {
        if (pair.second->canLookMouse)
            pair.second->SetDragging(x - pair.second->GetModelMatrix()->GetTranslateX(), y - pair.second->GetModelMatrix()->GetTranslateY());
        else
            pair.second->SetDragging(0.0f, 0.0f);
    }
}

void LAppLive2DManager::OnTap(csmFloat32 x, csmFloat32 y)
{
    if (DebugLogEnable)
    {
        LAppPal::PrintLogLn("[APP]tap point: {x:%.2f y:%.2f}", x, y);
    }
   
    for (auto& pair : _models) {
        //Shinobu Debug
        Csm::ICubismModelSetting* lms = pair.second->GetModelSetting();
        for (int i = 0; i < lms->GetHitAreasCount(); ++i) {
            if (pair.second->HitTest(lms->GetHitAreaName(i), x, y)) {
                printf(lms->GetHitAreaName(i));
                printf("\n");

                pair.second->StartRandomMotion("Idle", /*PriorityNormal*/PriorityForce, FinishedMotion, BeganMotion);

            }
        }
        //const csmInt32 count = pair.second->GetModelSetting()->GetHitAreasCount();
        //for (csmInt32 i = 0; i < count; i++)
        //{
        //    //if (pair.second->HitTest(pair.second->GetModelSetting()->GetHitAreaName(i), x, y)) {
        //        std::cout << pair.second->GetModelSetting()->GetHitAreaName(i) << std::endl;
        //    //}
        //}

        //if (pair.second->HitTest(HitAreaNameHead, x, y))
        //{
        //    printf(HitAreaNameHead);
        //    printf("\n");

        //    if (DebugLogEnable)
        //    {
        //        LAppPal::PrintLogLn("[APP]hit area: [%s]", HitAreaNameHead);
        //    }
        //    pair.second->SetRandomExpression();
        //}
        //else if (pair.second->HitTest(HitAreaNameBody, x, y))
        //{
        //    printf(HitAreaNameBody);
        //    printf("\n");
        //    if (DebugLogEnable)
        //    {
        //        LAppPal::PrintLogLn("[APP]hit area: [%s]", HitAreaNameBody);
        //    }
        //    pair.second->StartRandomMotion(/*MotionGroupTapBody*/"", PriorityNormal, FinishedMotion, BeganMotion);
        //}
    }
}

void LAppLive2DManager::OnUpdate() const
{
    int windowWidth, windowHeight;
    LAppDelegate::GetInstance()->GetClientSize(windowWidth, windowHeight);

    // D3D11 フレーム先頭処理
    // 各フレームでの、Cubism SDK の処理前にコール
    Rendering::CubismRenderer_D3D11::StartFrame(LAppDelegate::GetInstance()->GetD3dDevice(), LAppDelegate::GetInstance()->GetD3dContext(), windowWidth, windowHeight);

    for (auto& pair : _models)
    {
        // 投影用マトリックス
        CubismMatrix44 projection;
        LAppModel* model = pair.second;

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

ModelJsonConfig LAppLive2DManager::GetModelJsonConfigFromName(std::string modelname)
{
    const csmChar* modelptr = modelname.c_str();
    const csmChar* lastSlash = strrchr(modelptr, '/');
    static csmChar modelPath[DEFSIZEK] = { 0 };
    static csmChar modelJsonName[DEFSIZEK] = { 0 };
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

void RefreshTarget() {
/*
    * モデル半透明表示を行うサンプルを提示する。
    * ここでUSE_RENDER_TARGET、USE_MODEL_RENDER_TARGETが定義されている場合
    * 別のレンダリングターゲットにモデルを描画し、描画結果をテクスチャとして別のスプライトに張り付ける。
    */
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

void LAppLive2DManager::RefreshScene(int userid, std::string modelname)
{
    if (_modelDir.GetSize() <= 0)
        return;
    static const std::string defdir = "./Resources/";
    if (modelname.size() <= defdir.size()) {
        if (_models.find(userid) != _models.end()) {
            ReleaseOneModel(userid);
            std::cout << u8"Cubism 尝试删除Model" << std::endl;
        }
        else {
            std::cout << u8"Cubism 删除为空：" << modelname.c_str() << std::endl;

        }
        return;
    }
    std::cout << u8"Cubism 尝试加载：" << modelname.c_str() << std::endl;

    bool havedir = false;
    for (int i = 0; i < _modelDir.GetSize(); ++i) {
        if(strcmp(_modelDir[i].GetRawString(), modelname.c_str())==0) {
            havedir = true;
            break;
        }
    }
    if (havedir == false) {
        std::cout << u8"Cubism 加载失败，没有找到该模型：" << modelname.c_str() << std::endl;
        return;
    }

    ModelJsonConfig mjc = GetModelJsonConfigFromName(modelname);
    LAppModel* lam = new LAppModel();
    if (_models.find(userid) != _models.end())
        ReleaseOneModel(userid);
    _models[userid] = lam;

    lam->LoadAssets(mjc.modelPath.GetRawString(), mjc.modelJsonName.GetRawString());

    for (int i = 0; i < Su::UserConfig::getUserVector().size(); ++i) {
        Su::UserConfig& su = Su::UserConfig::getUserVector()[i];
        if (su.user_id == userid) {

            su.cubism_cg.cubism_ts_s = su.cubism_cg.cubism_ts_s == 0.0f ? 1.0f : su.cubism_cg.cubism_ts_s;
            su.cubism_cg.cubism_ts_x = su.cubism_cg.cubism_ts_x == 0.0f ? 1.0f : su.cubism_cg.cubism_ts_x;
            su.cubism_cg.cubism_ts_y = su.cubism_cg.cubism_ts_y == 0.0f ? 1.0f : su.cubism_cg.cubism_ts_y;
            lam->GetModelMatrix()->GetArray()[12] =su.cubism_cg.cubism_tx_t;
            lam->GetModelMatrix()->GetArray()[13] =su.cubism_cg.cubism_ty_t;
            lam->GetModelMatrix()->GetArray()[0] = su.cubism_cg.cubism_ts_x * su.cubism_cg.cubism_ts_s;
            lam->GetModelMatrix()->GetArray()[5] = su.cubism_cg.cubism_ts_y * su.cubism_cg.cubism_ts_s;
            lam->GetModelMatrix()->GetArray()[4] = su.cubism_cg.cubism_tx_s;
            lam->GetModelMatrix()->GetArray()[1] = su.cubism_cg.cubism_ty_s;
            lam->GetModelMatrix()->GetArray()[7] = su.cubism_cg.cubism_tx_p;
            lam->GetModelMatrix()->GetArray()[3] = su.cubism_cg.cubism_ty_p;
            break;
        }
    }

    std::cout << u8"Cubism 加载成功：" << modelname.c_str() << std::endl;
    std::cout << u8"用户 ID：" << userid << u8" Cubism Model：" << modelname.c_str() << std::endl;
    RefreshTarget();
}

void LAppLive2DManager::RefreshSceneAndUserId(Csm::csmUint32 pos)
{
    if (_modelDir.GetSize() <= 0)
        return;
    std::cout << u8"Cubism 归置" << std::endl;

    if (_models.find(pos) != _models.end()) {
        ReleaseOneModel(pos);
    }
    else {
    }
    static std::vector<int>vid;
    vid.clear();
    vid.reserve(_models.size());
    for (auto& pair : _models)
        vid.push_back(pair.first);
    std::sort(vid.begin(), vid.end());
    bool can_pop = false;
    for (int i = 0; i < vid.size(); ++i) {
        if (vid[i] > pos) {
            can_pop = true;
            _models[vid[i] - 1] = _models[vid[i]];
            std::cout << u8"Cubism 归置变化 i - 1 : " << vid[i] - 1 << " -> " << vid[i] << std::endl;
        }
    }
    if (can_pop)
        _models.erase(vid.back());

    std::cout << u8"更新后models" << std::endl;
    for (auto& pair : _models) {
        std::cout << pair.first << " " << pair.second->GetModel() << endl;
    }


  
}
Csm::csmVector<Csm::csmString>& LAppLive2DManager::GetModelDir()
{
    return  _modelDir;
}


csmUint32 LAppLive2DManager::GetModelNum() const
{
    return _models.size();
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
    for (auto& pair : _models) 
        pair.second->GetRenderBuffer().DestroyOffscreenSurface();
}

void LAppLive2DManager::SetViewMatrix(CubismMatrix44* m)
{
    for (int i = 0; i < 16; i++) {
        _viewMatrix->GetArray()[i] = m->GetArray()[i];
    }
}

