﻿/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppModel.hpp"
#include <fstream>
#include <vector>
#include <CubismModelSettingJson.hpp>
#include <Motion/CubismMotion.hpp>
#include <Physics/CubismPhysics.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Rendering/D3D11/CubismRenderer_D3D11.hpp>
#include <Utils/CubismString.hpp>
#include <Id/CubismIdManager.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>
#include "LAppDefine.hpp"
#include "LAppPal.hpp"
#include "LAppTextureManager.hpp"
#include "LAppDelegate.hpp"
#include "../shinobugui_src/sufunction.h"
#include <string>

using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::DefaultParameterId;
using namespace LAppDefine;

namespace {
    csmByte* CreateBuffer(const csmChar* path, csmSizeInt* size)
    {
        if (DebugLogEnable)
        {
            LAppPal::PrintLogLn("[APP]create buffer: %s ", path);
        }
        return LAppPal::LoadFileAsBytes(path, size);
    }

    void DeleteBuffer(csmByte* buffer, const csmChar* path = "")
    {
        if (DebugLogEnable)
        {
            LAppPal::PrintLogLn("[APP]delete buffer: %s", path);
        }
        LAppPal::ReleaseBytes(buffer);
    }
}
void LAppModel::InitMultiplyColor()
{
    this->set_all_m_mark = false;
    this->multiply_color[0] = 1.0f;
    this->multiply_color[1] = 1.0f;
    this->multiply_color[2] = 1.0f;
    this->multiply_color[3] = 1.0f;
    for (int i = 0; i < int(this->GetModel()->GetDrawableCount()); i++)
    {
        this->drawable_multiply_color[i][0] = this->GetModel()->GetDrawableMultiplyColor(i).X;
        this->drawable_multiply_color[i][1] = this->GetModel()->GetDrawableMultiplyColor(i).Y;
        this->drawable_multiply_color[i][2] = this->GetModel()->GetDrawableMultiplyColor(i).Z;
        this->drawable_multiply_color[i][3] = this->GetModel()->GetDrawableMultiplyColor(i).W;
    }
}

void LAppModel::InitScreenColor()
{
    this->set_all_s_mark = false;
    this->screen_color[0] = 0.0f;
    this->screen_color[1] = 0.0f;
    this->screen_color[2] = 0.0f;
    this->screen_color[3] = 1.0f;
    for (int i = 0; i < int(this->GetModel()->GetDrawableCount()); i++)
    {
        this->drawable_screen_color[i][0] = this->GetModel()->GetDrawableScreenColor(i).X;
        this->drawable_screen_color[i][1] = this->GetModel()->GetDrawableScreenColor(i).Y;
        this->drawable_screen_color[i][2] = this->GetModel()->GetDrawableScreenColor(i).Z;
        this->drawable_screen_color[i][3] = this->GetModel()->GetDrawableScreenColor(i).W;
    }
}

void LAppModel::InitPartMultiplyColor()
{
    this->set_all_mp_mark = false;
    this->multiply_group_color[0] = 1.0f;
    this->multiply_group_color[1] = 1.0f;
    this->multiply_group_color[2] = 1.0f;
    this->multiply_group_color[3] = 1.0f;
    for (int i = 0; i < this->GetModel()->GetPartCount(); i++)
    {
        this->drawable_part_multiply_color[i][0] = 1.0f;
        this->drawable_part_multiply_color[i][1] = 1.0f;
        this->drawable_part_multiply_color[i][2] = 1.0f;
        this->drawable_part_multiply_color[i][3] = 1.0f;
    }
}

void LAppModel::InitPartScreenColor()
{
    this->set_all_sp_mark = false;
    this->screen_group_color[0] = 0.0f;
    this->screen_group_color[1] = 0.0f;
    this->screen_group_color[2] = 0.0f;
    this->screen_group_color[3] = 1.0f;
    for (int i = 0; i < this->GetModel()->GetPartCount(); i++)
    {
        this->drawable_part_screen_color[i][0] = 0.0f;
        this->drawable_part_screen_color[i][1] = 0.0f;
        this->drawable_part_screen_color[i][2] = 0.0f;
        this->drawable_part_screen_color[i][3] = 1.0f;
    }
}

void LAppModel::InitPartOpacity()
{
    this->set_all_po_mark = false;
    this->group_opacity = 1.0f;
    for (int i = 0; i < this->GetModel()->GetPartCount(); i++) {
        this->drawable_part_opacity[i] = 1.0f;
    }

}

