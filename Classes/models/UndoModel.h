#ifndef __UNDO_MODEL_H__
#define __UNDO_MODEL_H__

#include "cocos2d.h"

namespace chessgame {
namespace models {

// 一次可回退动作记录。
struct UndoModel {
    int movedCardId = -1;
    int previousTopCardId = -1;
    cocos2d::Vec2 movedCardFromPosition = cocos2d::Vec2::ZERO;
    cocos2d::Vec2 movedCardToPosition = cocos2d::Vec2::ZERO;
    bool movedCardWasInPlayfield = false;
    bool movedCardWasConsumed = false;
    bool previousTopWasConsumed = false;
};

} // namespace models
} // namespace chessgame

#endif // __UNDO_MODEL_H__
