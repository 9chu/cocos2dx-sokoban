#include "GameScene.h"

#include "../Base/CocosHelper.h"

NAMESPACE_SOKOBAN;

GameStageLayer::GameStageLayer()
{
    // 初始化ECS系统
    m_pBaseSystem = make_ref<BaseSystem>(this);
    
    // 添加所有的System
    AddSystem(m_pBaseSystem.get());

	// !TODO
	LoadLevel("Level/1.tmx");
}

void GameStageLayer::update(float delta)
{
    // 更新组件逻辑
    ECSStage::update(delta);
}

void GameStageLayer::LoadLevel(const char* pPath)
{
	RefPtr<TMXFile> pFile = make_ref<TMXFile>(pPath);

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
						// !TODO...

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
    AddLayer(make_ref<GameStageLayer>().get());
    
    // ! FOR TEST
    // LoadMap("Level/Level1-1.tmx");
}

/*
void GameScene::LoadMap(const char* pPath)
{
    
}

void GameScene::LoadTiledLayer(const TMXFile* pFile, const TMXMapTiledLayer* pLayer)
{
    }

void GameScene::LoadObjectLayer(const TMXFile* pFile, const TMXMapTiledLayer* pLayer)
{
    AAZ_LOGINFO("map load. %s", pLayer->GetName().c_str());
}
 
 cocos2d::RefPtr<ECSCocosNode> ECSCocosNode::CreateFromTMXTiledLayer(TMXFile* pFile, uint32_t iLayerIndex)
 {
 const TMXMapLayerBase* pLayerBase = pFile->GetLayer(iLayerIndex);
 if (!pLayerBase)
 {
 AAZ_LOGERROR("failed to load layer with index %d.", iLayerIndex);
 return nullptr;
 }
 else if (!pLayerBase->IsTiledLayer())
 {
 AAZ_LOGERROR("invalid operation, layer isn't a tiled layer.");
 return nullptr;
 }
 
 const TMXMapTiledLayer* pLayer = static_cast<const TMXMapTiledLayer*>(pLayerBase);
 
 // 构建一个父节点存储所有对象
 RefPtr<Node> pParentNode = Node::create();
 for (uint32_t y = 0; y < pLayer->GetHeight(); ++y)
 {
 for (uint32_t x = 0; x < pLayer->GetWidth(); ++x)
 {
 uint32_t iGid = pLayer->GetGidOfXY(x, y);
 if (iGid > 0)
 {
 const char* pTexPath;
 cocos2d::Rect tRect;
 const TMXCustomPropertyList* pPropertyList;
 
 if (pFile->GetTileInfoByGid(iGid, pTexPath, tRect, pPropertyList))
 {
 if (pTexPath == nullptr || *pTexPath == '\0')
 AAZ_LOGWARN("null or empty texture path.");
 else
 {
 Sprite* pSprite = Sprite::create(pTexPath, tRect);
 pSprite->getTexture()->setAliasTexParameters();
 pSprite->setAnchorPoint(Vec2(0.f, 0.f));
 pSprite->setPosition(Vec2(x * tRect.size.width, y * tRect.size.height));
 pParentNode->addChild(pSprite, -static_cast<int>(y));
 }
 }
 }
 }
 }
 
 // 创建ECS组件
 return make_ref<ECSCocosNode>(pParentNode);
 }

*/
