#include "GameScene.h"

cocos2d::Scene* GameScene::createScene() 
{
    return GameScene::create();
}
GameScene::~GameScene() 
{
    CC_SAFE_DELETE(_manager);
}
bool GameScene::init() 
{
    if (!Scene::init()) return false;
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    //创建主牌区 (1080 * 1500, 绿色) 
    auto mainArea = cocos2d::LayerColor::create(cocos2d::Color4B(34, 139, 34, 255), 1080, 1500);
    if (mainArea) 
    {
        mainArea->setPosition(cocos2d::Vec2(origin.x + (visibleSize.width - 1080) / 2, origin.y + 600));
        this->addChild(mainArea, -2);
    }
    //创建堆牌区 (1080 * 580, 紫色)
    auto deckArea = cocos2d::LayerColor::create(cocos2d::Color4B(128, 0, 128, 255), 1080, 580);
    if (deckArea) 
    {
        deckArea->setPosition(cocos2d::Vec2(origin.x + (visibleSize.width - 1080) / 2, origin.y + 20));
        this->addChild(deckArea, -2);
    }
    //实例化逻辑处理器
    _manager = new GameManager();
    // 创建回退按钮
    cocos2d::MenuItemFont::setFontSize(40);
    // 创建文字菜单项：参数1是显示的文字，参数2是点击后的回调函数
    auto undoItem = cocos2d::MenuItemFont::create("回退", [this](Ref* sender) 
        {
        CCLOG("Undo clicked!"); // 调试日志，确认点击有效
        if (_manager) 
        {
            _manager->undo();
        }
        });
    //设置文字颜色
    undoItem->setColor(cocos2d::Color3B::WHITE);
    // 放在堆牌区右边
    undoItem->setPosition(cocos2d::Vec2(origin.x + visibleSize.width - 100, origin.y + 580/2));
    // 创建菜单容器并添加
    auto menu = cocos2d::Menu::create(undoItem, NULL);
    menu->setPosition(cocos2d::Vec2::ZERO);
    this->addChild(menu, 10);
    //注册触摸监听
    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    _manager->initGame(this);//开始初始化
    return true;
}

bool GameScene::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    cocos2d::Vec2 location = touch->getLocation();
    // 有人点屏幕了
    _manager->handleTouch(location);
    return true;
}