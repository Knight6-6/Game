#pragma once
#include "cocos2d.h"
#include <string>

class Card : public cocos2d::Sprite 
{
public:
    //静态创建方法：传入牌号，花色，颜色和层级
    static Card* createWithData(int number, const std::string& suit, const std::string& color,int localZOrder);
    void setFaceUp(bool up); // 设置正反面
    int getCardNumber() const { return _cardNumber; }
    std::string getSuit() const { return _suit; }
    bool isFaceUp() const { return _isFaceUp; }
private:
    // 内部初始化逻辑
    bool initWithData(int number, const std::string& suit, const std::string& color , int localZOrder);
	int _cardNumber;       // 牌号（1-13，分别对应A-K）
	bool _isFaceUp;        // 是否正面朝上
    std::string _suit;     //花色
    std::string _color;    //颜色
	int _localZOrder;      //层级
};