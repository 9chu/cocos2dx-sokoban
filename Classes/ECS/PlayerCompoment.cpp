#include "PlayerCompoment.h"
#include "BaseCompoment.h"

NAMESPACE_SOKOBAN;

void PlayerSystem::UpdateEntity(ECSEntity* p)
{
	BaseCompoment* pBaseCompoment = p->GetCompoment<BaseCompoment>();
	PlayerCompoment* pPlayerCompoment = p->GetCompoment<PlayerCompoment>();
	
	// 处理玩家，通常只有一个实例
	if (pBaseCompoment && pPlayerCompoment)
	{
		// 获取玩家当前的位置
		int32_t iPlayerX = (int)pBaseCompoment->GetPosition().x / SOKOBAN_TILE_WIDTH;
		int32_t iPlayerY = (int)pBaseCompoment->GetPosition().y / SOKOBAN_TILE_HEIGHT;

		// 玩家下一步前进方向
		int32_t iDirectionX = 0;
		int32_t iDirectionY = 0;
		Vec2 vecOffset(0.f, 0.f);

		// 获取玩家输入
		PlayerInput tLastInput = m_pProvider->GetPlayerLastInput();
		switch (tLastInput)
		{
		case PlayerInput::Left:
			iDirectionX = -1;
			vecOffset.x = -(float)SOKOBAN_TILE_WIDTH;
			break;
		case PlayerInput::Right:
			iDirectionX = 1;
			vecOffset.x = (float)SOKOBAN_TILE_WIDTH;
			break;
		case PlayerInput::Up:
			iDirectionY = 1;
			vecOffset.y = (float)SOKOBAN_TILE_HEIGHT;
			break;
		case PlayerInput::Down:
			iDirectionY = -1;
			vecOffset.y = -(float)SOKOBAN_TILE_HEIGHT;
			break;
		default:
			break;
		}

		// 计算下一个格子
		int32_t iPlayerNextX = iPlayerX + iDirectionX;
		int32_t iPlayerNextY = iPlayerY + iDirectionY;
		Vec2 vecPosition = pBaseCompoment->GetPosition() + vecOffset;

		// 检查是否可达
		bool bReachable = false;
		TileObjectType iObjType = m_pTileSystem->GetObjectTypeAt(iPlayerNextX, iPlayerNextY);
		switch (iObjType)
		{
		case TileObjectType::None:
			bReachable = true;
			break;
		case TileObjectType::Wall:
			bReachable = false;
			break;
		case TileObjectType::Box:
			{
				ECSEntity* pBox = m_pTileSystem->GetObjectAt(iPlayerNextX, iPlayerNextY);

				// 计算箱子下一个位置
				int32_t iBoxNextX = iPlayerNextX + iDirectionX;
				int32_t iBoxNextY = iPlayerNextY + iDirectionY;
				Vec2 vecBoxPosition = pBox->GetCompoment<BaseCompoment>()->GetPosition() + vecOffset;

				// 如果箱子下一个位置可达
				if (m_pTileSystem->GetObjectTypeAt(iBoxNextX, iBoxNextY) == TileObjectType::None)
				{
					bReachable = true;  // 玩家可达新位置

					// 将箱子放置到新的位置
					pBox->GetCompoment<BaseCompoment>()->SetPosition(vecBoxPosition);
				}
			}
			break;
		}

		// 更新玩家位置
		if (bReachable)
			pBaseCompoment->SetPosition(vecPosition);
	}
}
