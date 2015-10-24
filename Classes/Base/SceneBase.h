/*
 * \file SceneBase.h
 * \author chu
 * \date 2015/10/11
 */
#pragma once
#include "../Common.h"

namespace sokoban
{
    /// \brief 场景基类
    /// \note 增加一些实用函数
    class SceneBase :
        public AutoInvokeInit<cocos2d::Scene>
    {
    private:
        using TouchPair = std::pair<cocos2d::RefPtr<cocos2d::Touch>, cocos2d::RefPtr<cocos2d::Layer>>;
        
        std::vector<cocos2d::RefPtr<cocos2d::Layer>> m_vecLayerList;  // 层列表
        
        // 触摸事件分发器
        cocos2d::RefPtr<cocos2d::EventListenerTouchAllAtOnce> m_pTouchEventListener;
        std::unordered_map<cocos2d::Touch*, TouchPair> m_umTouchRouter;
    public:
        SceneBase();
    public:
        /// \brief 增加一个层用于分发触摸事件
        /// \note 新增加的层拥有最高的优先级来获得触摸事件的分发
        void AddLayer(cocos2d::RefPtr<cocos2d::Layer> pLayer, int32_t iLocalZOrder=0);
        
        /// \brief 向Scene发送消息
        void ReceiveMessage(int32_t iEventId, cocos2d::Event* unused_event=nullptr)
        {
            onMessage(iEventId, unused_event);
        }
    protected:  // events
        void onEnter()override;
        void onExit()override;
        
        // 用于接收传递的事件
        virtual void onMessage(int32_t iEventId, cocos2d::Event* unused_event);
    };
}
