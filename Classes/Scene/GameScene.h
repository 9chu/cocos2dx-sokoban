/*
 * \file GameScene.h
 * \author chu
 * \date 2015/09/29
 */
#pragma once
#include "../Common.h"
#include "../Base/SceneBase.h"

#include "../ECS/BaseCompoment.h"
#include "../ECS/TileCompoment.h"
#include "../ECS/TriggerCompoment.h"
#include "../ECS/PlayerCompoment.h"

#include "../Base/TMXFile.h"

namespace sokoban
{    
	/// \brief 输入接收层
	class PlayerInputLayer :
		public cocos2d::Layer,
		public IPlayerInputProvider
	{
	private:
		PlayerInput m_iInputState = PlayerInput::None;
		cocos2d::RefPtr<cocos2d::Touch> m_pTouch;
		cocos2d::Vec2 m_vecBasePosition;
		cocos2d::Vec2 m_vecNewPosition;
	public:
		PlayerInputLayer();
	protected:
		void update(float delta)override;
		bool onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* unused_event)override;
		void onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* unused_event)override;
		void onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* unused_event)override;
		void onTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* unused_event)override;
	public:
		PlayerInput GetPlayerLastInput()const noexcept override;
	};

    /// \brief 游戏场景层
    class GameStageLayer :
        public ECSStage
    {
    private:
        // ECS系统
		cocos2d::RefPtr<BaseSystem> m_pBaseSystem;
		cocos2d::RefPtr<TileSystem> m_pTileSystem;
		cocos2d::RefPtr<TriggerSystem> m_pTriggerSystem;
		cocos2d::RefPtr<PlayerSystem> m_pPlayerSystem;
        
        // 关卡信息
        uint32_t m_iLevelNo = 0;  // 关卡标记
        uint32_t m_iBoxCount = 0;  // 箱子数量
        
        // 关卡状态
        bool m_bLevelPass = false;  // 通关标志
        float m_fLevelPassDelay = 0.f;  // 通关延迟计数器
        uint32_t m_iActivedBoxCount = 0;  // 当前已经激活的箱子数量
	public:
		GameStageLayer(IPlayerInputProvider* pInput);
    protected:
		void update(float delta)override;
		void onEnterTrigger(ECSEntity* pTrigger, ECSEntity* pEmitter);
		void onLeaveTrigger(ECSEntity* pEmitter);
	public:
		bool LoadLevel(uint32_t iLevelNo);
    };
    
    /// \brief 游戏场景
    class GameScene :
        public SceneBase
    {
    public:
        GameScene();
    };
}
