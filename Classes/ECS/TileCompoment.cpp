#include "TileCompoment.h"
#include "BaseCompoment.h"

NAMESPACE_SOKOBAN;

TileSystem::TileSystem(uint32_t iTileWidth, uint32_t iTileHeight)
	: m_iTileWidth(iTileWidth), m_iTileHeight(iTileHeight)
{
}

TileObjectType TileSystem::GetObjectTypeAt(int32_t iXIndex, int32_t iYIndex)const noexcept
{
	if (iXIndex < 0 || iYIndex < 0 || iXIndex >= (int)m_iWidth || iYIndex >= (int)m_iHeight)
		return TileObjectType::None;

	uint32_t iIndex = iXIndex + iYIndex * m_iWidth;
	return m_arrObjectType[iIndex];
}

TileObjectType TileSystem::GetObjectTypeAt(cocos2d::Vec2 vecPosition)const noexcept
{
	return GetObjectTypeAt((int)vecPosition.x / m_iTileWidth, (int)vecPosition.y / m_iTileHeight);
}

void TileSystem::SetObjectTypeAt(int32_t iXIndex, int32_t iYIndex, TileObjectType iType)noexcept
{
	if (iXIndex < 0 || iYIndex < 0 || iXIndex >= (int)m_iWidth || iYIndex >= (int)m_iHeight)
	{
		CHU_LOGWARN("out of range, args x=%d y=%d", iXIndex, iYIndex);
		return;
	}	

	uint32_t iIndex = iXIndex + iYIndex * m_iWidth;
	m_arrObjectType[iIndex] = iType;
}

void TileSystem::SetObjectTypeAt(cocos2d::Vec2 vecPosition, TileObjectType iType)noexcept
{
	SetObjectTypeAt((int)vecPosition.x / m_iTileWidth, (int)vecPosition.y / m_iTileHeight, iType);
}

void TileSystem::ClearObjectType()noexcept
{
	for (size_t i = 0; i < m_iWidth; ++i)
	{
		for (size_t j = 0; j < m_iHeight; ++j)
			m_arrObjectType[i + j * m_iWidth] = TileObjectType::None;
	}
}

void TileSystem::ResetSize(uint32_t iWidth, uint32_t iHeight)
{
	m_arrObjectType.resize(iWidth * iHeight);
	m_iWidth = iWidth;
	m_iHeight = iHeight;
}

void TileSystem::AddEntity(ECSEntity* p)
{
	BaseCompoment* pBaseCompoment = p->GetCompoment<BaseCompoment>();
	TileCompoment* pStaticCompoment = p->GetCompoment<TileCompoment>();
	if (pBaseCompoment && pStaticCompoment)
		SetObjectTypeAt(pBaseCompoment->GetPosition(), pStaticCompoment->GetObjectType());
}

void TileSystem::RemoveEntity(ECSEntity* p)
{
	BaseCompoment* pBaseCompoment = p->GetCompoment<BaseCompoment>();
	TileCompoment* pStaticCompoment = p->GetCompoment<TileCompoment>();
	if (pBaseCompoment && pStaticCompoment)
		SetObjectTypeAt(pBaseCompoment->GetPosition(), TileObjectType::None);
}
