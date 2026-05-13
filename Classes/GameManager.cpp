#include "GameManager.h"

void GameManager::initGame(cocos2d::Scene* scene) {
    //清理旧数据
    _deck.clear();
    _tableau.clear();
    int zOrder = 100;
    _wastePilePos = cocos2d::Vec2(750, 280);
    // 定义花色方便使用
    std::string s[] = { "club", "diamond", "heart", "spade" };
    std::string c[] = { "black", "red", "red", "black" };
    //构造主牌区 (Tableau)
    int tableauNums[] = {10, 11, 12, 13, 4, 5, 6, 7,  };
    for (int i = 0; i < 8; i++) 
    {
        auto card = Card::createWithData(tableauNums[i], s[i % 4], c[i % 4], zOrder++);
        if (card) 
        {
            card->setFaceUp(true);
            float x = 250 + (i % 4) * 200;
            float y = 1500 - (i / 4) * 210;
            card->setPosition(cocos2d::Vec2(x, y));
            scene->addChild(card, zOrder);
            _tableau.push_back(card);
        }
    }
    //构造手牌堆 (Deck)
    int deckNums[] = { 1, 9, 3 };
    for (int i = 0; i < 3; i++) 
    {
        auto card = Card::createWithData(deckNums[i], s[i % 4], c[i % 4], zOrder++);
        if (card) 
        {
            card->setFaceUp(false); // 还没翻开的牌
			float offsetX = i * 30.0f; // 每张牌稍微错开一点，形成叠牌效果
            card->setPosition(cocos2d::Vec2(150 + offsetX, 280));
            scene->addChild(card, zOrder);
            _deck.push_back(card);
        }
    }
    refreshTableauCoverStatus();
    //自动翻开第一张起手牌
    if (!_deck.empty()) {
        _activeCard = _deck.back();
        _deck.pop_back();
        auto delay = cocos2d::DelayTime::create(1.0f);
        auto move = cocos2d::MoveTo::create(0.3f, _wastePilePos);
        auto callback = cocos2d::CallFunc::create([this]() {
            _activeCard->setFaceUp(true);
            _activeCard->setLocalZOrder(100);
            CCLOG("Game Start! Active Card is 3. Goal: Clear 4-7 and 10-K");
            });

        _activeCard->runAction(cocos2d::Sequence::create(delay, callback, move, nullptr));
    }
}
void GameManager::refreshTableauCoverStatus() 
{
    // 遍历所有桌面牌
    for (size_t i = 0; i < _tableau.size(); ++i) 
    {
        Card* cardA = _tableau[i];
        bool isCovered = false;
        // 检查是否有任何层级更高的牌压住了 cardA
        for (size_t j = 0; j < _tableau.size(); ++j) 
        {
            if (i == j) continue;
            Card* cardB = _tableau[j];
            if (cardB->getLocalZOrder() > cardA->getLocalZOrder()) 
            {
                if (cardA->getBoundingBox().intersectsRect(cardB->getBoundingBox())) 
                {
                    isCovered = true;
                    break; //只要被一张压住，就标记为覆盖
                }
            }
        }
        //没被压住就 FaceUp(true)，被压住就 FaceUp(false)
        cardA->setFaceUp(!isCovered);
    }
}

void GameManager::handleTouch(cocos2d::Vec2 location) {
    //桌面牌检测
    for (int i = (int)_tableau.size() - 1; i >= 0; --i) 
    {
        Card* card = _tableau[i];
        if (card->isFaceUp() && card->getBoundingBox().containsPoint(location)) 
        {
            this->onTableauCardClicked(card);
            return;
        }
    }
    //手牌堆检测
    if (!_deck.empty()) 
    {
        Card* topCard = _deck.back();
        if (topCard->getBoundingBox().containsPoint(location)) {
            this->onDeckClicked();
            return;
        }
    }
}
void GameManager::onTableauCardClicked(Card* clickedCard) {
    if (!_activeCard) return;

    int diff = abs(clickedCard->getCardNumber() - _activeCard->getCardNumber());
    if (diff == 1 || diff == 12) {
        //存入回退记录
        Memento m = { clickedCard, clickedCard->getPosition(), _activeCard, false, clickedCard->getLocalZOrder() };
        _history.push(m);
        //移动到消除区
        clickedCard->setLocalZOrder(_activeCard->getLocalZOrder() + 1);
        clickedCard->runAction(cocos2d::MoveTo::create(0.2f, _wastePilePos));
        //从桌面名单移除
        auto it = std::find(_tableau.begin(), _tableau.end(), clickedCard);
        if (it != _tableau.end()) 
        {
            _tableau.erase(it);
        }
        //更新当前目标牌
        _activeCard = clickedCard;
        refreshTableauCoverStatus();
    }
}

void GameManager::onDeckClicked() 
{
    if (_deck.empty()) return;
    //取出最上面的一张
    Card* newCard = _deck.back();
    _deck.pop_back();
    //存入回退记录
    Memento m = { newCard, newCard->getPosition(), _activeCard, true, newCard->getLocalZOrder() };
    _history.push(m);
    //翻转并移动到消除区
    newCard->setFaceUp(true);
    newCard->setLocalZOrder(_activeCard ? _activeCard->getLocalZOrder() + 1 : 1);
    auto move = cocos2d::MoveTo::create(0.2f, _wastePilePos);
    newCard->runAction(move);
    //更新当前目标牌
    _activeCard = newCard;
    refreshTableauCoverStatus();
}

void GameManager::undo() {
    if (_history.empty()) return;
    Memento last = _history.top();
    _history.pop();
    //恢复之前的身份层级
    last.card->setLocalZOrder(last.localZOrder);

    //移动回原位
    auto moveBack = cocos2d::MoveTo::create(0.2f, last.fromPos);
    last.card->runAction(moveBack);

    //状态恢复
    if (last.isFromDeck) {
        last.card->setFaceUp(false);
        _deck.push_back(last.card);
    }
    else {
        _tableau.push_back(last.card);
    }
    //活跃牌指针回退
    _activeCard = last.lastActiveCard;
    //确保消除区的当前“活跃牌”在最顶层 (ZOrder 设高一点)
    if (_activeCard) {
        _activeCard->setLocalZOrder(2000);
    }
}