void LAppModel::StartFlashColor(int mark, int index) {
    if (index <= -1)
        return;
    float* tmpf = nullptr;
    if (mark == 0)      tmpf = drawable_multiply_color[index];
    else if (mark == 1) tmpf = drawable_screen_color[index];
    else if (mark == 2) tmpf = drawable_part_multiply_color[index];
    else if (mark == 3) tmpf = drawable_part_screen_color[index];
    else return;
    if (tmpf == nullptr)return;
    FLICKER fr;
    fr.hash_mark = std::to_string(mark) +"_" +std::to_string(index);
    fr.flash_color[0] = &(tmpf[0]); fr.flash_color[1] = &(tmpf[1]);
    fr.flash_color[2] = &(tmpf[2]); fr.flash_color[3] = &(tmpf[3]);
    fr.rgb_backup[0] = *(&tmpf[0]); fr.rgb_backup[1] = *(&tmpf[1]);
    fr.rgb_backup[2] = *(&tmpf[2]); fr.rgb_backup[3] = *(&tmpf[3]);
    bool have = false;
    for (const auto& val : flicker_v) {
        if (val.hash_mark == fr.hash_mark) {
            have = true;
            break;
        }
    }
    if (have == false) {
        std::cout << "Flicker Save Color: " << fr.rgb_backup[0] << " " << fr.rgb_backup[1] << " " << fr.rgb_backup[2] << std::endl;
        flicker_v.push_back(fr);
    }
}

void LAppModel::StartFlashOpacity(int index)
{
    if (index < 0)
        return;
    float* tmpf = nullptr;
    tmpf = &drawable_part_opacity[index];
    if (tmpf == nullptr)
        return;
    FLICKER_OPACITY fr;
    fr.hash_mark = std::to_string(index);
    fr.flash_color = &(*tmpf);
    fr.rgb_backup = *tmpf;
    bool have = false;
    for (const auto& val : flicker_opacity_v) {
        if (val.hash_mark == fr.hash_mark) {
            have = true;
            break;
        }
    }
    if (have == false) {
        std::cout << "Flicker Save Opacity: " << fr.rgb_backup << std::endl;
        flicker_opacity_v.push_back(fr);
    }
}

void LAppModel::UpdateFlashColor()
{
    for (int i = 0; i < int(flicker_v.size()); ++i) {
        FLICKER& fr =flicker_v[i];
        Su::ColorConvertHSVtoRGB(cosf(fr.flash_time * 6.0f) * 0.5f + 0.5f, 0.5f, 0.5f, *fr.flash_color[0], *fr.flash_color[1], *fr.flash_color[2]);
        *fr.flash_color[3] = 1.0f;
        if ((fr.flash_time -= (1.f / float(GlobalTemp::CubismFrameCount))) <= 0.0f) {
            std::cout << "Flicker Goback Color: " << fr.rgb_backup[0] << " " << fr.rgb_backup[1] << " " << fr.rgb_backup[2] << std::endl;
            *fr.flash_color[0] = fr.rgb_backup[0];
            *fr.flash_color[1] = fr.rgb_backup[1];
            *fr.flash_color[2] = fr.rgb_backup[2];
            *fr.flash_color[3] = fr.rgb_backup[3];
        }
        if (fr.flash_time <= 0.0f) {
            fr.flash_time = 0.0f;
            continue;
        }
    }

    flicker_v.erase(
        std::remove_if(flicker_v.begin(), flicker_v.end(),
            [](const FLICKER& fr) { return std::fabs(fr.flash_time) < ZEROFLOAT; }
        ),
        flicker_v.end()
    );
}

void LAppModel::UpdateFlashOpacity()
{
    for (int i = 0; i < int(flicker_opacity_v.size()); ++i) {
        FLICKER_OPACITY& fr = flicker_opacity_v[i];
        *fr.flash_color = fabs(sinf(asinf(fr.rgb_backup) + (1.0f-fr.flash_time) * 4.0f *acosf(-1)));
        if ((fr.flash_time -= (1.f / float(GlobalTemp::CubismFrameCount))) <= 0.0f) {
            std::cout << "Flicker Goback Opacity: " << fr.rgb_backup << std::endl;
            *fr.flash_color = fr.rgb_backup;
        }
        if (fr.flash_time <= 0.0f) {
            fr.flash_time = 0.0f;
            continue;
        }
    }
    flicker_opacity_v.erase(
        std::remove_if(flicker_opacity_v.begin(), flicker_opacity_v.end(),
            [](const FLICKER_OPACITY& fr) { return std::fabs(fr.flash_time) < ZEROFLOAT; }
        ),
        flicker_opacity_v.end()
    );
}

