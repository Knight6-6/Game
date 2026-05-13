#pragma once
#include <cocos2d.h>
#include "GameManager.h"

class GameScene : public cocos2d::Scene 
{
public:
    virtual ~GameScene(); 
    static cocos2d::Scene* createScene(); 
    virtual bool init() override;
    CREATE_FUNC(GameScene);
private:
    GameManager* _manager; 
    // 点击回调
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
};