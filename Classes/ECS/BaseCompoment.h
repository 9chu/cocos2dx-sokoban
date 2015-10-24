/*
 * \file BaseCompoment.h
 * \author chu
 * \date 2015/10/18
 * \brief 实现基本的组件，绑定Cocos2dx的Node
 */
#pragma once
#include "../Common.h"
#include "../Base/ECSBase.h"

namespace sokoban
{
    /// \brief 基本组件
    class BaseCompoment :
        public ECSCompoment
    {
        CHU_COMPOMENT_DEF(ECSCompomentType::BaseCompoment);
    private:
        cocos2d::RefPtr<cocos2d::Node> m_pNativeNode;
    public:
        BaseCompoment(cocos2d::RefPtr<cocos2d::Node> pNativeNode);
    public:
        cocos2d::Node* GetNativeNode() { return m_pNativeNode; }
        
        /// \brief 获取坐标
        cocos2d::Vec2 GetPosition()const { return m_pNativeNode->getPosition(); }
        void SetPositionX(float x) { m_pNativeNode->setPositionX(x); }
        void SetPositionY(float y) { m_pNativeNode->setPositionY(y); }
        void SetPosition(cocos2d::Vec2 xy) { m_pNativeNode->setPosition(xy); }
    };
    
    /// \brief 基本组件系统
    /// \note 优先级=0
    class BaseSystem :
        public ECSSystem
    {
    private:
        cocos2d::RefPtr<cocos2d::Camera> m_pCamera;
        cocos2d::Node* m_pContainerWeakRef = nullptr;
    public:
        BaseSystem(cocos2d::Node* pContainer);
        ~BaseSystem();
    public:
        /// \brief 获取对象绑定的相机
        cocos2d::Camera* GetCamera() { return m_pCamera; }
    public:
        void AddEntity(ECSEntity* p)override;
        void RemoveEntity(ECSEntity* p)override;
    };
}
