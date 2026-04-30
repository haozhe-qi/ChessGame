#ifndef __GAME_MODEL_H__
#define __GAME_MODEL_H__

#include "models/CardModel.h"
#include <unordered_map>
#include <vector>

namespace chessgame {
namespace models {

// 运行时游戏核心数据。
struct GameModel {
    std::unordered_map<int, CardModel> cards;
    std::vector<int> playfieldCardIds;
    std::vector<int> stackCardIds;
    int topCardId = -1;

    CardModel* findCard(int cardId) {
        auto it = cards.find(cardId);
        if (it == cards.end()) {
            return nullptr;
        }
        return &it->second;
    }

    const CardModel* findCard(int cardId) const {
        auto it = cards.find(cardId);
        if (it == cards.end()) {
            return nullptr;
        }
        return &it->second;
    }
};

} // namespace models
} // namespace chessgame

#endif // __GAME_MODEL_H__
