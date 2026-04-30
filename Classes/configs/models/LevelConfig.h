#ifndef __LEVEL_CONFIG_H__
#define __LEVEL_CONFIG_H__

#include "cocos2d.h"
#include <string>
#include <vector>

namespace chessgame {
namespace configs {
namespace models {

// 关卡中单张卡牌的静态配置。
struct LevelCardConfig {
    int cardFace = 0;
    int cardSuit = 0;
    cocos2d::Vec2 position = cocos2d::Vec2::ZERO;
};

// 关卡静态配置：桌面牌与备用牌堆。
struct LevelConfig {
    std::vector<LevelCardConfig> playfield;
    std::vector<LevelCardConfig> stack;
};

} // namespace models
} // namespace configs
} // namespace chessgame

#endif // __LEVEL_CONFIG_H__
