#include "PlayerCompoment.h"
#include "BaseCompoment.h"

NAMESPACE_SOKOBAN;

void PlayerSystem::UpdateEntity(ECSEntity* p)
{
	BaseCompoment* pBaseCompoment = p->GetCompoment<BaseCompoment>();
	PlayerCompoment* pPlayerCompoment = p->GetCompoment<PlayerCompoment>();
	
	// ������ң�ͨ��ֻ��һ��ʵ��
	if (pBaseCompoment && pPlayerCompoment)
	{
		// ��ȡ��ҵ�ǰ��λ��
		int32_t iPlayerX = (int)pBaseCompoment->GetPosition().x / SOKOBAN_TILE_WIDTH;
		int32_t iPlayerY = (int)pBaseCompoment->GetPosition().y / SOKOBAN_TILE_HEIGHT;

		// �����һ��ǰ������
		int32_t iDirectionX = 0;
		int32_t iDirectionY = 0;
		Vec2 vecOffset(0.f, 0.f);

		// ��ȡ�������
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

		// ������һ������
		int32_t iPlayerNextX = iPlayerX + iDirectionX;
		int32_t iPlayerNextY = iPlayerY + iDirectionY;
		Vec2 vecPosition = pBaseCompoment->GetPosition() + vecOffset;

		// ����Ƿ�ɴ�
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

				// ����������һ��λ��
				int32_t iBoxNextX = iPlayerNextX + iDirectionX;
				int32_t iBoxNextY = iPlayerNextY + iDirectionY;
				Vec2 vecBoxPosition = pBox->GetCompoment<BaseCompoment>()->GetPosition() + vecOffset;

				// ���������һ��λ�ÿɴ�
				if (m_pTileSystem->GetObjectTypeAt(iBoxNextX, iBoxNextY) == TileObjectType::None)
				{
					bReachable = true;  // ��ҿɴ���λ��

					// �����ӷ��õ��µ�λ��
					pBox->GetCompoment<BaseCompoment>()->SetPosition(vecBoxPosition);
				}
			}
			break;
		}

		// �������λ��
		if (bReachable)
			pBaseCompoment->SetPosition(vecPosition);
	}
}
