#ifndef __CARD_MODEL_H__
#define __CARD_MODEL_H__

#include "cocos2d.h"

namespace chessgame {
namespace models {

enum CardSuitType {
    CST_NONE = -1,
    CST_CLUBS,
    CST_DIAMONDS,
    CST_HEARTS,
    CST_SPADES,
    CST_NUM_CARD_SUIT_TYPES
};

enum CardFaceType {
    CFT_NONE = -1,
    CFT_ACE,
    CFT_TWO,
    CFT_THREE,
    CFT_FOUR,
    CFT_FIVE,
    CFT_SIX,
    CFT_SEVEN,
    CFT_EIGHT,
    CFT_NINE,
    CFT_TEN,
    CFT_JACK,
    CFT_QUEEN,
    CFT_KING,
    CFT_NUM_CARD_FACE_TYPES
};

// 运行时卡牌数据模型。
struct CardModel {
    int cardId = -1;
    int cardFace = CFT_NONE;
    int cardSuit = CST_NONE;
    cocos2d::Vec2 originalPosition = cocos2d::Vec2::ZERO;
    cocos2d::Vec2 currentPosition = cocos2d::Vec2::ZERO;
    bool inPlayfield = false;
    bool isTopCard = false;
    bool isConsumed = false;
};

} // namespace models
} // namespace chessgame

#endif // __CARD_MODEL_H__
