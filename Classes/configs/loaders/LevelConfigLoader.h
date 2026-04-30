#ifndef __LEVEL_CONFIG_LOADER_H__
#define __LEVEL_CONFIG_LOADER_H__

#include "configs/models/LevelConfig.h"
#include "platform/CCFileUtils.h"
#include "json/document.h"

namespace chessgame {
namespace configs {
namespace loaders {

// 关卡配置加载器：从 JSON 文件读取静态配置。
class LevelConfigLoader {
public:
    static bool loadFromJsonFile(const std::string& filePath, models::LevelConfig& outConfig) {
        outConfig.playfield.clear();
        outConfig.stack.clear();

        const std::string jsonText = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);
        if (jsonText.empty()) {
            CCLOG("[LevelConfigLoader] failed to read file: %s", filePath.c_str());
            return false;
        }

        rapidjson::Document doc;
        doc.Parse(jsonText.c_str());
        if (doc.HasParseError() || !doc.IsObject()) {
            CCLOG("[LevelConfigLoader] json parse error: %s", filePath.c_str());
            return false;
        }

        parseCardArray(doc, "Playfield", outConfig.playfield);
        parseCardArray(doc, "Stack", outConfig.stack);
        return true;
    }

private:
    static void parseCardArray(const rapidjson::Document& doc,
                               const char* key,
                               std::vector<models::LevelCardConfig>& outCards) {
        if (!doc.HasMember(key) || !doc[key].IsArray()) {
            return;
        }

        const rapidjson::Value& arr = doc[key];
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
            const rapidjson::Value& item = arr[i];
            if (!item.IsObject()) {
                continue;
            }

            models::LevelCardConfig cfg;
            cfg.cardFace = item.HasMember("CardFace") && item["CardFace"].IsInt() ? item["CardFace"].GetInt() : 0;
            cfg.cardSuit = item.HasMember("CardSuit") && item["CardSuit"].IsInt() ? item["CardSuit"].GetInt() : 0;

            if (item.HasMember("Position") && item["Position"].IsObject()) {
                const rapidjson::Value& pos = item["Position"];
                const float x = pos.HasMember("x") && pos["x"].IsNumber() ? static_cast<float>(pos["x"].GetDouble()) : 0.0f;
                const float y = pos.HasMember("y") && pos["y"].IsNumber() ? static_cast<float>(pos["y"].GetDouble()) : 0.0f;
                cfg.position = cocos2d::Vec2(x, y);
            }
            outCards.push_back(cfg);
        }
    }
};

} // namespace loaders
} // namespace configs
} // namespace chessgame

#endif // __LEVEL_CONFIG_LOADER_H__
