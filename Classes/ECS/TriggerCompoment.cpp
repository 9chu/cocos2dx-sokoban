#include "TriggerCompoment.h"
#include "BaseCompoment.h"

NAMESPACE_SOKOBAN;

TriggerSystem::TriggerSystem(uint32_t iTileWidth, uint32_t iTileHeight)
	: m_iTileWidth(iTileWidth), m_iTileHeight(iTileHeight)
{
}

void TriggerSystem::AddEntity(ECSEntity* p)
{
	BaseCompoment* pBaseCompoment = p->GetCompoment<BaseCompoment>();
	TriggerCompoment* pTriggerCompoment = p->GetCompoment<TriggerCompoment>();
	
	if (pBaseCompoment && pTriggerCompoment)
        m_arrTriggerList.emplace_back(p);
}

void TriggerSystem::RemoveEntity(ECSEntity* p)
{
	BaseCompoment* pBaseCompoment = p->GetCompoment<BaseCompoment>();
	TriggerCompoment* pTriggerCompoment = p->GetCompoment<TriggerCompoment>();

	if (pBaseCompoment && pTriggerCompoment)
	{
		for (auto i = m_arrTriggerList.begin(); i != m_arrTriggerList.end(); ++i)
		{
			if (*i == p)
			{
				m_arrTriggerList.erase(i);
				return;
			}	
		}
	}
}

void TriggerSystem::UpdateEntity(float delta, ECSEntity* p)
{
	BaseCompoment* pBaseCompoment = p->GetCompoment<BaseCompoment>();
	TriggerEmitterCompoment* pTriggerEmitterCompoment = p->GetCompoment<TriggerEmitterCompoment>();

	if (pBaseCompoment && pTriggerEmitterCompoment)
	{
		if (!pTriggerEmitterCompoment->IsDisabled())
		{
			int32_t iTriggerEmitterX = (int)(pBaseCompoment->GetPosition().x + m_iTileWidth / 2.f) / m_iTileWidth;
			int32_t iTriggerEmitterY = (int)(pBaseCompoment->GetPosition().y + m_iTileHeight / 2.f) / m_iTileHeight;

			ECSEntity* pHitTrigger = nullptr;

			for (auto& pTrigger : m_arrTriggerList)
			{
				// 依次检查是否可触发
				BaseCompoment* pTriggerBaseCompoment = pTrigger->GetCompoment<BaseCompoment>();
				
                assert(pTriggerBaseCompoment != nullptr);
                
				int32_t iTriggerX = (int)(pTriggerBaseCompoment->GetPosition().x + m_iTileWidth / 2.f) / m_iTileWidth;
				int32_t iTriggerY = (int)(pTriggerBaseCompoment->GetPosition().y + m_iTileHeight / 2.f) / m_iTileHeight;

				if (iTriggerEmitterX == iTriggerX && iTriggerEmitterY == iTriggerY)
				{
					pHitTrigger = pTrigger.get();
					break;
				}
			}

			if (pTriggerEmitterCompoment->IsTriggered() && !pHitTrigger)
			{
				// 从触发态归位
				pTriggerEmitterCompoment->m_bTriggered = false;

				// 执行回调
                auto pCallback = pTriggerEmitterCompoment->GetLeaveCallback();
				if (pCallback)
					pCallback(p);
			}
			else if (!pTriggerEmitterCompoment->IsTriggered() && pHitTrigger)
			{
				// 从普通态触发
				pTriggerEmitterCompoment->m_bTriggered = true;

				// 执行回调
				auto pCallback = pTriggerEmitterCompoment->GetEnterCallback();
				if (pCallback)
					pCallback(pHitTrigger, p);
			}
		}
	}
}
