/*
* \file PlayerCompoment.h
* \author chu
* \date 2015/10/24
* \brief ������
*/
#pragma once
#include "../Common.h"
#include "../Base/ECSBase.h"

#include "TileCompoment.h"

namespace sokoban
{
	/// \brief �������״̬
	enum class PlayerInput
	{
		None = 0,
		Left,
		Right,
		Up,
		Down
	};

	/// \brief �������ӿ�
	struct IPlayerInputProvider
	{
		/// \brief ��ȡ������һ֡������
		/// \note ����״̬������һ֡�����
		virtual PlayerInput GetPlayerLastInput()const noexcept = 0;
	};

	/// \brief ������
	class PlayerCompoment :
		public ECSCompoment
	{
		CHU_COMPOMENT_DEF(ECSCompomentType::PlayerCompoment);
	public:
		PlayerCompoment() {}
	};

	/// \brief ����߼�ϵͳ
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
