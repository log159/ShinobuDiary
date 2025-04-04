﻿/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <Rendering/D3D11/CubismNativeInclude_D3D11.hpp>
#include <CubismFramework.hpp>
#include <Math/CubismMatrix44.hpp>
#include <Type/csmVector.hpp>
#include <Type/csmMap.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include "../shinobugui_src/global.h"
#include "../shinobugui_src/suconfig.h"

struct ModelJsonConfig {
    Csm::csmString modelPath;
    Csm::csmString modelJsonName;
};
class LAppModel;

/**
* @brief サンプルアプリケーションにおいてCubismModelを管理するクラス<br>
*         モデル生成と破棄、タップイベントの処理、モデル切り替えを行う。
*
*/
class LAppLive2DManager
{

public:

    ModelJsonConfig GetModelJsonConfigFromName(std::string modelname);

    void RefreshScene(int userid,std::string modelname);

    void RefreshSceneAndUserId(int pos);

    Csm::csmVector<Csm::csmString>& GetModelDir();

    /**
     * @brief 解放するモデル格納
     */
    struct ReleaseModel
    {
        ReleaseModel()
        {
            _model = NULL;
            _counter = 0;
        }

        LAppModel*  _model;
        int         _counter;
    };

    /**
    * @brief   クラスのインスタンス（シングルトン）を返す。<br>
    *           インスタンスが生成されていない場合は内部でインスタンを生成する。
    *
    * @return  クラスのインスタンス
    */
    static LAppLive2DManager* GetInstance();

    /**
    * @brief   クラスのインスタンス（シングルトン）を解放する。
    *
    */
    static void ReleaseInstance();

    /**
    * @brief   Resources フォルダにあるモデルフォルダ名をセットする
    *
    */
    void SetUpModel();

    /**
    * @brief   Resources フォルダにあるモデルフォルダ名を取得する
    *
    */
    Csm::csmVector<Csm::csmString> GetModelDir() const;

    /**
    * @brief   Resources フォルダにあるモデルフォルダのサイズを取得する
    *
    */
    Csm::csmInt32 GetModelDirSize() const;

    //直接返回_models
    std::unordered_map<int, LAppModel*>& GetModel();
    /**
    * @brief   現在のシーンで保持しているすべてのモデルを解放する
    *
    */
    void ReleaseAllModel();

    void ReleaseOneModel(int userid);

    /**
    * @brief   画面をドラッグしたときの処理
    *
    * @param[in]   x   画面のX座標
    * @param[in]   y   画面のY座標
    */
    void OnDrag(Csm::csmFloat32 x, Csm::csmFloat32 y) const;

    /**
    * @brief   画面をタップしたときの処理
    *
    * @param[in]   x   画面のX座標
    * @param[in]   y   画面のY座標
    */
    void OnTap(Csm::csmFloat32 x, Csm::csmFloat32 y);

    /**
    * @brief   画面を更新するときの処理
    *          モデルの更新処理および描画処理を行う
    */
    void OnUpdate() const;

    /**
     * @brief   フレーム末端の処理
     */
    void EndFrame();

    /**
     * @brief   モデル個数を得る
     * @return  所持モデル個数
     */
    Csm::csmUint32 GetModelNum() const;

    /**
     * @brief   ウィンドウサイズが変更された際に呼ばれる処理
     */
    void ResizedWindow();

    /**
     * @brief   viewMatrixをセットする
     */
    void SetViewMatrix(Live2D::Cubism::Framework::CubismMatrix44* m);

private:
    /**
    * @brief  コンストラクタ
    */
    LAppLive2DManager();

    /**
    * @brief  デストラクタ
    */
    virtual ~LAppLive2DManager();

    Csm::CubismMatrix44* _viewMatrix; ///< モデル描画に用いるView行列
    std::unordered_map<int,LAppModel*> _models; ///< モデルインスタンスのコンテナ
    //Csm::csmInt32 _sceneIndex; ///< 表示するシーンのインデックス値

    Csm::csmVector<Csm::csmString> _modelDir; ///< モデルディレクトリ名のコンテナ

    Csm::csmVector<ReleaseModel> _releaseModel; ///< 解放予定モデルインスタンスのコンテナ

    Csm::csmMap<LAppModel*, int> _releaseModels; ///< モデルインスタンスのコンテナ(解放予約、解放カウント)
};
