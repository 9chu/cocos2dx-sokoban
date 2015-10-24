#include "AppDelegate.h"

// 入口场景
#include "../Scene/GameScene.h"

NAMESPACE_SOKOBAN;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
}

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // 初始化Director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview)
    {
        glview = GLViewImpl::createWithRect(u8"推箱子", Rect(0, 0, SOKOBAN_DESIGN_WIDTH, SOKOBAN_DESIGN_HEIGHT));
        director->setOpenGLView(glview);
    }

    // 分辨率适配
    director->getOpenGLView()->setDesignResolutionSize(SOKOBAN_DESIGN_WIDTH, SOKOBAN_DESIGN_HEIGHT, ResolutionPolicy::FIXED_HEIGHT);

    // 显示FPS
    director->setDisplayStats(true);

    // 设置帧率为60FPS
    director->setAnimationInterval(1.0f / 60);

    // 增加资源路径
    FileUtils::getInstance()->addSearchPath("res");
    
    // 运行
    auto p = make_ref<GameScene>();
    director->runWithScene(p);
    
    return true;
}

void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
