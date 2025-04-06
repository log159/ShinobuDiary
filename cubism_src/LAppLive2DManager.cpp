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
#include <CubismDefaultParameterId.hpp>
#include <./Id/CubismIdManager.hpp>
#include <CubismCdiJson.hpp>
#include <iostream>
#include <fstream>

using namespace Csm;
using namespace LAppDefine;
namespace Csmd = Live2D::Cubism::Framework::DefaultParameterId;
using namespace Csmd;

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
            _models.erase(userid);
            break;
        }
    }
}

void LAppLive2DManager::SetUpModel()
{

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
        std::cout << _modelDir[i].GetRawString() << std::endl;
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

std::unordered_map<int, LAppModel*>& LAppLive2DManager::GetModel()
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
        LAppModel* lam = pair.second;
        if (lam->canHitareas == false)
            continue;
        Csm::ICubismModelSetting* lms = pair.second->GetModelSetting();
        for (int i = 0; i < lms->GetHitAreasCount(); ++i) {
            if (pair.second->HitTest(lms->GetHitAreaName(i), x, y)) {
                lam->hit_name = lms->GetHitAreaName(i);

                ImVector<ImGuiID>& ivm = lam->hit_areas_motion_map[lam->GetModelSetting()->GetHitAreaId(i)->GetString().GetRawString()].Items[1];
                ImVector<ImGuiID>& ive = lam->hit_areas_expression_map[lam->GetModelSetting()->GetHitAreaId(i)->GetString().GetRawString()].Items[1];
                int p1=-1, p2=-1;
                std::cout << u8"触发动作----------------------------------------" << std::endl;
                if (ivm.Size > 0) {
                    std::pair<int, int>& pr=lam->motion_map[ivm[rand() % ivm.Size]];
                    const Csm::csmChar* gid = lam->GetModelSetting()->GetMotionGroupName(pr.first);
                    lam->StartMotion(gid, pr.second, LAppDefine::PriorityForce);
                    lam->hit_motion_name = lam->GetModelSetting()->GetMotionFileName(gid, pr.second);
                    std::cout << gid << u8" " << lam->hit_motion_name << std::endl;
                }
                if (ive.Size > 0) {
                    int ep = lam->expression_map[ive[rand() % ive.Size]];
                    lam->SetExpression(lam->GetModelSetting()->GetExpressionName(ep));
                    lam->hit_expression_name = lam->GetModelSetting()->GetExpressionName(ep);
                    std::cout << lam->hit_expression_name << std::endl;
                }
                std::cout << u8"Hit ID " << lam->GetModelSetting()->GetHitAreaId(i)->GetString().GetRawString() << " " << lam->hit_name << std::endl;
            }
        }
    }
}

