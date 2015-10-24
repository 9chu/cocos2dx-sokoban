#include "TMXFile.h"

NAMESPACE_SOKOBAN;

const std::string& TMXCustomPropertyList::GetProperty(const char* pKey)const
{
    auto i = m_umPropertyList.find(pKey);
    if (i == m_umPropertyList.end())
        return empty_string();
    return i->second;
}

void TMXCustomPropertyList::SetProperty(const char* pKey, const char* pValue)
{
    m_umPropertyList[pKey] = pValue;
}

bool TMXCustomPropertyList::ContainsProperty(const char* pKey)const
{
    auto i = m_umPropertyList.find(pKey);
    if (i == m_umPropertyList.end())
        return false;
    return true;
}

int32_t TMXCustomPropertyList::GetPropertyAsInt32(const char* pKey, int32_t iDefaultValue)const
{
    const string& tValue = GetProperty(pKey);
    if (tValue.empty())
        return iDefaultValue;
    else
        return ::atoi(tValue.c_str());
}

////////////////////////////////////////////////////////////////////////////////

TMXTileSet::TMXTileSet(uint32_t iFirstGid, uint32_t iTileWidth, uint32_t iTileHeight, const std::string& strName)
{
    m_iFirstGid = iFirstGid;
    m_iTileWidth = iTileWidth;
    m_iTileHeight = iTileHeight;
    m_strName = strName;
}

const TMXCustomPropertyList* TMXTileSet::GetTileCustomPropertyList(uint32_t iIndex)const
{
    auto i = m_umTileProperty.find(iIndex);
    if (i == m_umTileProperty.end())
        return nullptr;
    return i->second;
}

TMXCustomPropertyList* TMXTileSet::GetTileCustomPropertyList(uint32_t iIndex)
{
    RefPtr<TMXCustomPropertyList>& pRet = m_umTileProperty[iIndex];
    if (pRet == nullptr)
        pRet = make_ref<TMXCustomPropertyList>();
    return pRet;
}

////////////////////////////////////////////////////////////////////////////////

TMXMapObject::TMXMapObject(uint32_t iID, int iX, int iY, uint32_t iWidth, uint32_t iHeight)
{
    m_iID = iID;
    m_iX = iX;
    m_iY = iY;
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_cplCustomPropertyList = make_ref<TMXCustomPropertyList>();
}

////////////////////////////////////////////////////////////////////////////////

TMXMapLayerBase::TMXMapLayerBase(const std::string& strName)
{
    m_strName = strName;
    m_cplCustomPropertyList = make_ref<TMXCustomPropertyList>();
}

////////////////////////////////////////////////////////////////////////////////

TMXMapTiledLayer::TMXMapTiledLayer(uint32_t iWidth, uint32_t iHeight, const std::string& strName)
    : TMXMapLayerBase(strName)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_vecGidList.resize(iWidth * iHeight);
}

uint32_t TMXMapTiledLayer::GetGidOfXY(uint32_t iX, uint32_t iY)const
{
    if (iX >= GetWidth() || iY >= GetHeight())
        return 0U;
    return m_vecGidList[iX + iY * GetWidth()];
}

void TMXMapTiledLayer::SetGidOfXY(uint32_t iX, uint32_t iY, uint32_t iGid)
{
    if (iX < GetWidth() && iY < GetHeight())
        m_vecGidList[iX + iY * GetWidth()] = iGid;
}

////////////////////////////////////////////////////////////////////////////////

TMXMapObjectLayer::TMXMapObjectLayer(const std::string& strName)
    : TMXMapLayerBase(strName)
{}

const TMXMapObject* TMXMapObjectLayer::GetObject(size_t iIndex)const
{
    if (iIndex >= GetObjectCount())
        return nullptr;
    return m_vecObjects[iIndex];
}

