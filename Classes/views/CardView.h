#ifndef __CARD_VIEW_H__
#define __CARD_VIEW_H__

#include "2d/CCDrawNode.h"
#include "2d/CCLabel.h"
#include "2d/CCLayer.h"
#include "2d/CCSprite.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCDirector.h"
#include "models/CardModel.h"
#include <functional>
#include <new>
#include <string>

namespace chessgame {
namespace views {

// 单张卡牌视图：负责卡牌展示与点击输入。
class CardView : public cocos2d::Node {
public:
    static CardView* create(const chessgame::models::CardModel& model) {
        CardView* ret = new (std::nothrow) CardView();
        if (ret && ret->initWithModel(model)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool initWithModel(const chessgame::models::CardModel& model) {
        if (!Node::init()) {
            return false;
        }

        _cardId = model.cardId;
        _cardSize = cocos2d::Size(160.0f, 220.0f);
        setContentSize(_cardSize);
        setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));

        auto cardBg = cocos2d::Sprite::create("res/card_general.png");
        if (cardBg) {
            const float sx = _cardSize.width / cardBg->getContentSize().width;
            const float sy = _cardSize.height / cardBg->getContentSize().height;
            cardBg->setScaleX(sx);
            cardBg->setScaleY(sy);
            cardBg->setPosition(_cardSize.width * 0.5f, _cardSize.height * 0.5f);
            addChild(cardBg, 0);
        } else {
            auto fallback = cocos2d::LayerColor::create(cocos2d::Color4B(250, 250, 250, 255), _cardSize.width, _cardSize.height);
            addChild(fallback, 0);
            auto border = cocos2d::DrawNode::create();
            cocos2d::Vec2 vertices[4] = {
                cocos2d::Vec2(0, 0),
                cocos2d::Vec2(_cardSize.width, 0),
                cocos2d::Vec2(_cardSize.width, _cardSize.height),
                cocos2d::Vec2(0, _cardSize.height)
            };
            border->drawPoly(vertices, 4, true, cocos2d::Color4F(0.15f, 0.15f, 0.15f, 1.0f));
            addChild(border, 1);
        }

        const std::string faceText = toFaceText(model.cardFace);
        const std::string suitText = toSuitText(model.cardSuit);
        const bool isRed = isRedSuit(model.cardSuit);
        const cocos2d::Color4B textColor = isRed ? cocos2d::Color4B(190, 36, 36, 255) : cocos2d::Color4B(24, 24, 24, 255);

        auto leftTop = cocos2d::Label::createWithTTF(faceText, "fonts/arial.ttf", 34);
        leftTop->setAnchorPoint(cocos2d::Vec2(0.0f, 1.0f));
        leftTop->setColor(cocos2d::Color3B(textColor.r, textColor.g, textColor.b));
        leftTop->setPosition(16.0f, _cardSize.height - 16.0f);
        addChild(leftTop, 2);

        auto centerSuit = cocos2d::Label::createWithTTF(suitText, "fonts/arial.ttf", 56);
        centerSuit->setColor(cocos2d::Color3B(textColor.r, textColor.g, textColor.b));
        centerSuit->setPosition(_cardSize.width * 0.5f, _cardSize.height * 0.53f);
        addChild(centerSuit, 2);

        auto rightBottom = cocos2d::Label::createWithTTF(faceText, "fonts/arial.ttf", 34);
        rightBottom->setAnchorPoint(cocos2d::Vec2(1.0f, 0.0f));
        rightBottom->setColor(cocos2d::Color3B(textColor.r, textColor.g, textColor.b));
        rightBottom->setPosition(_cardSize.width - 16.0f, 16.0f);
        addChild(rightBottom, 2);

        initTouch();
        return true;
    }

    int getCardId() const {
        return _cardId;
    }

    void setOnClick(const std::function<void(int)>& callback) {
        _onClick = callback;
    }

    void setCardEnabled(bool enabled) {
        _enabled = enabled;
        setOpacity(enabled ? 255 : 145);
    }

    void setTopVisual(bool isTop) {
        setScale(isTop ? 1.06f : 1.0f);
    }

private:
    void initTouch() {
        auto listener = cocos2d::EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);

        listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event*) {
            if (!_enabled || !isVisible()) {
                return false;
            }
            const cocos2d::Vec2 local = convertToNodeSpace(touch->getLocation());
            const cocos2d::Rect rect(0.0f, 0.0f, _cardSize.width, _cardSize.height);
            return rect.containsPoint(local);
        };

        listener->onTouchEnded = [this](cocos2d::Touch*, cocos2d::Event*) {
            if (_enabled && _onClick) {
                _onClick(_cardId);
            }
        };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }

    static bool isRedSuit(int suit) {
        return suit == chessgame::models::CST_DIAMONDS || suit == chessgame::models::CST_HEARTS;
    }

    static std::string toSuitText(int suit) {
        switch (suit) {
        case chessgame::models::CST_CLUBS:
            return "C";
        case chessgame::models::CST_DIAMONDS:
            return "D";
        case chessgame::models::CST_HEARTS:
            return "H";
        case chessgame::models::CST_SPADES:
            return "S";
        default:
            return "?";
        }
    }

    static std::string toFaceText(int face) {
        switch (face) {
        case chessgame::models::CFT_ACE:
            return "A";
        case chessgame::models::CFT_TWO:
            return "2";
        case chessgame::models::CFT_THREE:
            return "3";
        case chessgame::models::CFT_FOUR:
            return "4";
        case chessgame::models::CFT_FIVE:
            return "5";
        case chessgame::models::CFT_SIX:
            return "6";
        case chessgame::models::CFT_SEVEN:
            return "7";
        case chessgame::models::CFT_EIGHT:
            return "8";
        case chessgame::models::CFT_NINE:
            return "9";
        case chessgame::models::CFT_TEN:
            return "10";
        case chessgame::models::CFT_JACK:
            return "J";
        case chessgame::models::CFT_QUEEN:
            return "Q";
        case chessgame::models::CFT_KING:
            return "K";
        default:
            return "?";
        }
    }

private:
    int _cardId = -1;
    bool _enabled = true;
    cocos2d::Size _cardSize;
    std::function<void(int)> _onClick;
};

} // namespace views
} // namespace chessgame

#endif // __CARD_VIEW_H__
