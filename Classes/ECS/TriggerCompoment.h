/*
* \file TriggerCompoment.h
* \author chu
* \date 2015/10/24
* \brief 触发器组件
*/
#pragma once
#include "../Common.h"
#include "../Base/ECSBase.h"

namespace sokoban
{
	/// \brief 触发器组件
	class TriggerCompoment :
		public ECSCompoment
	{
		CHU_COMPOMENT_DEF(ECSCompomentType::TriggerCompoment);
	public:
		// <触发器, 触发者> 回调控制器
		using Callback = std::function<void(ECSEntity*, ECSEntity*)>;
	private:
		Callback m_pCallback;
	public:
		Callback GetCallback()const noexcept { return m_pCallback; }
		void SetCallback(Callback cb)noexcept { m_pCallback = cb; }
	};

	/// \brief 触发者组件
	class TriggerEmitterCompoment :
		public ECSCompoment
	{
		friend class TriggerSystem;
		CHU_COMPOMENT_DEF(ECSCompomentType::TriggerEmitterCompoment);
	private:
		bool m_bDisabled = false;
		bool m_bTriggered = false;
	public:
		bool IsDisabled()const noexcept { return m_bDisabled; }
		void SetDisabled(bool b)noexcept
		{
			m_bDisabled = b; 
			if (b)
				m_bTriggered = false;
		}
		bool IsTriggered()const noexcept { return m_bTriggered; }
	};

	/// \brief 触发系统
	class TriggerSystem :
		public ECSSystem
	{
	private:
		uint32_t m_iTileWidth = 0;
		uint32_t m_iTileHeight = 0;
		std::vector<cocos2d::RefPtr<ECSEntity>> m_arrTriggerList;
	public:
		TriggerSystem(uint32_t iTileWidth, uint32_t iTileHeight);
	public:
		size_t GetTriggerCount()const noexcept { return m_arrTriggerList.size(); }
	public:
		void AddEntity(ECSEntity* p)override;
		void RemoveEntity(ECSEntity* p)override;
		void UpdateEntity(ECSEntity* p)override;
	};
}
