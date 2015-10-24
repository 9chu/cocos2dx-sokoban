/*
 * \file Common.h
 * \author chu
 * \date 2015/09/27
 */
#pragma once
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <memory>
#include <stack>
#include <typeinfo>

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

#include "SceneDefination.h"

#define NAMESPACE_SOKOBAN \
    using namespace std; \
    using namespace sokoban; \
    using namespace cocos2d; \
    using namespace cocostudio; \
    using namespace cocostudio::timeline

#define CHU_LOG(level, formattxt, ...) \
    cocos2d::log("[%s] %s:%d (%s): %s", level, __FILE__, __LINE__, __FUNCTION__, cocos2d::StringUtils::format(formattxt, ##__VA_ARGS__).c_str())

#if !defined(COCOS2D_DEBUG) || COCOS2D_DEBUG == 0
    #define CHU_LOGFATAL(format, ...)  CHU_LOG("FATAL", format, ##__VA_ARGS__)
    #define CHU_LOGERROR(...)          do {} while (0)
    #define CHU_LOGWARN(...)           do {} while (0)
    #define CHU_LOGINFO(...)           do {} while (0)
#elif COCOS2D_DEBUG == 1
    #define CHU_LOGFATAL(format, ...)  CHU_LOG("FATAL", format, ##__VA_ARGS__)
    #define CHU_LOGERROR(format, ...)  CHU_LOG("ERROR", format, ##__VA_ARGS__)
    #define CHU_LOGWARN(format, ...)   CHU_LOG("WARN", format, ##__VA_ARGS__)
    #define CHU_LOGINFO(format, ...)   do {} while (0)
#elif COCOS2D_DEBUG > 1
    #define CHU_LOGFATAL(format, ...)  CHU_LOG("FATAL", format, ##__VA_ARGS__)
    #define CHU_LOGERROR(format, ...)  CHU_LOG("ERROR", format, ##__VA_ARGS__)
    #define CHU_LOGWARN(format, ...)   CHU_LOG("WARN", format, ##__VA_ARGS__)
    #define CHU_LOGINFO(format, ...)   CHU_LOG("INFO", format, ##__VA_ARGS__)
#endif

namespace sokoban
{
    /// \brief 空字符串
    inline const std::string& empty_string()
    {
        static std::string s_strEmpty;
        return s_strEmpty;
    }
    
    /// \brief 构造一个继承自cocos2d::Ref的对象
    template <typename T, typename... Args>
    cocos2d::RefPtr<T> make_ref(Args... args)
    {
        T* pObj = new(std::nothrow) T(args...);
        if (!pObj)
        {
            CHU_LOGFATAL("allocate memory failed for '%s'.", typeid(T).name());
            ::abort();
            return nullptr;  // never return
        }
        
        cocos2d::RefPtr<T> tRet(pObj);
        pObj->release();
        return tRet;
    }
    
    /// \brief 构造一个继承自cocos2d::Ref的对象并以指针形式返回
    template <typename T, typename... Args>
    T* make_ref_temp(Args... args)
    {
        T* pObj = new(std::nothrow) T(args...);
        if (!pObj)
        {
            CHU_LOGFATAL("allocate memory failed for '%s'.", typeid(T).name());
            ::abort();
            return nullptr;  // never return
        }
        
        pObj->autorelease();
        return pObj;
    }
    
    /// \brief 不可复制类
    class Noncopyable
    {
    public:
        Noncopyable() = default;
        Noncopyable(const Noncopyable&) = delete;
        Noncopyable& operator=(const Noncopyable&)const = delete;
    };
    
    /// \brief 二段构造隐藏类
    template <typename T>
    class AutoInvokeInit :
        public T
    {
    public:
        AutoInvokeInit()
        {
            if (!T::init())
            {
                CHU_LOGERROR("failed to init base class '%s'.", typeid(T).name());
                throw std::logic_error("ConstructFailed");
            }
        }
    };
}
