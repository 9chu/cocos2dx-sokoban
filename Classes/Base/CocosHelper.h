/*
 * \file CocosHelper.h
 * \author chu
 * \date 2015/10/17
 */
#pragma once
#include "../Common.h"

namespace sokoban
{
    /// \brief Cocos辅助类
    class CocosHelper :
        public Noncopyable
    {
    public:
        /// \brief 从文件装载节点和时间轴
        /// \param[in] pPath 路径
        /// \param[out] pNode 节点
        /// \param[out] pTimeline 时间轴
        static void LoadNodeAndTimelineFromCSB(const char* pPath, cocos2d::RefPtr<cocos2d::Node>& pNode, cocos2d::RefPtr<cocostudio::timeline::ActionTimeline>& pTimeline);
        
        /// \brief 调整节点相对屏幕水平居中
        static void AdjustLayerHorizontalCentered(cocos2d::Node* pNode);
        
        /// \brief 从帧中获取事件ID
        template <typename T>
        static T GetEventIDFromFrame(cocostudio::timeline::Frame* pFrame)
        {
            cocostudio::timeline::EventFrame* pEventFrame = dynamic_cast<cocostudio::timeline::EventFrame*>(pFrame);
            if(!pEventFrame)
                return static_cast<T>(0);
            return static_cast<T>(::atoi(pEventFrame->getEvent().c_str()));
        }
    };
}
