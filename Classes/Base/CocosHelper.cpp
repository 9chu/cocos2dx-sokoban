#include "CocosHelper.h"

NAMESPACE_SOKOBAN;

void CocosHelper::LoadNodeAndTimelineFromCSB(const char* pPath, cocos2d::RefPtr<cocos2d::Node>& pNode, cocos2d::RefPtr<cocostudio::timeline::ActionTimeline>& pTimeline)
{
    pNode = CSLoader::createNode(pPath);
    if (!pNode)
    {
        CHU_LOGERROR("create node from '%s' failed.", pPath);
        throw logic_error("CreateNodeFailed");
    }
    
    pTimeline = CSLoader::createTimeline(pPath);
    if (!pTimeline)
    {
        CHU_LOGERROR("create timeline from '%s' failed.", pPath);
        throw logic_error("CreateTimelineFailed");
    }
}

void CocosHelper::AdjustLayerHorizontalCentered(cocos2d::Node* pNode)
{
    cocos2d::Size tSize = cocos2d::Director::getInstance()->getVisibleSize();
    float tScaleRate = tSize.height / SOKOBAN_DESIGN_HEIGHT;
    float tOffsetX = (tSize.width - tScaleRate * SOKOBAN_DESIGN_WIDTH) / 2.f / tScaleRate;
    pNode->setPosition(pNode->getPositionX() + tOffsetX, pNode->getPositionY() + 0.f);
}