const TMXMapObject* TMXMapObjectLayer::GetObjectById(uint32_t iID)const
{
    for (auto& i : m_vecObjects)
    {
        if (i->GetID() == iID)
            return i;
    }
    return nullptr;
}

void TMXMapObjectLayer::AddObject(TMXMapObject* p)
{
    m_vecObjects.push_back(p);
}

////////////////////////////////////////////////////////////////////////////////

TMXFile::TMXFile(const char* pPath)
{
    TMXFileParser tParser(this, pPath);
    SAXParser tSAXParser;
    tSAXParser.setDelegator(&tParser);
    tSAXParser.parse(pPath);
    if (tParser.Failed())
    {
        CHU_LOGERROR("failed to load tmx from '%s'.", pPath);
        throw std::logic_error("LoadFailed");
    }
}

const TMXTileSet* TMXFile::GetTileSet(uint32_t iIndex)const
{
    if (iIndex >= m_vecTileSet.size())
        return nullptr;
    return m_vecTileSet[iIndex];
}

const TMXTileSet* TMXFile::GetTileSet(const char* pName)const
{
    for (auto& p : m_vecTileSet)
    {
        if (::strcmp(p->GetName().c_str(), pName) == 0)
            return p;
    }
    return nullptr;
}

const TMXMapLayerBase* TMXFile::GetLayer(uint32_t iIndex)const
{
    if (iIndex >= m_vecMapLayer.size())
        return nullptr;
    return m_vecMapLayer[iIndex];
}

const TMXMapLayerBase* TMXFile::GetLayer(const char* pName)const
{
    for (auto& p : m_vecMapLayer)
    {
        if (::strcmp(p->GetName().c_str(), pName) == 0)
            return p;
    }
    return nullptr;
}

