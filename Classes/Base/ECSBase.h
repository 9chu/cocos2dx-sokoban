/*
 * \file ECSBase.h
 * \author chu
 * \date 2015/10/6
 */
#pragma once
#include "../Common.h"

#define CHU_COMPOMENT_DEF(comptype) \
    public: \
        static const ECSCompomentType CompomentType = comptype; \
        ECSCompomentType GetType()const override { return CompomentType; } \
    private:

namespace sokoban
{
    /// \brief 组件类型
    enum class ECSCompomentType
    {
        BaseCompoment,  // 基本组件，绑定Cocos2dx的Node，提供Position相关属性
		TileCompoment,  // 瓷砖组件
		TriggerCompoment,  // 触发器组件
		TriggerEmitterCompoment,  // 触发者组件
        PlayerCompoment,  // 玩家组件，用于控制玩家行为
        
        MAX_COMPOMENT_,  // 组件最大值
    };
    
    /// \brief ECS组件
    class ECSCompoment :
        public cocos2d::Ref
    {
    public:
        /// \brief 获取组件类型
        virtual ECSCompomentType GetType()const = 0;
    };
    
    /// \brief ECS实体
    class ECSEntity :
        public cocos2d::Ref
    {
    private:
        bool m_bDead = false;  // 死亡标识
        cocos2d::RefPtr<ECSCompoment> m_arrCompoments[(uint32_t)ECSCompomentType::MAX_COMPOMENT_];
    public:
        /// \brief 判断对象是否死亡
        bool IsDead()const { return m_bDead; }
        
        /// \brief 设置对象死亡标识
        void SetDead(bool b) { m_bDead = b; }
        
        /// \brief 获取组件
        /// \param[in] iCompoment 组件类别
        /// \return 若组件不存在则返回nullptr
        ECSCompoment* GetCompoment(ECSCompomentType iCompoment)
        {
            CCASSERT(iCompoment < ECSCompomentType::MAX_COMPOMENT_, "Compoment index out of range.");
            return m_arrCompoments[static_cast<uint32_t>(iCompoment)];
        }
        
        /// \brief 设置组件
        /// \note 对象组件一旦被设置则无法被移除，并且应当在加入场景前进行设置
        /// \param[in] pCompoment 组件指针
        void SetCompoment(ECSCompoment* pCompoment)
        {
            CCASSERT(pCompoment != nullptr, "null pCompoment");
            
            ECSCompomentType iType = pCompoment->GetType();
            CCASSERT(iType < ECSCompomentType::MAX_COMPOMENT_, "Compoment index out of range.");
            
            m_arrCompoments[static_cast<uint32_t>(iType)] = pCompoment;
        }
        
        /// \brief 获取组件模板
        template <typename T>
        T* GetCompoment()
        {
            return static_cast<T*>(GetCompoment(T::CompomentType));
        }
    };
    
    /// \brief ECS系统
    class ECSSystem :
        public cocos2d::Ref
    {
    public:
        /// \brief 获取系统的优先级
        /// \note 越小的值优先级越高，越先获取更新权限
        /// \return 默认返回0
        virtual int32_t GetPriority()const;
        
        /// \brief 处理对象加入
        /// \param[in] p 对象指针
        virtual void AddEntity(ECSEntity* p);
        
        /// \brief 处理对象移除
        /// \param[in] p 对象指针
        virtual void RemoveEntity(ECSEntity* p);
        
        /// \brief 更新对象
        /// \param[in] p 对象指针
        virtual void UpdateEntity(float delta, ECSEntity* p);
        
        /// \brief 更新事件
        virtual void Update(float delta);

		/// \brief 在更新前调用
		virtual void BeforeUpdate(float delta);
    };
    
    /// \brief ECS舞台
    /// \note 所有的ECS游戏对象都存储于舞台中
    class ECSStage :
        public cocos2d::Layer
    {
    private:
        std::vector<cocos2d::RefPtr<ECSSystem>> m_vecSystemList;
        std::vector<cocos2d::RefPtr<ECSEntity>> m_vecEntityList;
    public:
        ECSStage();
    public:
        /// \brief 添加一个系统
        void AddSystem(cocos2d::RefPtr<ECSSystem> p);
        
        /// \brief 添加一个实体
        void AddEntity(cocos2d::RefPtr<ECSEntity> p);
        
        /// \brief 清空
        void ClearEntity();
    protected:
        void update(float delta)override;
        void onEnter()override;
    };
}
