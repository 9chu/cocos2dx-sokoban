#include "PlayerCompoment.h"
#include "BaseCompoment.h"

#define ANIMATION_INTV 1.f
#define ANIMATION_FRAMECNT 2

NAMESPACE_SOKOBAN;

void PlayerSystem::changeAnimation(PlayerInput iInput, PlayerCompoment* pPlayerCompoment)
{
    PlayerAnimation iTargetAnimation = PlayerAnimation::DownAnimation;
    
    switch (iInput)
    {
    case PlayerInput::Left:
        iTargetAnimation = PlayerAnimation::LeftAnimation;
        break;
    case PlayerInput::Right:
        iTargetAnimation = PlayerAnimation::RightAnimation;
        break;
    case PlayerInput::Up:
        iTargetAnimation = PlayerAnimation::UpAnimation;
        break;
    case PlayerInput::Down:
        iTargetAnimation = PlayerAnimation::DownAnimation;
        break;
    default:
        assert(false);
        break;
    }
    
    if (pPlayerCompoment->GetAnimationState() != iTargetAnimation)
    {
        pPlayerCompoment->SetAnimationState(iTargetAnimation);
        
        // 交由UpdateAnimation进行动画切换
        pPlayerCompoment->SetAnimationFrame(ANIMATION_FRAMECNT);
        pPlayerCompoment->SetAnimationTimer(ANIMATION_INTV);
    }
}

void PlayerSystem::updateAnimation(float delta, BaseCompoment* pBaseCompoment, PlayerCompoment* pPlayerCompoment)
{
    bool bChangeSprite = false;
    
    // 变更动画
    float iTimer = pPlayerCompoment->SetAnimationTimer(pPlayerCompoment->GetAnimationTimer() + delta);
    if (iTimer >= ANIMATION_INTV)  // 动画间隔
    {
        uint32_t iFrame = pPlayerCompoment->SetAnimationFrame(pPlayerCompoment->GetAnimationFrame() + 1);
        if (iFrame >= ANIMATION_FRAMECNT)
            pPlayerCompoment->SetAnimationFrame(0);
        
        pPlayerCompoment->SetAnimationTimer(0.f);
        bChangeSprite = true;
    }
    
    if (bChangeSprite)
    {
        static Rect s_PlayerAnimationList[][ANIMATION_FRAMECNT] = {
            {
                Rect(2 * SOKOBAN_TILE_WIDTH, 2 * SOKOBAN_TILE_HEIGHT,
                     SOKOBAN_TILE_WIDTH, SOKOBAN_TILE_HEIGHT),
                Rect(3 * SOKOBAN_TILE_WIDTH, 2 * SOKOBAN_TILE_HEIGHT,
                     SOKOBAN_TILE_WIDTH, SOKOBAN_TILE_HEIGHT)
            },  // left animation
            {
                Rect(0 * SOKOBAN_TILE_WIDTH, 2 * SOKOBAN_TILE_HEIGHT,
                     SOKOBAN_TILE_WIDTH, SOKOBAN_TILE_HEIGHT),
                Rect(1 * SOKOBAN_TILE_WIDTH, 2 * SOKOBAN_TILE_HEIGHT,
                     SOKOBAN_TILE_WIDTH, SOKOBAN_TILE_HEIGHT)
            },  // right animation
            {
                Rect(2 * SOKOBAN_TILE_WIDTH, 3 * SOKOBAN_TILE_HEIGHT,
                     SOKOBAN_TILE_WIDTH, SOKOBAN_TILE_HEIGHT),
                Rect(3 * SOKOBAN_TILE_WIDTH, 3 * SOKOBAN_TILE_HEIGHT,
                     SOKOBAN_TILE_WIDTH, SOKOBAN_TILE_HEIGHT)
            },  // up animation
            {
                Rect(0 * SOKOBAN_TILE_WIDTH, 3 * SOKOBAN_TILE_HEIGHT,
                     SOKOBAN_TILE_WIDTH, SOKOBAN_TILE_HEIGHT),
                Rect(1 * SOKOBAN_TILE_WIDTH, 3 * SOKOBAN_TILE_HEIGHT,
                     SOKOBAN_TILE_WIDTH, SOKOBAN_TILE_HEIGHT)
            }  // down animation
        };
        
        // 获取Sprite接口
        Sprite* pSprite = dynamic_cast<Sprite*>(pBaseCompoment->GetNativeNode());
        assert(pSprite);
        
        PlayerAnimation iState = pPlayerCompoment->GetAnimationState();
        uint32_t iFrame = pPlayerCompoment->GetAnimationFrame();
        
        pSprite->setTextureRect(s_PlayerAnimationList[static_cast<int>(iState)][iFrame]);
    }
}

void PlayerSystem::UpdateEntity(float delta, ECSEntity* p)
{
	BaseCompoment* pBaseCompoment = p->GetCompoment<BaseCompoment>();
	PlayerCompoment* pPlayerCompoment = p->GetCompoment<PlayerCompoment>();
	
	// 处理玩家，通常只有一个实例
	if (pBaseCompoment && pPlayerCompoment)
	{
		// 获取玩家当前的位置
		int32_t iPlayerX = (int)(pBaseCompoment->GetPosition().x + SOKOBAN_TILE_WIDTH / 2.f) / SOKOBAN_TILE_WIDTH;
		int32_t iPlayerY = (int)(pBaseCompoment->GetPosition().y + SOKOBAN_TILE_HEIGHT / 2.f) / SOKOBAN_TILE_HEIGHT;

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
        
        // iDirectionX == 0 and iDirectionY == 0 -> iObjType == None
        assert(!(iDirectionX == 0 && iDirectionY == 0 && iObjType != TileObjectType::None));
		
        switch (iObjType)
		{
		case TileObjectType::None:
			bReachable = (iDirectionX != 0 || iDirectionY != 0);
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
        {
			pBaseCompoment->SetPosition(vecPosition);
            
            // 变更动画
            changeAnimation(tLastInput, pPlayerCompoment);
        }
        
        // 更新动画
        updateAnimation(delta, pBaseCompoment, pPlayerCompoment);
	}
}
