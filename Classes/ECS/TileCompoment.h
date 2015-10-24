/*
* \file TileCompoment.h
* \author chu
* \date 2015/10/24
* \brief 瓷砖地图组件
*/
#pragma once
#include "../Common.h"
#include "../Base/ECSBase.h"

namespace sokoban
{
	/// \brief 物体类型
	enum class TileObjectType
	{
		None,
		Wall,
		Box,
	};

	/// \brief 静态组件
	class TileCompoment :
		public ECSCompoment
	{
		CHU_COMPOMENT_DEF(ECSCompomentType::TileCompoment);
	private:
		TileObjectType m_iObjectType = TileObjectType::None;
	public:
		TileCompoment(TileObjectType iType)
			: m_iObjectType(iType) {}
	public:
		TileObjectType GetObjectType()const noexcept { return m_iObjectType; }
	};

	/// \brief 瓷砖地图系统
	class TileSystem :
		public ECSSystem
	{
	private:
		uint32_t m_iTileWidth = 0;
		uint32_t m_iTileHeight = 0;
		uint32_t m_iWidth = 0;
		uint32_t m_iHeight = 0;
		std::vector<cocos2d::RefPtr<ECSEntity>> m_arrObjectList;
		std::vector<ECSEntity*> m_arrObjectMap;
	public:
		TileSystem(uint32_t iTileWidth, uint32_t iTileHeight);
	private:
		void setObjectAt(int32_t iXIndex, int32_t iYIndex, ECSEntity* p)noexcept;
		void setObjectAt(cocos2d::Vec2 vecPosition, ECSEntity* p)noexcept;
		void clearObjectMap()noexcept;
	public:
		uint32_t GetTileWidth()const noexcept { return m_iTileWidth; }
		uint32_t GetTileHeight()const noexcept { return m_iTileHeight; }
		uint32_t GetWidth()const noexcept { return m_iWidth; }
		uint32_t GetHeight()const noexcept { return m_iHeight; }
		ECSEntity* GetObjectAt(int32_t iXIndex, int32_t iYIndex)const noexcept;
		ECSEntity* GetObjectAt(cocos2d::Vec2 vecPosition)const noexcept;
		TileObjectType GetObjectTypeAt(int32_t iXIndex, int32_t iYIndex)const noexcept;
		TileObjectType GetObjectTypeAt(cocos2d::Vec2 vecPosition)const noexcept;
		void ResetSize(uint32_t iWidth, uint32_t iHeight);
	public:
		void AddEntity(ECSEntity* p)override;
		void RemoveEntity(ECSEntity* p)override;
		void BeforeUpdate(float delta)override;
	};
}
