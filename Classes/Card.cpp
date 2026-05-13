#include "Card.h"

Card* Card::createWithData(int number, const std::string& suit, const std::string& color, int localZOrder)
{
    Card* card = new (std::nothrow) Card();
    if (card && card->initWithData(number, suit, color, localZOrder))
    {
        card->autorelease();
        return card;
    }
    CC_SAFE_DELETE(card);
    return nullptr;
}

bool Card::initWithData(int number, const std::string& suit, const std::string& color, int localZOrder)
{
    _cardNumber = number;
    _suit = suit;
    _color = color;
    _isFaceUp = false;
    _localZOrder = localZOrder;
    //先创建底牌
    if (!Sprite::initWithFile("card_general.png")) 
    {
        return false;
    }
    // 获取底板的大小，方便后面算坐标
    auto s = this->getContentSize();
    std::string numFileName;
	if (number == 1) numFileName = "A";
	else if (number == 11) numFileName = "J";
	else if (number == 12) numFileName = "Q";
	else if (number == 13) numFileName = "K";
	else numFileName = std::to_string(number); // 转成字符
    //创建中间的大数字
    std::string bigName = "number/big_" + color + "_" + numFileName + ".png";
    auto bigNum = Sprite::create(bigName);
    if (bigNum) 
    {
        bigNum->setPosition(cocos2d::Vec2(s.width / 2, s.height / 2));
        this->addChild(bigNum);
    }
    //创建左上角的小数字
    std::string smallName = "number/small_" + color + "_" + numFileName + ".png";
    auto smallNum = Sprite::create(smallName);
    if (smallNum) 
    {
        smallNum->setAnchorPoint(cocos2d::Vec2(0, 1)); // 锚点设为左上
        smallNum->setPosition(cocos2d::Vec2(10, s.height - 10));
        this->addChild(smallNum);
    }
    //创建右上角的花色
    std::string suitName = "suits/"+suit + ".png";
    auto suitSprite = Sprite::create(suitName);
    if (suitSprite) 
    {
        suitSprite->setAnchorPoint(cocos2d::Vec2(1, 1)); // 锚点设为右上
        suitSprite->setPosition(cocos2d::Vec2(s.width - 10, s.height - 10));
        this->addChild(suitSprite);
    }
    return true;
}
void Card::setFaceUp(bool up) 
{
    _isFaceUp = up;
}