void LAppModel::UpdateAllColor()
{
    UpdateFlashColor();
    UpdateFlashOpacity();
    for (int i = 0; i < this->GetModel()->GetDrawableCount(); ++i) {
        this->GetModel()->GetMultiplyColorRef()[i].Color.R = this->drawable_multiply_color[i][0];
        this->GetModel()->GetMultiplyColorRef()[i].Color.G = this->drawable_multiply_color[i][1];
        this->GetModel()->GetMultiplyColorRef()[i].Color.B = this->drawable_multiply_color[i][2];
        this->GetModel()->GetMultiplyColorRef()[i].Color.A = this->drawable_multiply_color[i][3];

        this->GetModel()->GetScreenColorRef()[i].Color.R = this->drawable_screen_color[i][0];
        this->GetModel()->GetScreenColorRef()[i].Color.G = this->drawable_screen_color[i][1];
        this->GetModel()->GetScreenColorRef()[i].Color.B = this->drawable_screen_color[i][2];
        this->GetModel()->GetScreenColorRef()[i].Color.A = this->drawable_screen_color[i][3];
    }

    for (int i = 0; i < this->GetModel()->GetPartCount(); i++)
    {
        float pm_r = this->drawable_part_multiply_color[i][0];
        float pm_g = this->drawable_part_multiply_color[i][1];
        float pm_b = this->drawable_part_multiply_color[i][2];
        float pm_a = this->drawable_part_multiply_color[i][3];
        this->GetModel()->SetPartMultiplyColor(i, pm_r, pm_g, pm_b, pm_a);
        
        float ps_r = this->drawable_part_screen_color[i][0];
        float ps_g = this->drawable_part_screen_color[i][1];
        float ps_b = this->drawable_part_screen_color[i][2];
        float ps_a = this->drawable_part_screen_color[i][3];
        this->GetModel()->SetPartScreenColor(i, ps_r, ps_g, ps_b, ps_a);

        if (canOpacityGroup) {
            this->GetModel()->SetPartOpacity(i, this->drawable_part_opacity[i]);
        }
    }


}

Csm::ICubismModelSetting* LAppModel::GetModelSetting()const
{
    return _modelSetting;
}

Csm::CubismTargetPoint* LAppModel::GetModelDragManager() const
{
    return _dragManager;
}

float& LAppModel::GetLookTargetDamping()
{
    return _dragManager->TimeToMaxSpeed;
}

std::vector<LookParam>& LAppModel::GetLookTargetParams()
{
    return _lookTargetParams;
}

Csm::csmVector<Csm::CubismBreath::BreathParameterData>& LAppModel::GetBreathParameters()
{
    return _breath->GetParameters();
}

Csm::csmVector<Csm::CubismIdHandle>& LAppModel::GetEyeBlinkIds()
{
    return _eyeBlink->GetParameterIds();
}

Csm::csmFloat32& LAppModel::GetBlinkingIntervalSeconds()
{
    return _eyeBlink->GetBlinkingIntervalSeconds();
}

Csm::csmFloat32& LAppModel::GetClosingSeconds()
{
    return _eyeBlink->GetClosingSeconds();
}

Csm::csmFloat32& LAppModel::GetClosedSeconds()
{
    return _eyeBlink->GetClosedSeconds();
}

Csm::csmFloat32& LAppModel::GetOpeningSeconds()
{
    return _eyeBlink->GetOpeningSeconds();
}

bool& LAppModel::GetCanBreath()
{
    return _breath->canBreath;
}

LAppModel::LAppModel()
    : CubismUserModel()
    , _modelSetting(NULL)
    , _userTimeSeconds(0.0f)
    , _deleteModel(false)
{
    if (MocConsistencyValidationEnable)
    {
        _mocConsistency = true;
    }

    if (DebugLogEnable)
    {
        _debugMode = true;
    }
    //delete
    //_idParamAngleX = CubismFramework::GetIdManager()->GetId(ParamAngleX);
    //_idParamAngleY = CubismFramework::GetIdManager()->GetId(ParamAngleY);
    //_idParamAngleZ = CubismFramework::GetIdManager()->GetId(ParamAngleZ);
    //_idParamBodyAngleX = CubismFramework::GetIdManager()->GetId(ParamBodyAngleX);

    //_idParamEyeBallX = CubismFramework::GetIdManager()->GetId(ParamEyeBallX);
    //_idParamEyeBallY = CubismFramework::GetIdManager()->GetId(ParamEyeBallY);

    //查询缓存区
    Su::MemsetStr(this->filter_param_buf, sizeof(this->filter_param_buf));
    Su::MemsetStr(this->filter_drawable_buf, sizeof(this->filter_drawable_buf));
    Su::MemsetStr(this->filter_part_buf, sizeof(this->filter_part_buf));
}

