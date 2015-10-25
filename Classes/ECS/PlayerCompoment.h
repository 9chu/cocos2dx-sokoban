/*
* \file PlayerCompoment.h
* \author chu
* \date 2015/10/24
* \brief 玩家组件
*/
#pragma once
#include "../Common.h"
#include "../Base/ECSBase.h"

#include "BaseCompoment.h"
#include "TileCompoment.h"

namespace sokoban
{
	/// \brief 玩家输入状态
	enum class PlayerInput
	{
		None = 0,
		Left,
		Right,
		Up,
		Down
	};
    
    /// \brief 玩家动画状态
    /// \note 关联updateAnimation::s_PlayerAnimationList
    enum class PlayerAnimation
    {
        LeftAnimation = 0,
        RightAnimation,
        UpAnimation,
        DownAnimation
    };

	/// \brief 玩家输入接口
	struct IPlayerInputProvider
	{
		/// \brief 获取玩家最近一帧的输入
		/// \note 输入状态将在下一帧被清除
		virtual PlayerInput GetPlayerLastInput()const noexcept = 0;
	};

	/// \brief 玩家组件
	class PlayerCompoment :
		public ECSCompoment
	{
		CHU_COMPOMENT_DEF(ECSCompomentType::PlayerCompoment);
    private:
        float m_iAnimationTimer = 0;
        PlayerAnimation m_iAnimationState = PlayerAnimation::DownAnimation;
        uint32_t m_iAnimationFrame = 0;
    public:
        float GetAnimationTimer()const noexcept { return m_iAnimationTimer; }
        float SetAnimationTimer(float iTimer)noexcept { m_iAnimationTimer = iTimer; return iTimer; }
        PlayerAnimation GetAnimationState()const noexcept { return m_iAnimationState; }
        void SetAnimationState(PlayerAnimation iAnimation)noexcept { m_iAnimationState = iAnimation; }
        uint32_t GetAnimationFrame()const noexcept { return m_iAnimationFrame; }
        uint32_t SetAnimationFrame(uint32_t iFrame)noexcept { m_iAnimationFrame = iFrame; return iFrame; }
	public:
		PlayerCompoment() {}
	};

	/// \brief 玩家逻辑系统
	class PlayerSystem :
		public ECSSystem
	{
	private:
		cocos2d::RefPtr<TileSystem> m_pTileSystem;
		IPlayerInputProvider* m_pProvider = nullptr;
	public:
		PlayerSystem(cocos2d::RefPtr<TileSystem> pTileSystem, IPlayerInputProvider* pProvider)
			: m_pTileSystem(pTileSystem), m_pProvider(pProvider) {}
    public:
        void changeAnimation(PlayerInput iInput, PlayerCompoment* pPlayerCompoment);
        void updateAnimation(float delta, BaseCompoment* pBaseCompoment, PlayerCompoment* pPlayerCompoment);
	public:
		void UpdateEntity(float delta, ECSEntity* p)override;
	};
}
