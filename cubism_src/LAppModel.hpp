/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <ICubismModelSetting.hpp>
#include <Type/csmRectF.hpp>
#include <Rendering/D3D11/CubismOffscreenSurface_D3D11.hpp>
#include <./Effect/CubismBreath.hpp>
#include <./Id/CubismId.hpp>
#include <unordered_map>
#include <unordered_set>
#include "../native_src/include/Live2DCubismCore.h"
#include "LAppWavFileHandler.hpp"
#include "../global.h"
#include "./LAppSprite.hpp"
#include "./LAppTextureManager.hpp"

namespace Csc = Live2D::Cubism::Core;
/**
 * @brief ユーザーが実際に使用するモデルの実装クラス<br>
 *         モデル生成、機能コンポーネント生成、更新処理とレンダリングの呼び出しを行う。
 *
 */

class LookParam{
public:
    const Csm::CubismId*    cid;
    bool                    enable;
    float                   param;
    int                     xyzpos;
    LookParam() :
        cid(nullptr),
        enable(false),
        param(0.0f),
        xyzpos(0)
    {}
};

static const char* XYZAXIS[3] = { "X","Y","Z" };
static const std::unordered_map<std::string, std::pair<float,int>>PARAM_DEF = {
    {"ParamAngleX",{30.f,0}},
    {"PARAM_ANGLE_X",{30.f,0}},
    {"ParamAngleY",{30.f,1}},
    {"PARAM_ANGLE_Y",{30.f,1}},
    {"ParamAngleZ",{1.f,2}},
    {"PARAM_ANGLE_Z",{1.f,2}},
    {"ParamBodyAngleX",{10.f,0}},
    {"PARAM_BODY_ANGLE_X",{10.f,0}},
    {"ParamEyeBallX",{1.f,0}},
    {"PARAM_EYE_BALL_X",{1.f,0}},
    {"ParamEyeBallY",{1.f,1}},
    {"PARAM_EYE_BALL_Y",{1.f,1}},
};
class BreathParam {
public:
    float Offset;
    float Peak;
    float Cycle;
    float Weight;
    BreathParam(float o, float p, float c, float w):
    Offset(o),
    Peak(p),
    Cycle(c),
    Weight(w)
    {}
};
static const std::unordered_map<std::string, BreathParam>BREATH_DEF = {
    {"ParamAngleX",BreathParam(0.0f, 15.0f, 6.5345f, 0.5f)},
    {"PARAM_ANGLE_X",BreathParam(0.0f, 15.0f, 6.5345f, 0.5f)},
    {"ParamAngleY",BreathParam(0.0f, 8.0f, 3.5345f, 0.5f)},
    {"PARAM_ANGLE_Y",BreathParam(0.0f, 8.0f, 3.5345f, 0.5f)},
    {"ParamAngleZ",BreathParam(0.0f, 10.0f, 5.5345f, 0.5f)},
    {"PARAM_ANGLE_Z",BreathParam(0.0f, 10.0f, 5.5345f, 0.5f)},
    {"ParamBodyAngleX",BreathParam(0.0f, 4.0f, 15.5345f, 0.5f)},
    {"PARAM_BODY_ANGLE_X",BreathParam(0.0f, 4.0f, 15.5345f, 0.5f)},
    {"ParamBreath",BreathParam(0.5f, 0.5f, 3.2345f, 0.5f)},
    {"PARAM_BREATH",BreathParam(0.5f, 0.5f, 3.2345f, 0.5f)},
};

static const std::unordered_set<std::string> EYEBLINK_DEF = {
    {"ParamEyeROpen"},
    {"ParamEyeLOpen" },
    {"PARAM_EYE_L_OPEN"},
    {"PARAM_EYE_R_OPEN"},
};
struct CCG {
    //cubism geometry
    float cubism_ts_s;
    float cubism_tx_t;
    float cubism_ty_t;
    float cubism_ts_x;
    float cubism_ts_y;
    float cubism_tx_s;
    float cubism_ty_s;
    float cubism_tx_p;
    float cubism_ty_p;
};