LAppModel::~LAppModel()
{
    _renderBuffer.DestroyOffscreenSurface();

    ReleaseMotions();
    ReleaseExpressions();

    for (csmInt32 i = 0; i < _modelSetting->GetMotionGroupCount(); i++)
    {
        const csmChar* group = _modelSetting->GetMotionGroupName(i);
        ReleaseMotionGroup(group);
    }

    // テクスチャの開放
    for (csmUint32 i = 0; i < _bindTextureId.GetSize(); i++)
    {
        LAppDelegate::GetInstance()->GetTextureManager()->ReleaseTexture(_bindTextureId[i]);
    }
    _bindTextureId.Clear();

    delete _modelSetting;
    _modelSetting = nullptr;
    //删除标记触发区域纹理
    LAppTextureManager* textureManager = LAppDelegate::GetInstance()->GetTextureManager();
    for (auto& val : hitareas) {
        for (auto& las : val.second) {
            if (las == nullptr)continue;
            if (las) textureManager->ReleaseTexture(las->GetTextureId());
            delete las;
            las = NULL;
        }
    }
    //删除cdi json数据
    delete cdi_json;
    cdi_json = nullptr;
}

void LAppModel::LoadAssets(const csmChar* dir, const csmChar* fileName)
{
    _modelHomeDir = dir;

    if (_debugMode)
    {
        LAppPal::PrintLogLn("[APP]load model setting: %s", fileName);
    }

    csmSizeInt size;
    const csmString path = csmString(dir) + fileName;

    csmByte* buffer = CreateBuffer(path.GetRawString(), &size);
    ICubismModelSetting* setting = new CubismModelSettingJson(buffer, size);
    DeleteBuffer(buffer, path.GetRawString());

    SetupModel(setting);

    if (_model == NULL)
    {
        LAppPal::PrintLogLn("Failed to LoadAssets().");
        return;
    }

    CreateRenderer();

    SetupTextures();
}


