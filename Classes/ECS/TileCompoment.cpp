#include "TileCompoment.h"
#include "BaseCompoment.h"

NAMESPACE_SOKOBAN;

TileSystem::TileSystem(uint32_t iTileWidth, uint32_t iTileHeight)
	: m_iTileWidth(iTileWidth), m_iTileHeight(iTileHeight)
{
}

void TileSystem::setObjectAt(int32_t iXIndex, int32_t iYIndex, ECSEntity* p)noexcept
{
	if (iXIndex < 0 || iYIndex < 0 || iXIndex >= (int)m_iWidth || iYIndex >= (int)m_iHeight)
	{
		CHU_LOGWARN("out of range, args x=%d y=%d", iXIndex, iYIndex);
		return;
	}

	uint32_t iIndex = iXIndex + iYIndex * m_iWidth;
	m_arrObjectMap[iIndex] = p;
}

void TileSystem::setObjectAt(cocos2d::Vec2 vecPosition, ECSEntity* p)noexcept
{
	setObjectAt((int)vecPosition.x / m_iTileWidth, (int)vecPosition.y / m_iTileHeight, p);
}

void TileSystem::clearObjectMap()noexcept
{
	if (m_arrObjectMap.size() > 0)
		::memset(&m_arrObjectMap[0], 0, sizeof(m_arrObjectMap[0]) * m_arrObjectList.size());
}

ECSEntity* TileSystem::GetObjectAt(int32_t iXIndex, int32_t iYIndex)const noexcept
{
	if (iXIndex < 0 || iYIndex < 0 || iXIndex >= (int)m_iWidth || iYIndex >= (int)m_iHeight)
		return nullptr;

	uint32_t iIndex = iXIndex + iYIndex * m_iWidth;
	return m_arrObjectMap[iIndex];
}

ECSEntity* TileSystem::GetObjectAt(cocos2d::Vec2 vecPosition)const noexcept
{
	return GetObjectAt((int)vecPosition.x / m_iTileWidth, (int)vecPosition.y / m_iTileHeight);
}

TileObjectType TileSystem::GetObjectTypeAt(int32_t iXIndex, int32_t iYIndex)const noexcept
{
	auto pObj = GetObjectAt(iXIndex, iYIndex);
	if (!pObj)
		return TileObjectType::None;
	return pObj->GetCompoment<TileCompoment>()->GetObjectType();
}

TileObjectType TileSystem::GetObjectTypeAt(cocos2d::Vec2 vecPosition)const noexcept
{
	return GetObjectTypeAt((int)vecPosition.x / m_iTileWidth, (int)vecPosition.y / m_iTileHeight);
}

void TileSystem::ResetSize(uint32_t iWidth, uint32_t iHeight)
{
	m_arrObjectMap.resize(iWidth * iHeight);
	m_iWidth = iWidth;
	m_iHeight = iHeight;
}

void TileSystem::AddEntity(ECSEntity* p)
{
	BaseCompoment* pBaseCompoment = p->GetCompoment<BaseCompoment>();
	TileCompoment* pTileCompoment = p->GetCompoment<TileCompoment>();

	if (pBaseCompoment && pTileCompoment)
		m_arrObjectList.emplace_back(p);
}

void TileSystem::RemoveEntity(ECSEntity* p)
{
	BaseCompoment* pBaseCompoment = p->GetCompoment<BaseCompoment>();
	TileCompoment* pTileCompoment = p->GetCompoment<TileCompoment>();

	if (pBaseCompoment && pTileCompoment)
	{
		for (auto i = m_arrObjectList.begin(); i != m_arrObjectList.end(); ++i)
		{
			if (*i == p)
			{
				m_arrObjectList.erase(i);
				return;
			}
		}
	}
}

void TileSystem::BeforeUpdate(float delta)
{
	clearObjectMap();

	for (auto& p : m_arrObjectList)
	{
		BaseCompoment* pBaseCompoment = p->GetCompoment<BaseCompoment>();
		TileCompoment* pStaticCompoment = p->GetCompoment<TileCompoment>();
		if (pBaseCompoment && pStaticCompoment)
			setObjectAt(pBaseCompoment->GetPosition(), p);
	}
}