void LAppLive2DManager::OnUpdate() const
{
    int windowWidth, windowHeight;
    LAppDelegate::GetInstance()->GetClientSize(windowWidth, windowHeight);
    ID3D11DeviceContext* renderContext = LAppDelegate::GetD3dContext();
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
    std::cout << "Change Cubism " << modelPath << " " << modelJsonName << std::endl;
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
    static const std::string defdir = ResourcesPath;
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
    for (int i = 0; i < int(_modelDir.GetSize()); ++i) {
        if(strcmp(_modelDir[i].GetRawString(), modelname.c_str())==0) {
            havedir = true;break;
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
    lam->LoadAssets(mjc.modelPath.GetRawString(), mjc.modelJsonName.GetRawString());

    //----------------------------------------------------------------------------------------INIT配置
    Su::UserConfig* uc = nullptr;
    for (int i = 0; i < int(Su::UserConfig::getUserVector().size()); ++i) {
        if (Su::UserConfig::getUserVector()[i].user_id == userid) {
            uc = &(Su::UserConfig::getUserVector()[i]);
            break;
        }
    }
    if (uc == nullptr) {
        std::cout << u8"UserConfig为空或无法找到对应用户 无法初始化Cubism参数" << std::endl;
        return;
    }

    if (uc->need_init_cubism) std::cout << u8"完全初始化" << std::endl;
    else std::cout << u8"预期初始化" << std::endl;
    //完全初始化--------------------------------------------------------------------------------
    //几何
    lam->def_cubism_cg.cubism_ts_s = 1.0f;
    lam->def_cubism_cg.cubism_tx_t = lam->GetModelMatrix()->GetArray()[12];
    lam->def_cubism_cg.cubism_ty_t = lam->GetModelMatrix()->GetArray()[13];
    lam->def_cubism_cg.cubism_ts_x = lam->GetModelMatrix()->GetArray()[0];
    lam->def_cubism_cg.cubism_ts_y = lam->GetModelMatrix()->GetArray()[5];
    lam->def_cubism_cg.cubism_tx_s = lam->GetModelMatrix()->GetArray()[4];
    lam->def_cubism_cg.cubism_ty_s = lam->GetModelMatrix()->GetArray()[1];
    lam->def_cubism_cg.cubism_tx_p = lam->GetModelMatrix()->GetArray()[7];
    lam->def_cubism_cg.cubism_ty_p = lam->GetModelMatrix()->GetArray()[3];
    lam->modelSize = lam->def_cubism_cg.cubism_ts_s;
    lam->modelSizeX = lam->GetModelMatrix()->GetArray()[0];
    lam->modelSizeY = lam->GetModelMatrix()->GetArray()[5];

    //目标看向
    lam->canLookMouse = true;
    lam->GetLookTargetDamping() = 0.15f;
    lam->GetLookTargetParams().clear();
    //呼吸
    lam->GetCanBreath() = true;
    lam->GetBreathParameters().Clear();
    for (int paramid = 0; paramid < lam->GetModel()->GetParameterCount(); paramid++) {
        const char* pid = lam->GetModel()->GetParameterId(paramid)->GetString().GetRawString();
        const Csm::CubismId* cid = CubismFramework::GetIdManager()->GetId(pid);
        std::string cid_str = std::string(cid->GetString().GetRawString());
        //目标看向ITEM
        do {
            LookParam lp;
            lp.cid = cid;
            if (PARAM_DEF.find(cid_str) != PARAM_DEF.end()) {
                lp.enable = true;
                lp.param = PARAM_DEF.at(cid_str).first;
                lp.xyzpos = PARAM_DEF.at(cid_str).second;
            }
            lam->def_look_target_params.push_back(lp);
        } while (0);
        //呼吸ITEM
        do {
            CubismBreath::BreathParameterData bp;
            bp.ParameterId = cid;
            if (BREATH_DEF.find(cid_str) != BREATH_DEF.end()) {
                BreathParam bit=BREATH_DEF.at(cid_str);
                bp.Enable = true;
                bp.Offset = bit.Offset;bp.Peak = bit.Peak;bp.Cycle = bit.Cycle;bp.Weight = bit.Weight;
            }
            else {
                bp.Enable = false;bp.Offset = 0.0f;bp.Peak = 0.0f;bp.Cycle = 3.0f;bp.Weight = 0.5f;
            }
            lam->def_breath_params.push_back(bp);
        } while (0);
    }
    for (const LookParam& val : lam->def_look_target_params)
        lam->GetLookTargetParams().push_back(val);
    for (const Csm::CubismBreath::BreathParameterData& val : lam->def_breath_params)
        lam->GetBreathParameters().PushBack(val);

    //自动眨眼
    lam->canEyeBlink = true;
    lam->blink_sel_list.Items[0].clear();
    lam->blink_sel_list.Items[1].clear();
    lam->GetEyeBlinkIds().Clear();
    lam->GetBlinkingIntervalSeconds() = 4.0f;
    lam->GetClosingSeconds() = 0.10f;
    lam->GetClosedSeconds() = 0.05f;
    lam->GetOpeningSeconds() = 0.15f;

    for (int paramid = 0; paramid < lam->GetModel()->GetParameterCount(); paramid++) {
        std::string param_str = lam->GetModel()->GetParameterId(paramid)->GetString().GetRawString();
        if (EYEBLINK_DEF.find(param_str) != EYEBLINK_DEF.end())
            lam->def_blink_list_ids[1].push_back((ImGuiID)paramid);
        else
            lam->def_blink_list_ids[0].push_back((ImGuiID)paramid);
    }
    for (const ImGuiID& pid: lam->def_blink_list_ids[0]) 
        lam->blink_sel_list.Items[0].push_back((ImGuiID)pid);
    for (const ImGuiID& pid : lam->def_blink_list_ids[1])
        lam->blink_sel_list.Items[1].push_back((ImGuiID)pid);
    for (const ImGuiID& pid : lam->def_blink_list_ids[1]) {
        std::string param_str = lam->GetModel()->GetParameterId(pid)->GetString().GetRawString();
        lam->GetEyeBlinkIds().PushBack(CubismFramework::GetIdManager()->GetId(param_str.c_str()));
    }

    //动画
    lam->animationAutoPlay = false;

    //触发
    lam->canHitareas = true;
    lam->previewHitareas = false;
    Csm::ICubismModelSetting* lcms = lam->GetModelSetting();
    CubismModel* lamcm = lam->GetModel();
    LAppTextureManager* textureManager = LAppDelegate::GetInstance()->GetTextureManager();
    ID3D11Device* device = LAppDelegate::GetInstance()->GetD3dDevice();
    LAppSpriteShader* shader = LAppDelegate::GetInstance()->GetView()->GetShader();
    for (int i = 0; i < lcms->GetHitAreasCount(); ++i) {
        const CubismIdHandle drawID = lcms->GetHitAreaId(i);
        const csmChar* hitName = lcms->GetHitAreaName(i);
        const csmInt32 drawIndex = lamcm->GetDrawableIndex(drawID);
        const csmInt32 count = lamcm->GetDrawableVertexCount(drawIndex);
        const csmFloat32* vertices = lamcm->GetDrawableVertices(drawIndex);
        csmFloat32 left = vertices[0];
        csmFloat32 right = vertices[0];
        csmFloat32 top = vertices[1];
        csmFloat32 bottom = vertices[1];
        for (csmInt32 j = 1; j < count; ++j)
        {
            csmFloat32 x = vertices[Constant::VertexOffset + j * Constant::VertexStep];
            csmFloat32 y = vertices[Constant::VertexOffset + j * Constant::VertexStep + 1];
            if (x < left) { left = x; }
            if (x > right) { right = x; }
            if (y < top) { top = y; }
            if (y > bottom) { bottom = y; }
        }
        Csm::CubismMatrix44* viewcm4 = LAppDelegate::GetInstance()->GetView()->GetDeviceToScreen();
        Csm::CubismMatrix44* lamcm4 = lam->GetModelMatrix();
        const csmFloat32 gx_l = viewcm4->InvertTransformX(lamcm4->TransformX(left));
        const csmFloat32 gx_r = viewcm4->InvertTransformX(lamcm4->TransformX(right));
        const csmFloat32 gy_t = LAppDefine::RenderTargetHeight - viewcm4->InvertTransformY(lamcm4->TransformY(bottom));
        const csmFloat32 gy_b = LAppDefine::RenderTargetHeight - viewcm4->InvertTransformY(lamcm4->TransformY(top));
        std::vector<LAppSprite*> vls;
        float rectWidth = gx_r - gx_l;
        float rectHeight = gy_t - gy_b;
        float lineThickness = 3.0; // 线条厚度
        LAppTextureManager::TextureInfo* topTexture = textureManager->CreateTextureFromRGBA(1.0, 0.0, 0.0, 1.0, UINT(rectWidth), UINT(lineThickness));
        vls.push_back(new LAppSprite(gx_l + rectWidth * 0.5f, gy_t - lineThickness * 0.5f, rectWidth, lineThickness, topTexture->id, shader, device));
        LAppTextureManager::TextureInfo* bottomTexture = textureManager->CreateTextureFromRGBA(1.0, 0.0, 0.0, 1.0, UINT(rectWidth), UINT(lineThickness));
        vls.push_back(new LAppSprite(gx_l + rectWidth * 0.5f, gy_b + lineThickness * 0.5f, rectWidth, lineThickness, bottomTexture->id, shader, device));
        LAppTextureManager::TextureInfo* leftTexture = textureManager->CreateTextureFromRGBA(1.0, 0.0, 0.0, 1.0, UINT(lineThickness), UINT(rectHeight));
        vls.push_back(new LAppSprite(gx_l + lineThickness * 0.5f, gy_b + rectHeight * 0.5f, lineThickness, rectHeight, leftTexture->id, shader, device));
        LAppTextureManager::TextureInfo* rightTexture = textureManager->CreateTextureFromRGBA(1.0, 0.0, 0.0, 1.0, UINT(lineThickness), UINT(rectHeight));
        vls.push_back(new LAppSprite(gx_r - lineThickness * 0.5f, gy_b + rectHeight * 0.5f, lineThickness, rectHeight, rightTexture->id, shader, device));
        lam->hitareas[std::string(drawID->GetString().GetRawString())] = vls;
    }
    for (int i = 0; i<lcms->GetHitAreasCount();i++)
    {
        Su::ShinobuExList& cm_sel=lam->hit_areas_motion_map[lcms->GetHitAreaId(i)->GetString().GetRawString()];
        cm_sel.Items[0].clear(); cm_sel.Items[1].clear();
        int m_pos = 0;
        for (int i = 0; i < lam->GetModelSetting()->GetMotionGroupCount(); ++i) {
            for (int j = 0; j < lcms->GetMotionCount(lam->GetModelSetting()->GetMotionGroupName(i)); ++j) {
                lam->motion_map[m_pos] = std::pair<int, int>(i, j);
                cm_sel.Items[1].push_back((ImGuiID)m_pos);
                m_pos++;
            }
        }
        Su::ShinobuExList& ce_sel = lam->hit_areas_expression_map[lcms->GetHitAreaId(i)->GetString().GetRawString()];
        ce_sel.Items[0].clear(); ce_sel.Items[1].clear();
        for (int i = 0; i < lam->GetModelSetting()->GetExpressionCount(); ++i) {
            lam->expression_map[i] = i;
            ce_sel.Items[1].push_back((ImGuiID)i);
        }
    }
    
    //正片叠底
    lam->InitMultiplyColor();
    //屏幕色
    lam->InitScreenColor();
    //正片叠底组
    lam->InitPartMultiplyColor();
    //屏幕色组
    lam->InitPartScreenColor();
    //透明组
    lam->canOpacityGroup = false;
    lam->InitPartOpacity();

    //观测
    lam->sdatal_v.resize(lam->GetModel()->GetParameterCount());

    std::string cdifile = mjc.modelPath.GetRawString();
    cdifile += lam->GetModelSetting()->GetDisplayInfoFileName();
    std::cout << cdifile << std::endl;
    std::ifstream file(cdifile, std::ios::binary | std::ios::ate); 
    if (!file) {
        std::cout << u8"无法打开cdi文件或没有cdi文件: " << cdifile << std::endl;
    }
    else {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        unsigned char* buffer = new unsigned char[static_cast<int>(size)];
        if (file.read(reinterpret_cast<char*>(buffer), size)) {
            std::cout << u8"成功读取 JSON 文件，大小: " << size << u8" 字节" << std::endl;
            lam->cdi_json = new CubismCdiJson(buffer, static_cast<csmSizeInt>(size));
            lam->cdi_exist = true;
        }
        else {
            std::cout << u8"读取文件失败" << std::endl;
        }
        delete[] buffer;
        buffer = nullptr;
    }

    uc->need_init_cubism = false;
    _models[userid] = lam;
    Su::UserConfigSave(uc);
    //----------------------------------------------------------------------------------------INIT结束
    std::cout << u8"Cubism 加载成功：" << modelname.c_str() << std::endl;
    std::cout << u8"用户 ID：" << userid << u8" Cubism Model：" << modelname.c_str() << std::endl;
    RefreshTarget();
}

void LAppLive2DManager::RefreshSceneAndUserId(int pos)
{
    if (_modelDir.GetSize() <= 0)
        return;
    std::cout << u8"Cubism 归置" << std::endl;

    if (_models.find(pos) != _models.end()) {
        ReleaseOneModel(pos);
    }
    std::unordered_map<int, LAppModel*> new_models;
    for (auto it = _models.begin(); it != _models.end(); ++it) {
        int index = it->first;
        if (index > pos)index--;
        new_models[index] = it->second;
    }
    _models.clear();
    _models = new_models;
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