void LAppModel::SetupModel(ICubismModelSetting* setting)
{
    _updating = true;
    _initialized = false;

    _modelSetting = setting;

    csmByte* buffer;
    csmSizeInt size;

    //Cubism Model
    if (strcmp(_modelSetting->GetModelFileName(), "") != 0)
    {
        csmString path = _modelSetting->GetModelFileName();
        path = _modelHomeDir + path;

        if (_debugMode)
        {
            LAppPal::PrintLogLn("[APP]create model: %s", setting->GetModelFileName());
        }

        buffer = CreateBuffer(path.GetRawString(), &size);
        LoadModel(buffer, size, _mocConsistency);
        DeleteBuffer(buffer, path.GetRawString());
    }

    //Expression
    if (_modelSetting->GetExpressionCount() > 0)
    {
        const csmInt32 count = _modelSetting->GetExpressionCount();
        for (csmInt32 i = 0; i < count; i++)
        {
            csmString name = _modelSetting->GetExpressionName(i);
            csmString path = _modelSetting->GetExpressionFileName(i);
            path = _modelHomeDir + path;

            buffer = CreateBuffer(path.GetRawString(), &size);
            ACubismMotion* motion = LoadExpression(buffer, size, name.GetRawString());

            if (motion)
            {
                if (_expressions[name] != NULL)
                {
                    ACubismMotion::Delete(_expressions[name]);
                    _expressions[name] = NULL;
                }
                _expressions[name] = motion;
            }

            DeleteBuffer(buffer, path.GetRawString());
        }
    }

    //Physics
    if (strcmp(_modelSetting->GetPhysicsFileName(), "") != 0)
    {
        csmString path = _modelSetting->GetPhysicsFileName();
        path = _modelHomeDir + path;

        buffer = CreateBuffer(path.GetRawString(), &size);
        LoadPhysics(buffer, size);
        DeleteBuffer(buffer, path.GetRawString());
    }

    //Pose
    if (strcmp(_modelSetting->GetPoseFileName(), "") != 0)
    {
        csmString path = _modelSetting->GetPoseFileName();
        path = _modelHomeDir + path;

        buffer = CreateBuffer(path.GetRawString(), &size);
        LoadPose(buffer, size);
        DeleteBuffer(buffer, path.GetRawString());
    }

    //EyeBlink
    {
        _eyeBlink = CubismEyeBlink::Create(_modelSetting);
    }

    //Breath
    {
        _breath = CubismBreath::Create();

        //csmVector<CubismBreath::BreathParameterData> breathParameters;

        //breathParameters.PushBack(CubismBreath::BreathParameterData(CubismFramework::GetIdManager()->GetId(ParamAngleX), 0.0f, 15.0f, 6.5345f, 0.5f));
        //breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamAngleY, 0.0f, 8.0f, 3.5345f, 0.5f));
        //breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamAngleZ, 0.0f, 10.0f, 5.5345f, 0.5f));
        //breathParameters.PushBack(CubismBreath::BreathParameterData(_idParamBodyAngleX, 0.0f, 4.0f, 15.5345f, 0.5f));
        //breathParameters.PushBack(CubismBreath::BreathParameterData(CubismFramework::GetIdManager()->GetId(ParamBreath), 0.5f, 0.5f, 3.2345f, 0.5f));

        //_breath->SetParameters(breathParameters);
    }

    //UserData
    if (strcmp(_modelSetting->GetUserDataFile(), "") != 0)
    {
        csmString path = _modelSetting->GetUserDataFile();
        path = _modelHomeDir + path;
        buffer = CreateBuffer(path.GetRawString(), &size);
        LoadUserData(buffer, size);
        DeleteBuffer(buffer, path.GetRawString());
    }

    //delete
    // EyeBlinkIds
    {
        //csmInt32 eyeBlinkIdCount = _modelSetting->GetEyeBlinkParameterCount();
        //for (csmInt32 i = 0; i < eyeBlinkIdCount; ++i)
        //{
        //    _eyeBlinkIds.PushBack(_modelSetting->GetEyeBlinkParameterId(i));
        //}
    }

    // LipSyncIds
    {
        csmInt32 lipSyncIdCount = _modelSetting->GetLipSyncParameterCount();
        for (csmInt32 i = 0; i < lipSyncIdCount; ++i)
        {
            _lipSyncIds.PushBack(_modelSetting->GetLipSyncParameterId(i));
        }
    }

    if (_modelSetting == NULL || _modelMatrix == NULL)
    {
        LAppPal::PrintLogLn("Failed to SetupModel().");
        return;
    }

    //Layout
    csmMap<csmString, csmFloat32> layout;
    _modelSetting->GetLayoutMap(layout);
    _modelMatrix->SetupFromLayout(layout);

    _model->SaveParameters();

    for (int paramid = 0; paramid < _model->GetParameterCount(); paramid++) {
        std::string param_str = GetModel()->GetParameterId(paramid)->GetString().GetRawString();
        if (EYEBLINK_DEF.find(param_str) != EYEBLINK_DEF.end()) {
            CubismIdHandle handle = CubismFramework::GetIdManager()->GetId(param_str.c_str());
            _eyeBlinkIdsDef.PushBack(handle);
        }
    }

    for (csmInt32 i = 0; i < _modelSetting->GetMotionGroupCount(); i++)
    {
        const csmChar* group = _modelSetting->GetMotionGroupName(i);
        PreloadMotionGroup(group);
    }

    _motionManager->StopAllMotions();

    _updating = false;
    _initialized = true;
}

void LAppModel::PreloadMotionGroup(const csmChar* group)
{
    const csmInt32 count = _modelSetting->GetMotionCount(group);

    for (csmInt32 i = 0; i < count; i++)
    {
        //ex) idle_0
        csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, i);
        csmString path = _modelSetting->GetMotionFileName(group, i);
        path = _modelHomeDir + path;

        if (_debugMode)
        {
            LAppPal::PrintLogLn("[APP]load motion: %s => [%s_%d] ", path.GetRawString(), group, i);
        }

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
        CubismMotion* tmpMotion = static_cast<CubismMotion*>(LoadMotion(buffer, size, name.GetRawString(), NULL, NULL, _modelSetting, group, i));
        if (tmpMotion)
        {
            //对于特殊的眨眼参数、需要考虑对其它组的影响
            tmpMotion->SetEffectIds(_eyeBlinkIdsDef, _lipSyncIds);
            if (_motions[name] != NULL)
            {
                ACubismMotion::Delete(_motions[name]);
            }
            _motions[name] = tmpMotion;
        }

        DeleteBuffer(buffer, path.GetRawString());
    }
}

void LAppModel::ReleaseMotionGroup(const csmChar* group) const
{
    const csmInt32 count = _modelSetting->GetMotionCount(group);
    for (csmInt32 i = 0; i < count; i++)
    {
        csmString voice = _modelSetting->GetMotionSoundFileName(group, i);
        if (strcmp(voice.GetRawString(), "") != 0)
        {
            csmString path = voice;
            path = _modelHomeDir + path;
        }
    }
}

