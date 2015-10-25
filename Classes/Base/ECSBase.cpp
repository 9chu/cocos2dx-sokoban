#include "ECSBase.h"

NAMESPACE_SOKOBAN;

int32_t ECSSystem::GetPriority()const
{
    return 0;
}

void ECSSystem::AddEntity(ECSEntity* p)
{   
}

void ECSSystem::RemoveEntity(ECSEntity* p)
{
}

void ECSSystem::UpdateEntity(float delta, ECSEntity* p)
{
}

void ECSSystem::Update(float delta)
{
}

void ECSSystem::BeforeUpdate(float delta)
{
}

////////////////////////////////////////////////////////////////////////////////

ECSStage::ECSStage()
{
    scheduleUpdate();
}

void ECSStage::AddSystem(cocos2d::RefPtr<ECSSystem> p)
{
    m_vecSystemList.emplace_back(p);
}

void ECSStage::AddEntity(cocos2d::RefPtr<ECSEntity> p)
{
    m_vecEntityList.emplace_back(p);
    
    // 通知所有组件新加入对象
    for (auto& j : m_vecSystemList)
        j->AddEntity(p);
}

void ECSStage::update(float delta)
{
    Layer::update(delta);

	// 预更新所有组件
	auto j = m_vecSystemList.begin();
	while (j != m_vecSystemList.end())
	{
		(*j)->BeforeUpdate(delta);
		++j;
	}

    // 更新所有对象
    auto i = m_vecEntityList.begin();
    while (i != m_vecEntityList.end())
    {
        for (auto& j : m_vecSystemList)
            j->UpdateEntity(delta, *i);
        ++i;
    }
    
    // 再次扫描所有对象删除已被标记为Dead的对象
    i = m_vecEntityList.begin();
    while (i != m_vecEntityList.end())
    {
        if ((*i)->IsDead())
        {
            // 通知所有组件移除对象
            for (auto& j : m_vecSystemList)
                j->RemoveEntity(*i);
            i = m_vecEntityList.erase(i);
        }
        else
            ++i;
    }
    
    // 更新所有组件
    j = m_vecSystemList.begin();
    while (j != m_vecSystemList.end())
    {
        (*j)->Update(delta);
        ++j;
    }
}

void ECSStage::onEnter()
{
    Layer::onEnter();
    
    // 对System进行排序
    std::sort(m_vecSystemList.begin(), m_vecSystemList.end(),
        [](const RefPtr<ECSSystem>& a, const RefPtr<ECSSystem>& b)
        {
            return a->GetPriority() < b->GetPriority();
        });
}