class LAppModel : public Csm::CubismUserModel
{
private:
    bool                                            canFlash            = false;
    float*                                          flashColor[4]       = { nullptr };
    float                                           flashTime           = 0.0f;
    float                                           flashing            = false;
    float                                           rgb_backup[4]       = { 0.0f };
public:
    long long                                       hit_num             = 0;
    std::string                                     hit_name;
    std::string                                     hit_id;
    int                                             hit_ui_selected     = -1;
    int                                             function_selected   = 0;
    std::unordered_map<int, std::pair<int, int>>    motion_map;
    std::unordered_map<int, int>                    expression_map;
    std::string                                     hit_motion_name;
    std::string                                     hit_expression_name;
    float                                           multiply_group_color[4] = { 1.0f,1.0f, 1.0f, 1.0f };
    float                                           screen_group_color[4]   = { 0.0f,0.0f, 0.0f, 1.0f };
    bool                                            set_all_mp_mark = false;
    bool                                            set_all_sp_mark = false;
public:
    CCG                                                 def_cubism_cg;
    std::vector<LookParam>                              def_look_target_params;
    std::vector<Csm::CubismBreath::BreathParameterData> def_breath_params;
    std::vector<ImGuiID>                                def_blink_list_ids;

public:
    bool                canLookMouse                                = false;    //是否看向
    bool                canEyeBlink                                 = false;    //是否自动眨眼
    bool                previewHitareas                             = false;    //预览触发区域
    bool                animationAutoPlay                           = false;    //启用动画自动播放
    float               drawable_multiply_color[DEFSIZEK16][4]      = { 0.f };  //正片叠底参数
    float               drawable_screen_color[DEFSIZEK16][4]        = { 0.f};   //屏幕色参数
    float               drawable_part_multiply_color[DEFSIZEK16][4] = { 0.f };  //正片叠底组参数
    float               drawable_part_screen_color[DEFSIZEK16][4]   = { 0.f };  //屏幕色组参数
    std::unordered_map<std::string, std::vector<LAppSprite*>>       hitareas;   //点击触发预览区域 对应 N * 4 个矩形
    std::unordered_map<std::string, Su::ShinobuExList>              hit_areas_motion_map;
    std::unordered_map<std::string, Su::ShinobuExList>              hit_areas_expression_map;

    void InitMultiplyColor();
    void InitScreenColor();
    void InitPartMultiplyColor();
    void InitPartScreenColor();

    void StartFlashColor(int mark,int index);
    void UpdateFlashColor();
    void UpdateAllColor();

    Csm::ICubismModelSetting*   GetModelSetting() const;
    Csm::CubismTargetPoint*     GetModelDragManager()const;
    float&                      GetLookTargetDamping();                             //看向目标阻尼
    bool&                       GetCanBreath();                                     //是否呼吸
    Csm::csmFloat32&            GetBlinkingIntervalSeconds();                       //眨眼的时间间隔
    Csm::csmFloat32&            GetClosingSeconds();                                //闭眼所需的时间
    Csm::csmFloat32&            GetClosedSeconds();                                 //完全闭合状态的持续时间
    Csm::csmFloat32&            GetOpeningSeconds();                                //闭合到完全睁开的过渡时间
    std::vector<LookParam>&                 GetLookTargetParams();                  //看向参数
    Csm::csmVector<Csm::CubismIdHandle>&    GetEyeBlinkIds();                       //眨眼参数
    Csm::csmVector<Csm::CubismBreath::BreathParameterData>& GetBreathParameters();  //呼吸参数
    /**
     * @brief コンストラクタ
     */
    LAppModel();

    /**
     * @brief デストラクタ
     *
     */
    virtual ~LAppModel();

    /**
     * @brief model3.jsonが置かれたディレクトリとファイルパスからモデルを生成する
     *
     */
    void LoadAssets(const Csm::csmChar* dir, const  Csm::csmChar* fileName);

    /**
     * @brief レンダラを再構築する
     *
     */
    void ReloadRenderer();

    /**
     * @brief   モデルの更新処理。モデルのパラメータから描画状態を決定する。
     *
     */
    void Update();

    /**
     * @brief   モデルを描画する処理。モデルを描画する空間のView-Projection行列を渡す。
     *
     * @param[in]  matrix  View-Projection行列
     */
    void Draw(Csm::CubismMatrix44& matrix);

    /**
     * @brief   引数で指定したモーションの再生を開始する。
     *
     * @param[in]   group                       モーショングループ名
     * @param[in]   no                          グループ内の番号
     * @param[in]   priority                    優先度
     * @param[in]   onFinishedMotionHandler     モーション再生終了時に呼び出されるコールバック関数。NULLの場合、呼び出されない。
     * @param[in]   onBeganMotionHandler        モーション再生開始時に呼び出されるコールバック関数。NULLの場合、呼び出されない。
     * @return                                  開始したモーションの識別番号を返す。個別のモーションが終了したか否かを判定するIsFinished()の引数で使用する。開始できない時は「-1」
     */
    Csm::CubismMotionQueueEntryHandle StartMotion(const Csm::csmChar* group, Csm::csmInt32 no, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL, Csm::ACubismMotion::BeganMotionCallback onBeganMotionHandler = NULL);

    /**
     * @brief   ランダムに選ばれたモーションの再生を開始する。
     *
     * @param[in]   group                       モーショングループ名
     * @param[in]   priority                    優先度
     * @param[in]   onFinishedMotionHandler     モーション再生終了時に呼び出されるコールバック関数。NULLの場合、呼び出されない。
     * @param[in]   onBeganMotionHandler        モーション再生開始時に呼び出されるコールバック関数。NULLの場合、呼び出されない。
     * @return                                  開始したモーションの識別番号を返す。個別のモーションが終了したか否かを判定するIsFinished()の引数で使用する。開始できない時は「-1」
     */
    Csm::CubismMotionQueueEntryHandle StartRandomMotion(const Csm::csmChar* group, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL, Csm::ACubismMotion::BeganMotionCallback onBeganMotionHandler = NULL);