void LAppModel::ReleaseMotions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = _motions.Begin(); iter != _motions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    _motions.Clear();
}

void LAppModel::ReleaseExpressions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = _expressions.Begin(); iter != _expressions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    _expressions.Clear();
}

void LAppModel::Update()
{
    const csmFloat32 deltaTimeSeconds = LAppPal::GetDeltaTime();
    _userTimeSeconds += deltaTimeSeconds;

    _dragManager->Update(deltaTimeSeconds);
    _dragX = _dragManager->GetX();
    _dragY = _dragManager->GetY();
    csmFloat32 _dragZ = _dragX * _dragY * -1.f;

    // モーションによるパラメータ更新の有無
    csmBool motionUpdated = false;

    //-----------------------------------------------------------------
    _model->LoadParameters(); // 前回セーブされた状態をロード
    if (_motionManager->IsFinished())
    {
        // モーションの再生がない場合、待機モーションの中からランダムで再生する
        //delete
        //StartRandomMotion(MotionGroupIdle, PriorityIdle);
        if (this->animationAutoPlay) {
            Csm::ICubismModelSetting* lcms = this->GetModelSetting();
            int  mg_count = lcms->GetMotionGroupCount();
            if (mg_count > 0) {
                int gno = rand() % mg_count;
                int m_count = lcms->GetMotionCount(lcms->GetMotionGroupName(gno));
                if (m_count > 0) {
                    int no = rand() % m_count;
                    const Csm::csmChar* gid = lcms->GetMotionGroupName(gno);
                    this->StartMotion(gid, no, LAppDefine::PriorityForce);
                }
            }
            int e_count = lcms->GetExpressionCount();
            if (e_count > 0) {
                int no = rand() % e_count;
                this->SetExpression(lcms->GetExpressionName(no));
            }
        }
    }
    else
    {
        motionUpdated = _motionManager->UpdateMotion(_model, deltaTimeSeconds); // モーションを更新
    }
    _model->SaveParameters(); // 状態を保存
    //-----------------------------------------------------------------

    // 不透明度
    _opacity = _model->GetModelOpacity();

    // まばたき
    if (!motionUpdated)
    {
        if (_eyeBlink != NULL)
        {
            // メインモーションの更新がないとき
            if(canEyeBlink)
                _eyeBlink->UpdateParameters(_model, deltaTimeSeconds); // 目パチ
        }
    }

    if (_expressionManager != NULL)
    {
        _expressionManager->UpdateMotion(_model, deltaTimeSeconds); // 表情でパラメータ更新（相対変化）
    }

    //ドラッグによる変化
    //ドラッグによる顔の向きの調整
    //_model->AddParameterValue(_idParamAngleX, _dragX * 30); // -30から30の値を加える
    //_model->AddParameterValue(_idParamAngleY, _dragY * 30);
    //_model->AddParameterValue(_idParamAngleZ, _dragZ * 30);

    ////ドラッグによる体の向きの調整
    //_model->AddParameterValue(_idParamBodyAngleX, _dragX * 10); // -10から10の値を加える

    ////ドラッグによる目の向きの調整
    //_model->AddParameterValue(_idParamEyeBallX, _dragX); // -1から1の値を加える
    //_model->AddParameterValue(_idParamEyeBallY, _dragY);

    for (int i = 0; i < int(_lookTargetParams.size()); ++i) {
        if (_lookTargetParams[i].enable) {
            if (_lookTargetParams[i].xyzpos == 0) {
                _model->AddParameterValue(_lookTargetParams[i].cid, _dragX * _lookTargetParams[i].param);

            }
            else if (_lookTargetParams[i].xyzpos == 1) {
                _model->AddParameterValue(_lookTargetParams[i].cid, _dragY * _lookTargetParams[i].param);

            }
            else if (_lookTargetParams[i].xyzpos == 2) {
                _model->AddParameterValue(_lookTargetParams[i].cid, _dragZ *_lookTargetParams[i].param);

            }

        }
    }

    // 呼吸など
    if (_breath != NULL)
    {
        _breath->UpdateParameters(_model, deltaTimeSeconds);
    }

    // 物理演算の設定
    if (_physics != NULL)
    {
        _physics->Evaluate(_model, deltaTimeSeconds);
    }

    // リップシンクの設定
    if (_lipSync)
    {
        // リアルタイムでリップシンクを行う場合、システムから音量を取得して0〜1の範囲で値を入力します。
        csmFloat32 value = 0.0f;

        // 状態更新/RMS値取得
        _wavFileHandler.Update(deltaTimeSeconds);
        value = _wavFileHandler.GetRms();

        for (csmUint32 i = 0; i < _lipSyncIds.GetSize(); ++i)
        {
            _model->AddParameterValue(_lipSyncIds[i], value, 0.8f);
        }
    }

    // ポーズの設定
    if (_pose != NULL)
    {
        _pose->UpdateParameters(_model, deltaTimeSeconds);
    }

    //状态观测
    this->add_t += deltaTimeSeconds;
    for (int i = 0; i < this->GetModel()->GetParameterCount(); ++i) {
        Su::ShinobuScrollingBuffer& sdata1 = this->sdatal_v[i];
        sdata1.AddPoint(this->add_t, this->GetModel()->GetParameterValue(i));
    }

    _model->Update();

}

