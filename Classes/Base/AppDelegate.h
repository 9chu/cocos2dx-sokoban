#pragma once
#include "../Common.h"

namespace sokoban
{
    /// \brief App代理
    class AppDelegate :
        private cocos2d::Application
    {
    public:
        AppDelegate();
        virtual ~AppDelegate();
    protected:
        virtual void initGLContextAttrs();
        virtual bool applicationDidFinishLaunching();
        virtual void applicationDidEnterBackground();
        virtual void applicationWillEnterForeground();
    };
}
