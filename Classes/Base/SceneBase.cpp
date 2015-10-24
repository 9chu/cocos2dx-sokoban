#include "SceneBase.h"

NAMESPACE_SOKOBAN;

SceneBase::SceneBase()
{
    // 触摸事件侦听器
    m_pTouchEventListener = EventListenerTouchAllAtOnce::create();
    if (!m_pTouchEventListener)
    {
        CHU_LOGERROR("EventListenerAllAtOnce create failed.");
        throw logic_error("CreateEventListenerFailed");
    }
    m_pTouchEventListener->onTouchesBegan = [this](const vector<Touch*>& pTouches, Event* pEvent)
    {
        // 分发触摸事件
        for (auto pTouch : pTouches)
        {
            for (auto i = m_vecLayerList.rbegin(); i != m_vecLayerList.rend(); ++i)
            {
                // 该层接受该Touch事件，记录到分发表等待下次分发事件
                if ((*i)->onTouchBegan(pTouch, pEvent))
                {
                    m_umTouchRouter.emplace(pTouch, make_pair(RefPtr<Touch>(pTouch), (*i)));
                    break;
                }
            }
        }
        
        // see: https://github.com/cocos2d/cocos2d-x/issues/13620
        pEvent->stopPropagation();
    };
    m_pTouchEventListener->onTouchesMoved = [this](const vector<Touch*>& pTouches, Event* pEvent)
    {
        // 分发触摸事件
        for (auto pTouch : pTouches)
        {
            auto i = m_umTouchRouter.find(pTouch);
            if (i == m_umTouchRouter.end())
                continue;
            i->second.second->onTouchMoved(pTouch, pEvent);
        }
        
        // see: https://github.com/cocos2d/cocos2d-x/issues/13620
        pEvent->stopPropagation();
    };
    m_pTouchEventListener->onTouchesEnded = [this](const vector<Touch*>& pTouches, Event* pEvent)
    {
        // 分发触摸事件
        for (auto pTouch : pTouches)
        {
            auto i = m_umTouchRouter.find(pTouch);
            if (i == m_umTouchRouter.end())
                continue;
            i->second.second->onTouchEnded(pTouch, pEvent);
            
            // 移除事件
            m_umTouchRouter.erase(i);
        }
        
        // see: https://github.com/cocos2d/cocos2d-x/issues/13620
        pEvent->stopPropagation();
    };
    m_pTouchEventListener->onTouchesCancelled = [this](const vector<Touch*>& pTouches, Event* pEvent)
    {
        // 分发触摸事件
        for (auto pTouch : pTouches)
        {
            auto i = m_umTouchRouter.find(pTouch);
            if (i == m_umTouchRouter.end())
                continue;
            i->second.second->onTouchCancelled(pTouch, pEvent);
            
            // 移除事件
            m_umTouchRouter.erase(i);
        }
        
        // see: https://github.com/cocos2d/cocos2d-x/issues/13620
        pEvent->stopPropagation();
    };
}

void SceneBase::AddLayer(cocos2d::RefPtr<cocos2d::Layer> pLayer, int32_t iLocalZOrder)
{
    m_vecLayerList.emplace_back(pLayer);
    addChild(pLayer, iLocalZOrder);
}

void SceneBase::onEnter()
{
    Scene::onEnter();
    
    CHU_LOGINFO("onEnter invoked.");
    
    // 设置事件回调
    auto pEventDispatcher = Director::getInstance()->getEventDispatcher();
    pEventDispatcher->addEventListenerWithSceneGraphPriority(m_pTouchEventListener, this);
    
    // 排序层列表
    std::sort(m_vecLayerList.begin(), m_vecLayerList.end(), [](const RefPtr<Layer>& a, const RefPtr<Layer>& b)
    {
        return a->getLocalZOrder() < b->getLocalZOrder();
    });
}

void SceneBase::onExit()
{
    Scene::onExit();
    
    CHU_LOGINFO("onExit invoked.");
    
    // 取消事件回调
    auto pEventDispatcher = Director::getInstance()->getEventDispatcher();
    pEventDispatcher->removeEventListener(m_pTouchEventListener);
    
    m_umTouchRouter.clear();
}

void SceneBase::onMessage(int32_t iEventId, cocos2d::Event* unused_event)
{
}