CubismMotionQueueEntryHandle LAppModel::StartMotion(const csmChar* group, csmInt32 no, csmInt32 priority,
                                            ACubismMotion::FinishedMotionCallback onFinishedMotionHandler, ACubismMotion::BeganMotionCallback onBeganMotionHandler)
{
    if (priority == PriorityForce)
    {
        _motionManager->SetReservePriority(priority);
    }
    else if (!_motionManager->ReserveMotion(priority))
    {
        if (_debugMode)
        {
            LAppPal::PrintLogLn("[APP]can't start motion.");
        }
        return InvalidMotionQueueEntryHandleValue;
    }
    const csmString fileName = _modelSetting->GetMotionFileName(group, no);


    //ex) idle_0
    csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, no);
    CubismMotion* motion = static_cast<CubismMotion*>(_motions[name.GetRawString()]);
    csmBool autoDelete = false;

    if (motion == NULL)
    {
        csmString path = fileName;
        path = _modelHomeDir + path;

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
        motion = static_cast<CubismMotion*>(LoadMotion(buffer, size, NULL, onFinishedMotionHandler, onBeganMotionHandler, _modelSetting, group, no));

        if (motion)
        {
            //Shinobu Debug
            motion->SetEffectIds(this->_eyeBlinkIdsDef, _lipSyncIds);
            autoDelete = true; // 終了時にメモリから削除
        }

        DeleteBuffer(buffer, path.GetRawString());
    }
    else
    {
        motion->SetBeganMotionHandler(onBeganMotionHandler);
        motion->SetFinishedMotionHandler(onFinishedMotionHandler);
    }

    //voice
    csmString voice = _modelSetting->GetMotionSoundFileName(group, no);
    if (strcmp(voice.GetRawString(), "") != 0)
    {
        csmString path = voice;
        path = _modelHomeDir + path;
        _wavFileHandler.Start(path);
    }

    if (_debugMode)
    {
        LAppPal::PrintLogLn("[APP]start motion: [%s_%d]", group, no);
    }

    this->current_motion_name = fileName.GetRawString();
    return  _motionManager->StartMotionPriority(motion, autoDelete, priority);
}

CubismMotionQueueEntryHandle LAppModel::StartRandomMotion(const csmChar* group, csmInt32 priority,
                                            ACubismMotion::FinishedMotionCallback onFinishedMotionHandler, ACubismMotion::BeganMotionCallback onBeganMotionHandler)
{
    if (_modelSetting->GetMotionCount(group) == 0)
    {
        return InvalidMotionQueueEntryHandleValue;
    }

    csmInt32 no = rand() % _modelSetting->GetMotionCount(group);

    return StartMotion(group, no, priority, onFinishedMotionHandler, onBeganMotionHandler);
}

void LAppModel::DoDraw()
{
    GetRenderer<Rendering::CubismRenderer_D3D11>()->DrawModel();
}

void LAppModel::Draw(Csm::CubismMatrix44& matrix)
{
    Rendering::CubismRenderer_D3D11* renderer = GetRenderer<Rendering::CubismRenderer_D3D11>();

    if(_model == NULL || _deleteModel || renderer==NULL)
    {
        return;
    }
    //更新Color数据
    this->UpdateAllColor();
    // 投影行列と乗算
    matrix.MultiplyByMatrix(_modelMatrix);

    renderer->SetMvpMatrix(&matrix);

    DoDraw();
}

