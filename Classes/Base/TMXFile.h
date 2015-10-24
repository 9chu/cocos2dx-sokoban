/*
 * \file TMXFileReader.h
 * \author chu
 * \date 2015/10/2
 */
#pragma once
#include "../Common.h"

namespace sokoban
{
    /// \brief 自定义属性列表
    class TMXCustomPropertyList :
        public cocos2d::Ref
    {
    private:
        std::unordered_map<std::string, std::string> m_umPropertyList;
    public:
        const std::string& GetProperty(const char* pKey)const;
        void SetProperty(const char* pKey, const char* pValue);
        bool ContainsProperty(const char* pKey)const;
        
        /// \brief 以int32获取一个值
        int32_t GetPropertyAsInt32(const char* pKey, int32_t iDefaultValue)const;
    };
    
    /// \brief 图块集
    class TMXTileSet :
        public cocos2d::Ref
    {
    private:
        uint32_t m_iFirstGid = 0;  // 对应第一个图块的gid
        uint32_t m_iTileWidth = 0;
        uint32_t m_iTileHeight = 0;
        uint32_t m_iWidthCount = 0;  // 图片在宽度方向的图块个数
        uint32_t m_iHeightCount = 0;  // 图片在高度方向的图块个数
        std::string m_strName;  // 图块集名称
        std::string m_strPath;  // 图块图片的路径
        std::unordered_map<uint32_t, cocos2d::RefPtr<TMXCustomPropertyList>> m_umTileProperty;  // 图块属性
    public:
        TMXTileSet(uint32_t iFirstGid, uint32_t iTileWidth, uint32_t iTileHeight, const std::string& strName);
    public:
        uint32_t GetFirstGid()const { return m_iFirstGid; }
        uint32_t GetTileWidth()const { return m_iTileWidth; }
        uint32_t GetTileHeight()const { return m_iTileHeight; }
        uint32_t GetWidthCount()const { return m_iWidthCount; }
        void SetWidthCount(uint32_t iCnt) { m_iWidthCount = iCnt; }
        uint32_t GetHeightCount()const { return m_iHeightCount; }
        void SetHeightCount(uint32_t iCnt) { m_iHeightCount = iCnt; }
        uint32_t GetTileMaxCount()const { return GetWidthCount() * GetHeightCount(); }
        const std::string& GetName()const { return m_strName; }
        const std::string& GetTexturePath()const { return m_strPath; }
        void SetTexturePath(const std::string& pPath) { m_strPath = pPath; }
        const TMXCustomPropertyList* GetTileCustomPropertyList(uint32_t iIndex)const;
        TMXCustomPropertyList* GetTileCustomPropertyList(uint32_t iIndex);
    };
    
    /// \brief 地图对象
    class TMXMapObject :
        public cocos2d::Ref
    {
    private:
        uint32_t m_iID = 0;
        int m_iX = 0;
        int m_iY = 0;
        uint32_t m_iWidth = 0;
        uint32_t m_iHeight = 0;
        cocos2d::RefPtr<TMXCustomPropertyList> m_cplCustomPropertyList;
    public:
        TMXMapObject(uint32_t iID, int iX, int iY, uint32_t iWidth, uint32_t iHeight);
    public:
        uint32_t GetID()const { return m_iID; }
        int GetX()const { return m_iX; }
        int GetY()const { return m_iY; }
        uint32_t GetWidth()const { return m_iWidth; }
        uint32_t GetHeight()const { return m_iHeight; }
        const TMXCustomPropertyList* GetCustomPropertyList()const { return m_cplCustomPropertyList; }
        TMXCustomPropertyList* GetCustomPropertyList() { return m_cplCustomPropertyList; }
    };
    
    /// \brief 地图层
    class TMXMapLayerBase :
        public cocos2d::Ref
    {
    private:
        std::string m_strName;
        cocos2d::RefPtr<TMXCustomPropertyList> m_cplCustomPropertyList;
    public:
        TMXMapLayerBase(const std::string& strName);
    public:
        virtual bool IsTiledLayer()const { return false; }
        virtual bool IsObjectLayer()const { return false; }
        const std::string GetName()const { return m_strName; }
        const TMXCustomPropertyList* GetCustomPropertyList()const { return m_cplCustomPropertyList; }
        TMXCustomPropertyList* GetCustomPropertyList() { return m_cplCustomPropertyList; }
    };
    
    /// \brief Tiled地图层
    class TMXMapTiledLayer :
        public TMXMapLayerBase
    {
    private:
        uint32_t m_iWidth = 0;
        uint32_t m_iHeight = 0;
        std::vector<uint32_t> m_vecGidList;
    public:
        TMXMapTiledLayer(uint32_t iWidth, uint32_t iHeight, const std::string& strName);
    public:
        bool IsTiledLayer()const { return true; }
        uint32_t GetWidth()const { return m_iWidth; }
        uint32_t GetHeight()const { return m_iHeight; }
        
        /// \brief 根据X,Y索引取得图块Gid
        uint32_t GetGidOfXY(uint32_t iX, uint32_t iY)const;
        
        /// \brief 根据X,Y索引设置图块Gid
        void SetGidOfXY(uint32_t iX, uint32_t iY, uint32_t iGid);
    };
    
