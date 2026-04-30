#ifndef __GAME_VIEW_H__
#define __GAME_VIEW_H__

#include "2d/CCActionInstant.h"
#include "2d/CCActionInterval.h"
#include "2d/CCDrawNode.h"
#include "2d/CCLabel.h"
#include "2d/CCMenu.h"
#include "2d/CCMenuItem.h"
#include "2d/CCScene.h"
#include "base/CCDirector.h"
#include "models/GameModel.h"
#include "views/CardView.h"
#include <functional>
#include <unordered_map>

namespace chessgame {
namespace views {

// 游戏主视图：负责区域展示、卡牌创建与动画播放。
class GameView : public cocos2d::Node {
public:
    CREATE_FUNC(GameView);

    bool init() override {
        if (!Node::init()) {
            return false;
        }

        const cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
        const cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
        const float width = visibleSize.width;

        _topCardPosition = cocos2d::Vec2(origin.x + width * 0.7f, origin.y + 220.0f);

        auto playfieldBg = cocos2d::DrawNode::create();
        cocos2d::Vec2 playRect[4] = {
            cocos2d::Vec2(origin.x, origin.y + 580.0f),
            cocos2d::Vec2(origin.x + width, origin.y + 580.0f),
            cocos2d::Vec2(origin.x + width, origin.y + 2080.0f),
            cocos2d::Vec2(origin.x, origin.y + 2080.0f)
        };
        playfieldBg->drawSolidPoly(playRect, 4, cocos2d::Color4F(0.10f, 0.45f, 0.21f, 0.35f));
        playfieldBg->drawPoly(playRect, 4, true, cocos2d::Color4F(0.90f, 0.95f, 0.90f, 0.9f));
        addChild(playfieldBg, 0);

        auto stackBg = cocos2d::DrawNode::create();
        cocos2d::Vec2 stackRect[4] = {
            cocos2d::Vec2(origin.x, origin.y),
            cocos2d::Vec2(origin.x + width, origin.y),
            cocos2d::Vec2(origin.x + width, origin.y + 580.0f),
            cocos2d::Vec2(origin.x, origin.y + 580.0f)
        };
        stackBg->drawSolidPoly(stackRect, 4, cocos2d::Color4F(0.11f, 0.28f, 0.54f, 0.35f));
        stackBg->drawPoly(stackRect, 4, true, cocos2d::Color4F(0.88f, 0.92f, 0.98f, 0.9f));
        addChild(stackBg, 0);

        auto title = cocos2d::Label::createWithTTF("ChessGame - Card Match", "fonts/arial.ttf", 36);
        title->setPosition(origin.x + width * 0.5f, origin.y + 2020.0f);
        addChild(title, 1);

        auto tip = cocos2d::Label::createWithTTF("Tap hand card to replace top; playfield card must be +/-1; Undo supported", "fonts/arial.ttf", 22);
        tip->setPosition(origin.x + width * 0.5f, origin.y + 1940.0f);
        addChild(tip, 1);

        auto topSlot = cocos2d::DrawNode::create();
        const float slotW = 170.0f;
        const float slotH = 230.0f;
        cocos2d::Vec2 slotRect[4] = {
            cocos2d::Vec2(_topCardPosition.x - slotW * 0.5f, _topCardPosition.y - slotH * 0.5f),
            cocos2d::Vec2(_topCardPosition.x + slotW * 0.5f, _topCardPosition.y - slotH * 0.5f),
            cocos2d::Vec2(_topCardPosition.x + slotW * 0.5f, _topCardPosition.y + slotH * 0.5f),
            cocos2d::Vec2(_topCardPosition.x - slotW * 0.5f, _topCardPosition.y + slotH * 0.5f)
        };
        topSlot->drawPoly(slotRect, 4, true, cocos2d::Color4F(0.96f, 0.94f, 0.35f, 1.0f));
        addChild(topSlot, 1);

        auto topText = cocos2d::Label::createWithTTF("Top Card", "fonts/arial.ttf", 24);
        topText->setPosition(_topCardPosition.x, _topCardPosition.y + 140.0f);
        addChild(topText, 1);

        auto undoItem = cocos2d::MenuItemLabel::create(
            cocos2d::Label::createWithTTF("Undo", "fonts/arial.ttf", 34),
            [this](cocos2d::Ref*) {
                if (_onUndoClick) {
                    _onUndoClick();
                }
            }
        );
        _undoMenu = cocos2d::Menu::create(undoItem, nullptr);
        _undoMenu->setPosition(origin.x + width - 120.0f, origin.y + 120.0f);
        addChild(_undoMenu, 2);

        _cardsRoot = cocos2d::Node::create();
        addChild(_cardsRoot, 5);
        return true;
    }

    void bindCallbacks(const std::function<void(int)>& onCardClick,
                       const std::function<void()>& onUndoClick) {
        _onCardClick = onCardClick;
        _onUndoClick = onUndoClick;
    }

    void buildCards(const chessgame::models::GameModel& gameModel) {
        _cardsRoot->removeAllChildren();
        _cardViews.clear();

        for (const auto& pair : gameModel.cards) {
            const chessgame::models::CardModel& card = pair.second;
            auto* cardView = CardView::create(card);
            if (!cardView) {
                continue;
            }

            cardView->setPosition(card.currentPosition);
            cardView->setOnClick([this](int cardId) {
                if (_onCardClick) {
                    _onCardClick(cardId);
                }
            });

            _cardsRoot->addChild(cardView, card.isTopCard ? 100 : 10);
            _cardViews[card.cardId] = cardView;
        }

        syncCardStates(gameModel);
    }

    void syncCardStates(const chessgame::models::GameModel& gameModel) {
        for (const auto& pair : gameModel.cards) {
            const chessgame::models::CardModel& card = pair.second;
            auto found = _cardViews.find(card.cardId);
            if (found == _cardViews.end()) {
                continue;
            }

            CardView* cardView = found->second;
            cardView->setPosition(card.currentPosition);
            cardView->setTopVisual(card.isTopCard);
            cardView->setLocalZOrder(card.isTopCard ? 120 : (card.inPlayfield ? 20 : 10));

            const bool clickable = !card.isConsumed && !card.isTopCard;
            cardView->setCardEnabled(clickable);
            cardView->setVisible(!card.isConsumed || card.isTopCard);
        }
    }

    void animateMoveCard(int cardId,
                         const cocos2d::Vec2& target,
                         const std::function<void()>& onDone = nullptr,
                         float duration = 0.22f) {
        auto found = _cardViews.find(cardId);
        if (found == _cardViews.end()) {
            if (onDone) {
                onDone();
            }
            return;
        }

        CardView* cardView = found->second;
        cardView->stopAllActions();
        auto move = cocos2d::MoveTo::create(duration, target);
        auto done = cocos2d::CallFunc::create([onDone]() {
            if (onDone) {
                onDone();
            }
        });
        cardView->runAction(cocos2d::Sequence::create(move, done, nullptr));
    }

    cocos2d::Vec2 getTopCardPosition() const {
        return _topCardPosition;
    }

private:
    cocos2d::Node* _cardsRoot = nullptr;
    cocos2d::Menu* _undoMenu = nullptr;
    cocos2d::Vec2 _topCardPosition = cocos2d::Vec2::ZERO;
    std::unordered_map<int, CardView*> _cardViews;
    std::function<void(int)> _onCardClick;
    std::function<void()> _onUndoClick;
};

} // namespace views
} // namespace chessgame

#endif // __GAME_VIEW_H__
