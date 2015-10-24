#include "GameScene.h"

#include "../Base/CocosHelper.h"

#define TOUCH_EMIT_LENGTH 30.f

NAMESPACE_SOKOBAN;

PlayerInputLayer::PlayerInputLayer()
{
	scheduleUpdate();  // 启动update
}

void PlayerInputLayer::update(float delta)
{
	m_iInputState = PlayerInput::None;
	if (m_pTouch)
	{
		Vec2 tOffset = m_vecNewPosition - m_vecBasePosition;
		if (tOffset.length() >= TOUCH_EMIT_LENGTH)
		{
			float ang = ::atan2(tOffset.y, tOffset.x);
			if (ang >= 3.1415926f / 4.f * 3)
				m_iInputState = PlayerInput::Left;
			else if (ang >= 3.1415926f / 4.f)
				m_iInputState = PlayerInput::Up;
			else if (ang >= -3.1415926f / 4.f)
				m_iInputState = PlayerInput::Right;
			else if (ang >= -3.1415926f / 4.f * 3.f)
				m_iInputState = PlayerInput::Down;
			else
				m_iInputState = PlayerInput::Left;
			m_vecBasePosition = m_vecNewPosition;
		}
	}
}

bool PlayerInputLayer::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* unused_event)
{
	if (m_pTouch)
		return false;

	m_pTouch = pTouch;
	m_vecNewPosition = m_vecBasePosition = pTouch->getLocation();
	return true;
}

void PlayerInputLayer::onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* unused_event)
{
	m_vecNewPosition = pTouch->getLocation();
}

void PlayerInputLayer::onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* unused_event)
{
	m_pTouch = nullptr;
}

void PlayerInputLayer::onTouchCancelled(cocos2d::Touch* pTouch, cocos2d::Event* unused_event)
{
	m_pTouch = nullptr;
}

PlayerInput PlayerInputLayer::GetPlayerLastInput()const noexcept
{
	return m_iInputState;
}

////////////////////////////////////////////////////////////////////////////////

GameStageLayer::GameStageLayer(IPlayerInputProvider* pInput)
{
    // 初始化ECS系统
    m_pBaseSystem = make_ref<BaseSystem>(this);
	m_pTileSystem = make_ref<TileSystem>(SOKOBAN_TILE_WIDTH, SOKOBAN_TILE_HEIGHT);
	m_pTriggerSystem = make_ref<TriggerSystem>(SOKOBAN_TILE_WIDTH, SOKOBAN_TILE_HEIGHT);
	m_pPlayerSystem = make_ref<PlayerSystem>(m_pTileSystem, pInput);

    // 添加所有的System
    AddSystem(m_pBaseSystem.get());
	AddSystem(m_pTileSystem.get());
	AddSystem(m_pTriggerSystem.get());
	AddSystem(m_pPlayerSystem.get());

	// !TODO
	LoadLevel("Level/1.tmx");
}

void GameStageLayer::update(float delta)
{
    // 更新组件逻辑
    ECSStage::update(delta);
}

void GameStageLayer::onEnterTrigger(ECSEntity* pTrigger, ECSEntity* pEmitter)
{
	// !TODO
	CHU_LOGINFO("hit enter");
}

void GameStageLayer::onLeaveTrigger(ECSEntity* pTrigger, ECSEntity* pEmitter)
{
	// !TODO
	CHU_LOGINFO("hit leave");
}

void GameStageLayer::LoadLevel(const char* pPath)
{
	RefPtr<TMXFile> pFile = make_ref<TMXFile>(pPath);

	// 设置系统的大小
	m_pTileSystem->ResetSize(pFile->GetWidth(), pFile->GetHeight());

	// 遍历所有层次，创建地图
	for (uint32_t i = 0; i < pFile->GetLayerCount(); ++i)
	{
		const TMXMapLayerBase* pLayer = pFile->GetLayer(i);
		const TMXCustomPropertyList* pPropertyList = pLayer->GetCustomPropertyList();

		if (!pLayer->IsTiledLayer())
			CHU_LOGERROR("unsupported layer type (Layer %s).", pLayer->GetName());
		else
		{
			const TMXMapTiledLayer* pTiledLayer = static_cast<const TMXMapTiledLayer*>(pLayer);

			for (uint32_t y = 0; y < pTiledLayer->GetHeight(); ++y)
			{
				for (uint32_t x = 0; x < pTiledLayer->GetWidth(); ++x)
				{
					uint32_t iGid = pTiledLayer->GetGidOfXY(x, y);
					
					if (iGid > 0)
					{
						const char* pTexPath;
						cocos2d::Rect tRect;
						const TMXCustomPropertyList* pPropertyList;

						// 创建ECS对象
						RefPtr<ECSEntity> pEntity = make_ref<ECSEntity>();

						if (pFile->GetTileInfoByGid(iGid, pTexPath, tRect, pPropertyList))
						{
							if (pTexPath == nullptr || *pTexPath == '\0')
								CHU_LOGWARN("null or empty texture path.");
							else
							{
								Sprite* pSprite = Sprite::create(pTexPath, tRect);
								pSprite->getTexture()->setAliasTexParameters();
								pSprite->setAnchorPoint(Vec2(0.f, 0.f));
								pSprite->setPosition(Vec2(x * tRect.size.width, y * tRect.size.height));
								
								pEntity->SetCompoment(make_ref<BaseCompoment>(pSprite));
							}
						}

						// 根据PropertyList实例化其他ECS组件
						if (pPropertyList->ContainsProperty("type"))
						{
							const string& tValue = pPropertyList->GetProperty("type");
							if (tValue == "wall")
								pEntity->SetCompoment(make_ref<TileCompoment>(TileObjectType::Wall));
							else if (tValue == "box")
							{
								pEntity->SetCompoment(make_ref<TileCompoment>(TileObjectType::Box));
								pEntity->SetCompoment(make_ref<TriggerEmitterCompoment>());
							}
							else if (tValue == "target")
							{
								RefPtr<TriggerCompoment> pTriggerCompoment = make_ref<TriggerCompoment>();
								pEntity->SetCompoment(pTriggerCompoment);

								// 绑定事件回调
								pTriggerCompoment->SetEnterCallback(std::bind(&GameStageLayer::onEnterTrigger, this, placeholders::_1, placeholders::_2));
								pTriggerCompoment->SetLeaveCallback(std::bind(&GameStageLayer::onLeaveTrigger, this, placeholders::_1, placeholders::_2));
							}
							else if (tValue == "player")
								pEntity->SetCompoment(make_ref<PlayerCompoment>());
							else
								CHU_LOGWARN("ignore unknown type '%s'.", tValue.c_str());
						}

						AddEntity(pEntity);
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

GameScene::GameScene()
{
	auto p = make_ref<PlayerInputLayer>();
	AddLayer(p.get(), 2);
    AddLayer(make_ref<GameStageLayer>(p.get()).get(), 1);
}
