#ifndef __UNDO_MANAGER_H__
#define __UNDO_MANAGER_H__

#include "models/UndoModel.h"
#include <vector>

namespace chessgame {
namespace managers {

// 回退记录管理器，按栈结构管理操作历史。
class UndoManager {
public:
    void clear() {
        _records.clear();
    }

    void push(const chessgame::models::UndoModel& record) {
        _records.push_back(record);
    }

    bool canUndo() const {
        return !_records.empty();
    }

    bool pop(chessgame::models::UndoModel& outRecord) {
        if (_records.empty()) {
            return false;
        }
        outRecord = _records.back();
        _records.pop_back();
        return true;
    }

private:
    std::vector<chessgame::models::UndoModel> _records;
};

} // namespace managers
} // namespace chessgame

#endif // __UNDO_MANAGER_H__
