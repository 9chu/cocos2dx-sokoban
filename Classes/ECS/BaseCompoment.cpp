#include "BaseCompoment.h"

NAMESPACE_SOKOBAN;

BaseCompoment::BaseCompoment(cocos2d::RefPtr<cocos2d::Node> pNativeNode)
    : m_pNativeNode(pNativeNode)
{
    if (pNativeNode == nullptr)
    {
        CHU_LOGERROR("null pointer.");
        throw logic_error("NullPointerException");
    }
}

////////////////////////////////////////////////////////////////////////////////

BaseSystem::BaseSystem(cocos2d::Node* pContainer)
    : m_pContainerWeakRef(pContainer)
{
    CCASSERT(pContainer, "invalid null pointer.");
    
    m_pCamera = Camera::create();
    if (!m_pCamera)
    {
        CHU_LOGERROR("create camera failed.");
        throw logic_error("FailedToCreateCamera");
    }
    
    m_pCamera->setCameraFlag(CameraFlag::USER1);
    m_pCamera->setDepth(-1);
    m_pContainerWeakRef->addChild(m_pCamera);
}

BaseSystem::~BaseSystem()
{
    m_pContainerWeakRef->removeChild(m_pCamera);
}

void BaseSystem::AddEntity(ECSEntity* p)
{
    BaseCompoment* pCompoment = p->GetCompoment<BaseCompoment>();
    if (pCompoment)
    {
        cocos2d::Node* pNode = pCompoment->GetNativeNode();
        m_pContainerWeakRef->addChild(pNode);
        pNode->setCameraMask(2);  // USER1
    }
}

void BaseSystem::RemoveEntity(ECSEntity* p)
{
    BaseCompoment* pCompoment = p->GetCompoment<BaseCompoment>();
    if (pCompoment)
        m_pContainerWeakRef->removeChild(pCompoment->GetNativeNode());
}
