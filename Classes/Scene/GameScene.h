/*
 * \file GameScene.h
 * \author chu
 * \date 2015/09/29
 */
#pragma once
#include "../Common.h"
#include "../Base/SceneBase.h"

#include "../ECS/BaseCompoment.h"
#include "../Base/TMXFile.h"

namespace sokoban
{    
    /// \brief 游戏场景层
    class GameStageLayer :
        public ECSStage
    {
    private:
        // ECS系统
		cocos2d::RefPtr<BaseSystem> m_pBaseSystem;
	public:
		GameStageLayer();
    protected:
		void update(float delta)override;
	public:
		void LoadLevel(const char* pPath);
    };
    
    /// \brief 游戏场景
    class GameScene :
        public SceneBase
    {
    public:
        GameScene();
    };
}