    /// \brief 对象层
    class TMXMapObjectLayer :
        public TMXMapLayerBase
    {
    private:
        std::vector<cocos2d::RefPtr<TMXMapObject>> m_vecObjects;
    public:
        TMXMapObjectLayer(const std::string& strName);
    public:
        virtual bool IsObjectLayer()const { return true; }
        size_t GetObjectCount()const { return m_vecObjects.size(); }
        
        /// \brief 通过索引获取一个对象
        const TMXMapObject* GetObject(size_t iIndex)const;
        
        /// \brief 通过ID获取一个对象
        const TMXMapObject* GetObjectById(uint32_t iID)const;
        
        /// \brief 新增一个对象
        void AddObject(TMXMapObject* p);
    };
    
    /// \brief TMX地图文件读取类
    /// \note 地图格式要求：base64无压缩，渲染方向为RightUp
    class TMXFile :
        public cocos2d::Ref
    {
        friend class TMXFileParser;
    private:
        uint32_t m_iWidth = 0;
        uint32_t m_iHeight = 0;
        uint32_t m_iTileWidth = 0;
        uint32_t m_iTileHeight = 0;
        uint32_t m_iNextObjectId = 1;
        std::vector<cocos2d::RefPtr<TMXTileSet>> m_vecTileSet;
        std::vector<cocos2d::RefPtr<TMXMapLayerBase>> m_vecMapLayer;
    public:
        TMXFile(const char* pPath);
    public:
        uint32_t GetWidth()const { return m_iWidth; }
        uint32_t GetHeight()const { return m_iHeight; }
        uint32_t GetTileWidth()const { return m_iTileWidth; }
        uint32_t GetTileHeight()const { return m_iTileHeight; }
        uint32_t GetNextObjectId()const { return m_iNextObjectId; }
        
        /// \brief 获取TileSet数量
        size_t GetTileSetCount()const { return m_vecTileSet.size(); }
        /// \brief 获取TileSet
        const TMXTileSet* GetTileSet(uint32_t iIndex)const;
        const TMXTileSet* GetTileSet(const char* pName)const;
        /// \brief 获取图层数量
        size_t GetLayerCount()const { return m_vecMapLayer.size(); }
        /// \brief 获取图层
        const TMXMapLayerBase* GetLayer(uint32_t iIndex)const;
        const TMXMapLayerBase* GetLayer(const char* pName)const;
        /// \brief 根据gid获取图块信息
        /// \param[in] iGid 图块的gid
        /// \param[out] outTexPath 图块对应的tileset纹理路径
        /// \param[out] outRect 图块在tileset上对应的纹理矩形
        /// \param[out] outPropertyList 图块对应的属性列表
        bool GetTileInfoByGid(uint32_t iGid, const char*& outTexPath, cocos2d::Rect& outRect, const TMXCustomPropertyList*& outPropertyList)const;
    };
    
    class TMXFileParser :
        public cocos2d::SAXDelegator,
        public Noncopyable
    {
        /// \brief 解析器状态
        enum class ParserState
        {
            ParseStart,  // first state
            ParseMapNode,  // in Map
            ParseTileSetNode,  // in Map\TileSet
            ParseTileSetImageNode,  // in Map\TileSet\Image
            ParseTileSetTileNode,  // in Map\TileSet\Tile
            ParseLayerNode,  // in Layer
            ParseLayerDataNode,  // in Layer\Data
            ParseObjectLayerNode,  // in ObjectGroup
            ParseObjectLayerObjectNode,  // in ObjectGroup\Object
            ParsePropertyListNode,  // general properties node
            ParsePropertyListPropertyNode,  // general properties/property node
        };
        union StatePointer
        {
            TMXFile* pFile;
            TMXTileSet* pTileSet;
            TMXCustomPropertyList* pPropertyList;
            TMXMapTiledLayer* pTiledLayer;
            TMXMapObjectLayer* pObjectLayer;
            TMXMapObject* pObject;
            
            StatePointer(TMXFile* p)
                : pFile(p) {}
            StatePointer(TMXTileSet* p)
                : pTileSet(p) {}
            StatePointer(TMXCustomPropertyList* p)
                : pPropertyList(p) {}
            StatePointer(TMXMapTiledLayer* p)
                : pTiledLayer(p) {}
            StatePointer(TMXMapObjectLayer* p)
                : pObjectLayer(p) {}
            StatePointer(TMXMapObject* p)
                : pObject(p) {}
            StatePointer(const StatePointer&) = default;
            StatePointer(StatePointer&&) = default;
        };
    private:
        bool m_bFailed = false;
        std::string m_strPath;  // 存储TMX的路径
        std::stack<std::pair<ParserState, StatePointer>> m_stState;  // 存储解析状态
    private:
        /*
        /// \brief 解码Base64字符串
        /// \param[in] pInput 输入字符串
        /// \param[in] iLength 字符串长度
        /// \param[out] pOutput 输出缓冲区，若为nullptr则不输出，只返回需要的长度
        /// \return 输出的字节数据长度，若返回(size_t)-1，说明发生错误
        size_t Base64Decode(const char* pInput, size_t iLength, char* pOutput);
        */
    public:
        TMXFileParser(TMXFile* pParent, const std::string& strPath);
    public:
        bool Failed()
        {
            if (!m_bFailed)
            {
                if (m_stState.empty())
                    m_bFailed = true;
                else if (m_stState.top().first != ParserState::ParseStart)
                    m_bFailed = true;
            }
            return m_bFailed;
        }
    public:
        void startElement(void* ctx, const char* name, const char** atts);
        void endElement(void* ctx, const char* name);
        void textHandler(void* ctx, const char* s, int len);
    };
}
