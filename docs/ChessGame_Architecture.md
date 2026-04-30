# ChessGame MVC 结构说明

## 1. 目录与职责
- `Classes/configs/models/LevelConfig.h`：关卡静态配置结构。
- `Classes/configs/loaders/LevelConfigLoader.h`：JSON 配置读取与解析。
- `Classes/models/CardModel.h`：卡牌运行时数据。
- `Classes/models/GameModel.h`：游戏运行时数据总模型。
- `Classes/models/UndoModel.h`：单条回退记录。
- `Classes/views/CardView.h`：单卡视图，负责渲染与点击。
- `Classes/views/GameView.h`：整体 UI 与卡牌动画。
- `Classes/services/CardRuleService.h`：无状态规则服务（点数差 1 匹配）。
- `Classes/managers/UndoManager.h`：回退记录栈。
- `Classes/controllers/GameController.h`：核心流程控制器。

## 2. 交互流程
1. `GameView` 接收卡牌点击，回调给 `GameController`。
2. `GameController` 调用 `CardRuleService` 判断是否合法。
3. 合法操作先写入 `UndoManager`，再更新 `GameModel`。
4. `GameController` 调用 `GameView::animateMoveCard` 播放 `MoveTo`。
5. 动画结束后 `GameView::syncCardStates` 与模型状态对齐。

## 3. 当前实现覆盖
- 手牌区点击替换顶部牌（MoveTo）。
- 桌面牌与顶部牌点数差 1 匹配后替换顶部牌（MoveTo）。
- 回退按钮按历史逆序反向平移恢复，直到无记录。

## 4. 如何新增一张卡牌
1. 在 `res/level_1.json` 的 `Playfield` 或 `Stack` 增加一条卡牌配置。
2. 设置 `CardFace`、`CardSuit`、`Position`。
3. 运行后 `GameController::buildModelFromLevel` 自动生成 `CardModel` 并由 `GameView` 渲染。

## 5. 如何新增一个回退类型
1. 扩展 `models/UndoModel.h`，新增该动作需要恢复的字段。
2. 在 `GameController` 触发动作前记录该字段。
3. 在 `GameController::onUndoClick` 里补充该动作的反向恢复逻辑。
4. 若有新动画，新增 `GameView` 对应播放接口。

## 6. 备注
- 当前采用头文件内联实现，避免修改 `proj.win32` 工程配置文件。
- 若后续允许调整工程配置，可把各模块拆分为 `.h + .cpp` 以进一步提升编译与维护效率。
