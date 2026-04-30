#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__

#include "configs/loaders/LevelConfigLoader.h"
#include "managers/UndoManager.h"
#include "models/GameModel.h"
#include "services/CardRuleService.h"
#include "views/GameView.h"
#include "base/CCDirector.h"
#include <algorithm>

namespace chessgame {
namespace controllers {

// 游戏总控制器：协调模型、视图与业务规则。
class GameController {
public:
    bool init(cocos2d::Scene* ownerScene) {
        if (!ownerScene) {
            return false;
        }

        if (!buildModelFromLevel()) {
            return false;
        }

        _gameView = chessgame::views::GameView::create();
        if (!_gameView) {
            return false;
        }

        ownerScene->addChild(_gameView);
        _gameView->bindCallbacks(
            [this](int cardId) { onCardClick(cardId); },
            [this]() { onUndoClick(); }
        );

        _topCardPosition = _gameView->getTopCardPosition();
        applyTopCardToSlot();

        _gameView->buildCards(_gameModel);
        _gameView->syncCardStates(_gameModel);
        return true;
    }

private:
    bool buildModelFromLevel() {
        chessgame::configs::models::LevelConfig levelConfig;
        static const char* kLevelCandidates[] = {
            "level_1.json",
            "res/level_1.json",
            "../res/level_1.json",
            "../../res/level_1.json",
            "../../../res/level_1.json",
            "../../../../res/level_1.json"
        };

        bool loaded = false;
        for (const char* candidate : kLevelCandidates) {
            if (chessgame::configs::loaders::LevelConfigLoader::loadFromJsonFile(candidate, levelConfig)) {
                loaded = true;
                break;
            }
        }
        if (!loaded) {
            CCLOG("[GameController] level config not found");
            return false;
        }

        _gameModel.cards.clear();
        _gameModel.playfieldCardIds.clear();
        _gameModel.stackCardIds.clear();
        _gameModel.topCardId = -1;
        _undoManager.clear();

        int nextCardId = 1;

        for (const auto& cfg : levelConfig.playfield) {
            chessgame::models::CardModel card;
            card.cardId = nextCardId++;
            card.cardFace = cfg.cardFace;
            card.cardSuit = cfg.cardSuit;
            card.originalPosition = cfg.position;
            card.currentPosition = cfg.position;
            card.inPlayfield = true;
            card.isTopCard = false;
            card.isConsumed = false;
            _gameModel.cards[card.cardId] = card;
            _gameModel.playfieldCardIds.push_back(card.cardId);
        }

        const cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
        const cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
        const float stackStartX = origin.x + visibleSize.width * 0.20f;
        const float stackY = origin.y + 220.0f;

        for (size_t i = 0; i < levelConfig.stack.size(); ++i) {
            const auto& cfg = levelConfig.stack[i];
            chessgame::models::CardModel card;
            card.cardId = nextCardId++;
            card.cardFace = cfg.cardFace;
            card.cardSuit = cfg.cardSuit;

            // 使用JSON配置的位置，如果为(0,0)则动态计算
            cocos2d::Vec2 reservePos;
            if (cfg.position.x > 0 || cfg.position.y > 0) {
                reservePos = cfg.position;
            } else {
                reservePos = cocos2d::Vec2(stackStartX + static_cast<float>(i) * 170.0f, stackY);
            }
            card.originalPosition = reservePos;
            card.currentPosition = reservePos;
            card.inPlayfield = false;
            card.isTopCard = false;
            card.isConsumed = false;

            _gameModel.cards[card.cardId] = card;
            _gameModel.stackCardIds.push_back(card.cardId);
        }

        if (_gameModel.stackCardIds.empty()) {
            return false;
        }

        _gameModel.topCardId = _gameModel.stackCardIds.front();
        return true;
    }

    void applyTopCardToSlot() {
        for (auto& pair : _gameModel.cards) {
            pair.second.isTopCard = false;
        }

        chessgame::models::CardModel* topCard = _gameModel.findCard(_gameModel.topCardId);
        if (!topCard) {
            return;
        }

        topCard->isTopCard = true;
        topCard->isConsumed = false;
        topCard->inPlayfield = false;
        topCard->currentPosition = _topCardPosition;
    }

    void onCardClick(int cardId) {
        if (_isAnimating) {
            return;
        }

        chessgame::models::CardModel* card = _gameModel.findCard(cardId);
        chessgame::models::CardModel* topCard = _gameModel.findCard(_gameModel.topCardId);
        if (!card || !topCard || card->isTopCard || card->isConsumed) {
            return;
        }

        if (card->inPlayfield) {
            if (!chessgame::services::CardRuleService::canMatchByFace(topCard->cardFace, card->cardFace)) {
                return;
            }
        }

        chessgame::models::UndoModel undo;
        undo.movedCardId = card->cardId;
        undo.previousTopCardId = topCard->cardId;
        undo.movedCardFromPosition = card->currentPosition;
        undo.movedCardToPosition = _topCardPosition;
        undo.movedCardWasInPlayfield = card->inPlayfield;
        undo.movedCardWasConsumed = card->isConsumed;
        undo.previousTopWasConsumed = topCard->isConsumed;
        _undoManager.push(undo);

        topCard->isTopCard = false;
        topCard->isConsumed = true;

        card->inPlayfield = false;
        card->isConsumed = false;
        card->isTopCard = true;
        card->currentPosition = _topCardPosition;
        _gameModel.topCardId = card->cardId;

        _isAnimating = true;
        _gameView->animateMoveCard(card->cardId, _topCardPosition, [this]() {
            _isAnimating = false;
            _gameView->syncCardStates(_gameModel);
        });
        _gameView->syncCardStates(_gameModel);
    }

    void onUndoClick() {
        if (_isAnimating) {
            return;
        }

        chessgame::models::UndoModel undo;
        if (!_undoManager.pop(undo)) {
            return;
        }

        chessgame::models::CardModel* movedCard = _gameModel.findCard(undo.movedCardId);
        chessgame::models::CardModel* previousTop = _gameModel.findCard(undo.previousTopCardId);
        if (!movedCard || !previousTop) {
            return;
        }

        movedCard->isTopCard = false;
        movedCard->inPlayfield = undo.movedCardWasInPlayfield;
        movedCard->isConsumed = undo.movedCardWasConsumed;
        movedCard->currentPosition = undo.movedCardFromPosition;

        previousTop->isTopCard = true;
        previousTop->isConsumed = undo.previousTopWasConsumed;
        previousTop->inPlayfield = false;
        previousTop->currentPosition = _topCardPosition;

        _gameModel.topCardId = previousTop->cardId;

        _isAnimating = true;
        _gameView->animateMoveCard(movedCard->cardId, undo.movedCardFromPosition, [this]() {
            _isAnimating = false;
            _gameView->syncCardStates(_gameModel);
        });
        _gameView->syncCardStates(_gameModel);
    }

private:
    chessgame::models::GameModel _gameModel;
    chessgame::managers::UndoManager _undoManager;
    chessgame::views::GameView* _gameView = nullptr;
    cocos2d::Vec2 _topCardPosition = cocos2d::Vec2::ZERO;
    bool _isAnimating = false;
};

} // namespace controllers
} // namespace chessgame

#endif // __GAME_CONTROLLER_H__
