#ifndef __CARD_RULE_SERVICE_H__
#define __CARD_RULE_SERVICE_H__

#include <cstdlib>

namespace chessgame {
namespace services {

// 无状态规则服务：判断两张牌是否可匹配。
class CardRuleService {
public:
    static bool canMatchByFace(int topFace, int candidateFace) {
        if (topFace < 0 || candidateFace < 0) {
            return false;
        }
        return std::abs(topFace - candidateFace) == 1;
    }
};

} // namespace services
} // namespace chessgame

#endif // __CARD_RULE_SERVICE_H__