csmBool LAppModel::HitTest(const csmChar* hitAreaName, csmFloat32 x, csmFloat32 y)
{
    // 透明時は当たり判定なし。
    if (_opacity < 1)
    {
        return false;
    }
    const csmInt32 count = _modelSetting->GetHitAreasCount();
    for (csmInt32 i = 0; i < count; i++)
    {
        if (strcmp(_modelSetting->GetHitAreaName(i), hitAreaName) == 0)
        {
            const CubismIdHandle drawID = _modelSetting->GetHitAreaId(i);
            bool hit_return = IsHit(drawID, x, y);
            if (hit_return) {
                if (this->hit_id.compare(drawID->GetString().GetRawString()) == 0)
                    this->hit_num++;
                else
                    this->hit_num = 1;
                this->hit_id=drawID->GetString().GetRawString();
            }
            return hit_return;
        }
    }
    return false; // 存在しない場合はfalse
}

void LAppModel::SetExpression(const csmChar* expressionID)
{
    ACubismMotion* motion = _expressions[expressionID];
    if (_debugMode)
    {
        LAppPal::PrintLogLn("[APP]expression: [%s]", expressionID);
    }
    this->current_expression_name = expressionID;
    if (motion != NULL)
    {
        _expressionManager->StartMotion(motion, false);
    }
    else
    {
        if (_debugMode)
        {
            LAppPal::PrintLogLn("[APP]expression[%s] is null ", expressionID);
        }
    }
}
void LAppModel::SetRandomExpression()
{
    if (_expressions.GetSize() == 0)
    {
        return;
    }

    csmInt32 no = rand() % _expressions.GetSize();
    csmMap<csmString, ACubismMotion*>::const_iterator map_ite;
    csmInt32 i = 0;
    for (map_ite = _expressions.Begin(); map_ite != _expressions.End(); map_ite++)
    {
        if (i == no)
        {
            csmString name = (*map_ite).First;
            SetExpression(name.GetRawString());
            return;
        }
        i++;
    }
}

void LAppModel::ReloadRenderer()
{
    DeleteRenderer();

    CreateRenderer();

    SetupTextures();
}

void LAppModel::SetupTextures()
{
#ifdef PREMULTIPLIED_ALPHA_ENABLE
    const bool isPreMult = true;
    // αが合成されていないテクスチャを無理矢理ここで合成する実験を行う場合はtrueにする
    const bool isTextureMult = false;
#else
    const bool isPreMult = false;
    const bool isTextureMult = false;
#endif

    _bindTextureId.Clear();

    for (csmInt32 modelTextureNumber = 0; modelTextureNumber < _modelSetting->GetTextureCount(); modelTextureNumber++)
    {
        // テクスチャ名が空文字だった場合はロード・バインド処理をスキップ
        if (strcmp(_modelSetting->GetTextureFileName(modelTextureNumber), "") == 0)
        {
            continue;
        }

        //テクスチャをロードする
        csmString texturePath = _modelSetting->GetTextureFileName(modelTextureNumber);
        texturePath = _modelHomeDir + texturePath;

        LAppTextureManager::TextureInfo* texture = LAppDelegate::GetInstance()->GetTextureManager()->CreateTextureFromPngFile(texturePath.GetRawString(), isTextureMult);

        //
        if (texture)
        {
            const csmUint64 textureManageId = texture->id;

            ID3D11ShaderResourceView* textureView = NULL;
            if (LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(textureManageId, textureView))
            {
                GetRenderer<Rendering::CubismRenderer_D3D11>()->BindTexture(modelTextureNumber, textureView);
                _bindTextureId.PushBack(textureManageId);
            }
        }
    }

    // premultであるなら設定
    GetRenderer<Rendering::CubismRenderer_D3D11>()->IsPremultipliedAlpha(isPreMult);
}

void LAppModel::MotionEventFired(const csmString& eventValue)
{
    CubismLogInfo("%s is fired on LAppModel!!", eventValue.GetRawString());
}


Csm::Rendering::CubismOffscreenSurface_D3D11& LAppModel::GetRenderBuffer()
{
    return _renderBuffer;
}

csmBool LAppModel::HasMocConsistencyFromFile(const csmChar* mocFileName)
{
    CSM_ASSERT(strcmp(mocFileName, ""));

    csmByte* buffer;
    csmSizeInt size;

    csmString path = mocFileName;
    path = _modelHomeDir + path;

    buffer = CreateBuffer(path.GetRawString(), &size);

    csmBool consistency = CubismMoc::HasMocConsistencyFromUnrevivedMoc(buffer, size);
    if (!consistency)
    {
        CubismLogInfo("Inconsistent MOC3.");
    }
    else
    {
        CubismLogInfo("Consistent MOC3.");
    }

    DeleteBuffer(buffer);

    return consistency;
}