    /**
     * @brief   引数で指定した表情モーションをセットする
     *
     * @param   expressionID    表情モーションのID
     */
    void SetExpression(const Csm::csmChar* expressionID);

    /**
     * @brief   ランダムに選ばれた表情モーションをセットする
     *
     */
    void SetRandomExpression();

    /**
    * @brief   イベントの発火を受け取る
    *
    */
    virtual void MotionEventFired(const Live2D::Cubism::Framework::csmString& eventValue);

    /**
     * @brief    当たり判定テスト。<br>
     *            指定IDの頂点リストから矩形を計算し、座標が矩形範囲内か判定する。
     *
     * @param[in]   hitAreaName     当たり判定をテストする対象のID
     * @param[in]   x               判定を行うX座標
     * @param[in]   y               判定を行うY座標
     */
    virtual Csm::csmBool HitTest(const Csm::csmChar* hitAreaName, Csm::csmFloat32 x, Csm::csmFloat32 y);

    /**
     * @brief   モデルに削除マークを付ける
     *          現在、役割としては表示を抑制しているだけ
     */
    void DeleteMark() { _deleteModel = true; }


    /**
     * @brief   別ターゲットに描画する際に使用するバッファの取得
     */
    Csm::Rendering::CubismOffscreenSurface_D3D11& GetRenderBuffer();

    /**
     * @brief   .moc3ファイルの整合性をチェックする
     *
     * @param[in]   mocName MOC3ファイル名
     * @return      MOC3に整合性があれば'true'、そうでなければ'false'。
     */
    Csm::csmBool HasMocConsistencyFromFile(const Csm::csmChar* mocFileName);

protected:
    /**
     *  @brief  モデルを描画する処理。モデルを描画する空間のView-Projection行列を渡す。
     *
     */
    void DoDraw();
private:
    /**
     * @brief model3.jsonからモデルを生成する。<br>
     *         model3.jsonの記述に従ってモデル生成、モーション、物理演算などのコンポーネント生成を行う。
     *
     * @param[in]   setting     ICubismModelSettingのインスタンス
     *
     */
    void SetupModel(Csm::ICubismModelSetting* setting);

    /**
     * @brief テクスチャをロードする
     *
     */
    void SetupTextures();

    /**
     * @brief   モーションデータをグループ名から一括でロードする。<br>
     *           モーションデータの名前は内部でModelSettingから取得する。
     *
     * @param[in]   group  モーションデータのグループ名
     */
    void PreloadMotionGroup(const Csm::csmChar* group);

    /**
     * @brief   モーションデータをグループ名から一括で解放する。<br>
     *           モーションデータの名前は内部でModelSettingから取得する。
     *
     * @param[in]   group  モーションデータのグループ名
     */
    void ReleaseMotionGroup(const Csm::csmChar* group) const;

    /**
    * @brief すべてのモーションデータの解放
    *
    * すべてのモーションデータを解放する。
    */
    void ReleaseMotions();

    /**
    * @brief すべての表情データの解放
    *
    * すべての表情データを解放する。
    */
    void ReleaseExpressions();

    Csm::ICubismModelSetting* _modelSetting; ///< モデルセッティング情報
    Csm::csmString _modelHomeDir; ///< モデルセッティングが置かれたディレクトリ
    Csm::csmFloat32 _userTimeSeconds; ///< デルタ時間の積算値[秒]
    Csm::csmVector<Csm::CubismIdHandle> _lipSyncIds; ///< モデルに設定されたリップシンク機能用パラメータID
    std::vector<LookParam> _lookTargetParams;
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _motions; ///< 読み込まれているモーションのリスト
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _expressions; ///< 読み込まれている表情のリスト
    Csm::csmVector<Csm::csmRectF> _hitArea;
    Csm::csmVector<Csm::csmRectF> _userArea;
    //delete
    //const Csm::CubismId* _idParamAngleX; ///< パラメータID: ParamAngleX
    //const Csm::CubismId* _idParamAngleY; ///< パラメータID: ParamAngleX
    //const Csm::CubismId* _idParamAngleZ; ///< パラメータID: ParamAngleX
    //const Csm::CubismId* _idParamBodyAngleX; ///< パラメータID: ParamBodyAngleX
    //const Csm::CubismId* _idParamEyeBallX; ///< パラメータID: ParamEyeBallX
    //const Csm::CubismId* _idParamEyeBallY; ///< パラメータID: ParamEyeBallXY
    Csm::csmVector<Csm::CubismIdHandle> _eyeBlinkIdsDef;
    Csm::csmVector<Csm::csmUint64> _bindTextureId; ///< テクスチャID
    LAppWavFileHandler _wavFileHandler; ///< wavファイルハンドラ
    bool _deleteModel;  ///< 実体消滅予定フラグ Drawを呼ばない
    Csm::Rendering::CubismOffscreenSurface_D3D11 _renderBuffer;   ///< フレームバッファ以外の描画先
};
