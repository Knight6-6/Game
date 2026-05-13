#pragma once
#include <vector>
#include "Card.h"
#include <cocos2d.h>
#include <stack>

class GameManager 
{
private:
    std::vector<Card*> _deck;        // 手牌堆
    std::vector<Card*> _tableau;     // 桌面上的牌
    Card* _activeCard = nullptr;     // 消除区当前最上面那张可见牌
    // 回退记录
    struct Memento 
    {
        Card* card;
        cocos2d::Vec2 fromPos;
        Card* lastActiveCard;        // 移动前消除区的那张牌
        bool isFromDeck;             // 是从手牌堆翻上来的，还是从桌面点下去的
		int localZOrder;             // 回退时恢复的层级
    }; 
    std::stack<Memento> _history;
    cocos2d::Vec2 _wastePilePos;     // 消除区坐标
public:
	void refreshTableauCoverStatus(); // 刷新桌面牌的覆盖状态
    void initGame(cocos2d::Scene* scene); // 接收场景指针
	void handleTouch(cocos2d::Vec2 location);//处理触摸事件，判断点击了哪里
    void onDeckClicked();            // 处理点击手牌堆翻牌
    void onTableauCardClicked(Card* card); // 处理点击桌面牌
    void undo();                     // 回退
};