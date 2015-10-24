/*
* \file PlayerCompoment.h
* \author chu
* \date 2015/10/24
* \brief 玩家组件
*/
#pragma once
#include "../Common.h"
#include "../Base/ECSBase.h"

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
		void UpdateEntity(ECSEntity* p)override;
	};
}