bool TMXFile::GetTileInfoByGid(uint32_t iGid, const char*& outTexPath, cocos2d::Rect& outRect, const TMXCustomPropertyList*& outPropertyList)const
{
    for (auto& p : m_vecTileSet)
    {
        if (p->GetFirstGid() <= iGid && p->GetFirstGid() + p->GetTileMaxCount() > iGid)
        {
            uint32_t iIndex = iGid - p->GetFirstGid();
            
            outTexPath = p->GetTexturePath().c_str();
            outPropertyList = p->GetTileCustomPropertyList(iIndex);
            
            uint32_t x = iIndex % p->GetWidthCount();
            uint32_t y = iIndex / p->GetWidthCount();
            outRect.setRect(x * p->GetTileWidth(), y * p->GetTileHeight(), p->GetTileWidth(), p->GetTileHeight());
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

#define PARSEFAILED(format, ...) \
    do { \
        CHU_LOGERROR(format, ##__VA_ARGS__); \
        m_bFailed = true; \
    } while (false)

/*
size_t TMXFileParser::Base64Decode(const char* pInput, size_t iLength, char* pOutput)
{
    static const uint8_t s_bufBase64CodecTable[] =
    {
        // 0,    1,    2,    3,    4,    5,    6,    7,    8,    9,    A,    B,    C,    D,    E,    F
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x0
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x1
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, // 0x2
        0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x3
        0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, // 0x4
        0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x5
        0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, // 0x6
        0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0x7
    };
    
    if (iLength % 4 != 0)
    {
        CCLOGERROR("Base64Decode: invalid input string.");
        return static_cast<size_t>(-1);
    }
    
    size_t iOutputLength = 0;
    bool bFinished = false;
    uint8_t iBytesOut[3] = { 0 };
    
    for (size_t i = 0; i < iLength; i += 4)
    {
        uint8_t iBytes[4] =
        {
            static_cast<uint8_t>(pInput[i]),
            static_cast<uint8_t>(pInput[i + 1]),
            static_cast<uint8_t>(pInput[i + 2]),
            static_cast<uint8_t>(pInput[i + 3])
        };
        uint32_t iTailMarkCount = 0;
        
        for (size_t j = 0; j < 4; ++j)
        {
            if (iBytes[j] >= std::extent<decltype(s_bufBase64CodecTable)>::value)
            {
                CCLOGERROR("Base64Decode: invalid character at position %d.", static_cast<int>(i + j));
                return static_cast<size_t>(-1);
            }
            
            if (iBytes[j] == '=')
            {
                bFinished = true;
                ++iTailMarkCount;
            }
            else
            {
                if (bFinished)
                {
                    CCLOGERROR("Base64Decode: invalid character at poistion %d.", static_cast<int>(i + j));
                    return static_cast<size_t>(-1);
                }
                
                uint8_t b = s_bufBase64CodecTable[iBytes[j]];
                if (b == 0xFF)
                {
                    CCLOGERROR("Base64Decode: invalid character at poistion %d.", static_cast<int>(i + j));
                    return static_cast<size_t>(-1);
                }
                switch (j)
                {
                    case 0:
                        iBytesOut[0] = b << 2;
                        break;
                    case 1:
                        iBytesOut[0] |= (b & 0x30) >> 4;
                        iBytesOut[1] = (b & 0x0F) << 4;
                        break;
                    case 2:
                        iBytesOut[1] |= (b & 0x3C) >> 2;
                        iBytesOut[2] = (b & 0x03) << 6;
                        break;
                    case 3:
                        iBytesOut[2] |= b;
                        break;
                }
            }
        }
        
        if (pOutput)
            memcpy(pOutput + iOutputLength, iBytesOut, 3 - iTailMarkCount);
        iOutputLength += 3 - iTailMarkCount;
    }
    
    return iOutputLength;
}
*/

TMXFileParser::TMXFileParser(TMXFile* pParent, const std::string& strPath)
    : m_strPath(strPath)
{
    m_stState.push(make_pair(ParserState::ParseStart, pParent));
}

void TMXFileParser::startElement(void* ctx, const char* name, const char** atts)
{
    class AttributeIterator :
        public Noncopyable
    {
    private:
        const char** m_pAttrStrings;
    public:
        bool operator()(const char*& pKey, const char*& pValue)
        {
            if (m_pAttrStrings && *m_pAttrStrings)
                pKey = *m_pAttrStrings;
            else
                return false;
            ++m_pAttrStrings;
            if (*m_pAttrStrings)
                pValue = *m_pAttrStrings;
            else
                return false;
            ++m_pAttrStrings;
            return true;
        }
    public:
        AttributeIterator(const char** atts)
        : m_pAttrStrings(atts) {}
    };
    
    if (m_bFailed || m_stState.empty())
    {
        m_bFailed = true;
        return;
    }
    
    ParserState s = m_stState.top().first;
    StatePointer p = m_stState.top().second;
    AttributeIterator attrs(atts);
    
    switch (s)
    {
        case ParserState::ParseStart:
            if (::strcmp(name, "map") == 0)
            {
                const char* pKey;
                const char* pValue;
                
                while (attrs(pKey, pValue))
                {
                    if (::strcmp(pKey, "version") == 0)
                    {
                        if (::strcmp(pValue, "1.0") != 0)
                        {
                            PARSEFAILED("unsupported version '%s'.", pValue);
                            break;
                        }
                    }
                    else if (::strcmp(pKey, "orientation") == 0)
                    {
                        if (::strcmp(pValue, "orthogonal") != 0)
                        {
                            PARSEFAILED("unsupported orientation type '%s'.", pValue);
                            break;
                        }
                    }
                    else if (::strcmp(pKey, "renderorder") == 0)
                    {
                        if (::strcmp(pValue, "right-up") != 0)
                        {
                            PARSEFAILED("unsupported render order '%s'.", pValue);
                            break;
                        }
                    }
                    else if (::strcmp(pKey, "width") == 0)
                        p.pFile->m_iWidth = ::atoi(pValue);
                    else if (::strcmp(pKey, "height") == 0)
                        p.pFile->m_iHeight = ::atoi(pValue);
                    else if (::strcmp(pKey, "tilewidth") == 0)
                        p.pFile->m_iTileWidth = ::atoi(pValue);
                    else if (::strcmp(pKey, "tileheight") == 0)
                        p.pFile->m_iTileHeight = ::atoi(pValue);
                    else if (::strcmp(pKey, "nextobjectid") == 0)
                        p.pFile->m_iNextObjectId = ::atoi(pValue);
                    else
                    {
                        PARSEFAILED("unknown attribute '%s'.", pKey);
                        break;
                    }
                }
                
                if (!m_bFailed)
                {
                    if (p.pFile->m_iWidth == 0 || p.pFile->m_iHeight == 0 || p.pFile->m_iTileWidth == 0 || p.pFile->m_iTileHeight == 0)
                        PARSEFAILED("invalid map arguments.");
                    else
                        m_stState.push(make_pair(ParserState::ParseMapNode, std::move(p)));
                }
            }
            else
                PARSEFAILED("node 'map' required, but found '%s'.", name);
            break;
        case ParserState::ParseMapNode:
            if (::strcmp(name, "tileset") == 0)
            {
                const char* pKey;
                const char* pValue;
                
                uint32_t iFirstGid = 0;
                string strName;
                uint32_t iTileWidth = 0;
                uint32_t iTileHeight = 0;
                
                while (attrs(pKey, pValue))
                {
                    if (::strcmp(pKey, "firstgid") == 0)
                        iFirstGid = ::atoi(pValue);
                    else if (::strcmp(pKey, "name") == 0)
                        strName = pValue;
                    else if (::strcmp(pKey, "tilewidth") == 0)
                        iTileWidth = ::atoi(pValue);
                    else if (::strcmp(pKey, "tileheight") == 0)
                        iTileHeight = ::atoi(pValue);
                    else
                    {
                        PARSEFAILED("unknown attribute '%s'.", pKey);
                        break;
                    }
                }
                
                if (!m_bFailed)
                {
                    if (iTileWidth != p.pFile->GetTileWidth() || iTileHeight != p.pFile->GetTileHeight())
                        PARSEFAILED("invalid tileset size, w=%d h=%d.", iTileWidth, iTileHeight);
                    else if (p.pFile->GetTileSet(strName.c_str() != nullptr))
                        PARSEFAILED("duplicated tileset name '%s'.", strName.c_str());
                    else
                    {
                        TMXTileSet* pSet = make_ref_temp<TMXTileSet>(iFirstGid, iTileWidth, iTileHeight, strName);
                        p.pFile->m_vecTileSet.push_back(pSet);
                        m_stState.push(make_pair(ParserState::ParseTileSetNode, pSet));
                    }
                }
            }
            else if (::strcmp(name, "layer") == 0)
            {
                const char* pKey;
                const char* pValue;
                
                string strName;
                uint32_t iWidth = 0;
                uint32_t iHeight = 0;
                
                while (attrs(pKey, pValue))
                {
                    if (::strcmp(pKey, "name") == 0)
                        strName = pValue;
                    else if (::strcmp(pKey, "width") == 0)
                        iWidth = ::atoi(pValue);
                    else if (::strcmp(pKey, "height") == 0)
                        iHeight = ::atoi(pValue);
                    else
                    {
                        PARSEFAILED("unknown attribute '%s'.", pKey);
                        break;
                    }
                }
                
                if (!m_bFailed)
                {
                    if (iWidth != p.pFile->GetWidth() || iHeight != p.pFile->GetHeight())
                        PARSEFAILED("invalid layer size, w=%d h=%d.", iWidth, iHeight);
                    else if (p.pFile->GetLayer(strName.c_str()))
                        PARSEFAILED("duplicated layer name '%s'.", strName.c_str());
                    else
                    {
                        TMXMapTiledLayer* pLayer = make_ref_temp<TMXMapTiledLayer>(iWidth, iHeight, strName);
                        p.pFile->m_vecMapLayer.push_back(pLayer);
                        m_stState.push(make_pair(ParserState::ParseLayerNode, pLayer));
                    }
                }
            }
            else if (::strcmp(name, "objectgroup") == 0)
            {
                const char* pKey;
                const char* pValue;
                
                string strName;
                
                while (attrs(pKey, pValue))
                {
                    if (::strcmp(pKey, "name") == 0)
                        strName = pValue;
                    else
                    {
                        PARSEFAILED("unknown attribute '%s'.", pKey);
                        break;
                    }
                }
                
                if (!m_bFailed)
                {
                    if (p.pFile->GetLayer(strName.c_str()))
                        PARSEFAILED("duplicated layer name '%s'.", strName.c_str());
                    else
                    {
                        TMXMapObjectLayer* pLayer = make_ref_temp<TMXMapObjectLayer>(strName);
                        p.pFile->m_vecMapLayer.push_back(pLayer);
                        m_stState.push(make_pair(ParserState::ParseObjectLayerNode, pLayer));
                    }
                }
            }
            else
                PARSEFAILED("unsupported node '%s'.", name);
            break;
        case ParserState::ParseTileSetNode:
            if (::strcmp(name, "image") == 0)
            {
                const char* pKey;
                const char* pValue;
                
                while (attrs(pKey, pValue))
                {
                    if (::strcmp(pKey, "source") == 0)
                    {
                        string tFullPath = FileUtils::getInstance()->fullPathFromRelativeFile(pValue, m_strPath);
                        p.pTileSet->SetTexturePath(tFullPath);
                    }
                    else if (::strcmp(pKey, "width") == 0)
                    {
                        uint32_t iImageWidth = ::atoi(pValue);
                        p.pTileSet->SetWidthCount(iImageWidth / p.pTileSet->GetTileWidth());
                    }
                    else if (::strcmp(pKey, "height") == 0)
                    {
                        uint32_t iImageHeight = ::atoi(pValue);
                        p.pTileSet->SetHeightCount(iImageHeight / p.pTileSet->GetTileHeight());
                    }
                    else
                    {
                        PARSEFAILED("unknown attribute '%s'.", pKey);
                        break;
                    }
                }
                
                if (!m_bFailed)
                    m_stState.push(make_pair(ParserState::ParseTileSetImageNode, p.pTileSet));
            }
            else if (::strcmp(name, "tile") == 0)
            {
                const char* pKey;
                const char* pValue;
                
                uint32_t iIndex = 0;
                
                while (attrs(pKey, pValue))
                {
                    if (::strcmp(pKey, "id") == 0)
                        iIndex = ::atoi(pValue);
                    else
                    {
                        PARSEFAILED("unknown attribute '%s'.", pKey);
                        break;
                    }
                }
                
                if (!m_bFailed)
                {
                    TMXCustomPropertyList* pPropList = p.pTileSet->GetTileCustomPropertyList(iIndex);
                    m_stState.push(make_pair(ParserState::ParseTileSetTileNode, pPropList));
                }
            }
            else
                PARSEFAILED("unsupported node '%s'.", name);
            break;
        case ParserState::ParseTileSetImageNode:
            PARSEFAILED("unexpected node '%s'.", name);
            break;
        case ParserState::ParseTileSetTileNode:
            if (::strcmp(name, "properties") == 0)
                m_stState.push(make_pair(ParserState::ParsePropertyListNode, p.pPropertyList));
            else
                PARSEFAILED("unexpected node '%s'.", name);
            break;
        case ParserState::ParseLayerNode:
            if (::strcmp(name, "properties") == 0)
                m_stState.push(make_pair(ParserState::ParsePropertyListNode, p.pTiledLayer->GetCustomPropertyList()));
            else if (::strcmp(name, "data") == 0)
            {
                const char* pKey;
                const char* pValue;
                
                while (attrs(pKey, pValue))
                {
                    if (::strcmp(pKey, "encoding") == 0)
                    {
                        if (::strcmp(pValue, "base64") != 0)
                        {
                            PARSEFAILED("unknown encoding '%s'.", pValue);
                            break;
                        }
                    }
                    else
                    {
                        PARSEFAILED("unknown attribute '%s'.", pKey);
                        break;
                    }
                }
                
                if (!m_bFailed)
                    m_stState.push(make_pair(ParserState::ParseLayerDataNode, std::move(p)));
            }
            else
                PARSEFAILED("unexpected node '%s'.", name);
            break;
        case ParserState::ParseLayerDataNode:
            PARSEFAILED("unexpected node '%s'.", name);
            break;
        case ParserState::ParseObjectLayerNode:
            if (::strcmp(name, "properties") == 0)
                m_stState.push(make_pair(ParserState::ParsePropertyListNode, p.pObjectLayer->GetCustomPropertyList()));
            else if (::strcmp(name, "object") == 0)
            {
                const char* pKey;
                const char* pValue;
                
                uint32_t iID = 0;
                int iX = 0;
                int iY = 0;
                uint32_t iWidth = 0;
                uint32_t iHeight = 0;
                
                while (attrs(pKey, pValue))
                {
                    if (::strcmp(pKey, "id") == 0)
                        iID = ::atoi(pValue);
                    else if (::strcmp(pKey, "x") == 0)
                        iX = ::atoi(pValue);
                    else if (::strcmp(pKey, "y") == 0)
                        iY = ::atoi(pValue);
                    else if (::strcmp(pKey, "width") == 0)
                        iWidth = ::atoi(pValue);
                    else if (::strcmp(pKey, "height") == 0)
                        iHeight = ::atoi(pValue);
                    else
                    {
                        PARSEFAILED("unknown attribute '%s'.", pKey);
                        break;
                    }
                }
                
                if (!m_bFailed)
                {
                    TMXMapObject* pObj = make_ref_temp<TMXMapObject>(iID, iX, iY, iWidth, iHeight);
                    p.pObjectLayer->AddObject(pObj);
                    m_stState.push(make_pair(ParserState::ParseObjectLayerObjectNode, pObj));
                }
            }
            else
                PARSEFAILED("unexpected node '%s'.", name);
            break;
        case ParserState::ParseObjectLayerObjectNode:
            if (::strcmp(name, "properties") == 0)
                m_stState.push(make_pair(ParserState::ParsePropertyListNode, p.pObject->GetCustomPropertyList()));
            else
                PARSEFAILED("unexpected node '%s'.", name);
            break;
        case ParserState::ParsePropertyListNode:
            if (::strcmp(name, "property") == 0)
            {
                const char* pKey;
                const char* pValue;
                
                string strPropName;
                string strPropValue;
                
                while (attrs(pKey, pValue))
                {
                    if (::strcmp(pKey, "name") == 0)
                        strPropName = pValue;
                    else if (::strcmp(pKey, "value") == 0)
                        strPropValue = pValue;
                    else
                    {
                        PARSEFAILED("unknown attribute '%s'.", pKey);
                        break;
                    }
                }
                
                if (!m_bFailed)
                {
                    p.pPropertyList->SetProperty(strPropName.c_str(), strPropValue.c_str());
                    m_stState.push(make_pair(ParserState::ParsePropertyListPropertyNode, std::move(p)));
                }
            }
            else
                PARSEFAILED("unexpected node '%s'.", name);
            break;
        case ParserState::ParsePropertyListPropertyNode:
            PARSEFAILED("unexpected node '%s'.", name);
            break;
        default:
            m_bFailed = true;
            CCASSERT(0, "TMXFileParser: unexpected code routine.");
            break;
    }
}

#define MATCHNODE(node_name) \
    if (::strcmp(name, node_name) == 0) \
        m_stState.pop(); \
    else \
        PARSEFAILED("mismatched node '%s', node '%s' expected.", name, node_name);

void TMXFileParser::endElement(void* ctx, const char* name)
{
    if (m_bFailed || m_stState.empty())
    {
        m_bFailed = true;
        return;
    }
    
    ParserState s = m_stState.top().first;
    
    switch (s)
    {
        case ParserState::ParseStart:
            PARSEFAILED("mismatched node '%s'.", name);
            break;
        case ParserState::ParseMapNode:
            MATCHNODE("map");
            break;
        case ParserState::ParseTileSetNode:
            MATCHNODE("tileset");
            break;
        case ParserState::ParseTileSetImageNode:
            MATCHNODE("image");
            break;
        case ParserState::ParseTileSetTileNode:
            MATCHNODE("tile");
            break;
        case ParserState::ParseLayerNode:
            MATCHNODE("layer");
            break;
        case ParserState::ParseLayerDataNode:
            MATCHNODE("data");
            break;
        case ParserState::ParseObjectLayerNode:
            MATCHNODE("objectgroup");
            break;
        case ParserState::ParseObjectLayerObjectNode:
            MATCHNODE("object");
            break;
        case ParserState::ParsePropertyListNode:
            MATCHNODE("properties");
            break;
        case ParserState::ParsePropertyListPropertyNode:
            MATCHNODE("property");
            break;
        default:
            m_bFailed = true;
            CCASSERT(0, "TMXFileParser: unexpected code routine.");
            break;
    }
}

void TMXFileParser::textHandler(void* ctx, const char* text, int len)
{
    if (m_bFailed || m_stState.empty())
    {
        m_bFailed = true;
        return;
    }
    
    ParserState s = m_stState.top().first;
    StatePointer p = m_stState.top().second;
    
    switch (s)
    {
        case ParserState::ParseStart:
        case ParserState::ParseMapNode:
        case ParserState::ParseTileSetNode:
        case ParserState::ParseTileSetImageNode:
        case ParserState::ParseTileSetTileNode:
        case ParserState::ParseLayerNode:
        case ParserState::ParseObjectLayerNode:
        case ParserState::ParseObjectLayerObjectNode:
        case ParserState::ParsePropertyListNode:
        case ParserState::ParsePropertyListPropertyNode:
            PARSEFAILED("unexpected data.");
            break;
        case ParserState::ParseLayerDataNode:
            // 手动trim
            while (len > 0 && text[len - 1] > 0 && ::isspace(text[len - 1]))
                --len;
            while (len > 0 && text[0] > 0 && ::isspace(text[0]))
            {
                --len;
                ++text;
            }
            
            // 解码base64串
            if (len > 0)
            {
                unsigned char* pOut = nullptr;
                int decodeLen = base64Decode((const unsigned char*)text, len, &pOut);
                if (decodeLen == p.pTiledLayer->GetWidth() * p.pTiledLayer->GetHeight() * sizeof(uint32_t))
                {
                    // 解析图块数据
                    const uint32_t* pGid = reinterpret_cast<uint32_t*>(pOut);
                    for (uint32_t iY = 0; iY < p.pTiledLayer->GetHeight(); ++iY)
                    {
                        for (uint32_t iX = 0; iX < p.pTiledLayer->GetWidth(); ++iX)
                            p.pTiledLayer->SetGidOfXY(iX, p.pTiledLayer->GetHeight() - iY - 1, *(pGid++));
                    }
                    free(pOut);
                }
                else
                {
                    if (pOut)
                        free(pOut);
                    PARSEFAILED("failed to decode map data, size %d mistached.", decodeLen);
                }
            }
            else
            {
                CCLOGWARN("TMXFileParser: empty layer data.");
            }
            break;
        default:
            m_bFailed = true;
            CCASSERT(0, "TMXFileParser: unexpected code routine.");
            break;
    }
}
