#pragma once

#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <string>
#include <sstream>
#include "Includes/Utils.h"

void saveToFile(std::string content, const std::string& fileName) {
    static std::unordered_map<std::string, std::ofstream> files;
    
    if (files.find(fileName) == files.end()) {
        std::string packageName(GetPackageName());
        std::string filePath = "/storage/emulated/0/Android/data/" + packageName + "/files/" + fileName;
        files[fileName].open(filePath, std::ios::app);
        std::ifstream checkFile(filePath);
        if (checkFile.peek() == std::ifstream::traits_type::eof()) {
            files[fileName] << "/*\n"
                            << "\tCredit\n"
                            << "\t• Telegram: @MEOW\n"
                            << "\t• Telegram Channel: https://t.me/MEOW\n"
                            << "\t• Telegram Group: https://t.me/MEOW\n"
                            << "*/\n";
        }
    }

    if (files[fileName].is_open()) {
        files[fileName] << content;
    }
}

static String *(*GetHeroName)(uint32_t heroId);
static String *(*GetSkinName)(uint32_t skinUniId);
static String *(*GetButtonName)(uint32_t buttonId);
static String *(*GetKillEffectName)(uint32_t effectId);

uintptr_t dwID_HeroSkin; 
uintptr_t dwHeroID_HeroSkin; 
std::unordered_map<std::string, uint32_t> heroNameToId;
std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, std::string>>> heroSkins;
std::unordered_set<uint32_t> writtenHeroIds;

void saveHeroData(uintptr_t instance) {
    uint32_t configId = *(uint32_t *)(instance + dwID_HeroSkin);
    uint32_t heroId = *(uint32_t *)(instance + dwHeroID_HeroSkin);

    std::string heroName = GetHeroName(heroId)->CString();
    if (heroName.empty() || heroName.find("[ex]") != std::string::npos) return;
    auto it = heroNameToId.find(heroName);
    if (it != heroNameToId.end() && it->second != heroId) return;
    heroNameToId[heroName] = heroId;

    std::string skinName = GetSkinName(configId)->CString();
    if (skinName.empty()) return;
    if (skinName.find("[ex]") != std::string::npos) {
        skinName = "Skin ID Trống";
    }

    heroSkins[heroId].emplace_back(configId, skinName);

    std::stringstream data;
    if (writtenHeroIds.insert(heroId).second) {
        data << "\n" << heroName << " [" << heroId << "]:\n";
    }
    int index = heroSkins[heroId].size();
    data << index << ". " << skinName << " : " << configId << "\n";
    saveToFile(data.str(), "HeroSkinData.txt");
}

int (*_TransferData_HeroSkin)(uintptr_t instance, void *trans);
int TransferData_HeroSkin(uintptr_t instance, void *trans) {
	if (instance != NULL) {
		saveHeroData(instance);
	}
	return _TransferData_HeroSkin(instance, trans);
}


uintptr_t dwButtonID;
std::unordered_set<uint32_t> writtenButtonIDs;

void saveButtonData(uintptr_t instance) {
    uint32_t buttonId = *(uint32_t *)(instance + dwButtonID);
    if (writtenButtonIDs.find(buttonId) != writtenButtonIDs.end()) return;
    std::stringstream data;
    data << "Button ID: " << buttonId << "\n";
    saveToFile(data.str(), "ButtonData.txt");
    writtenButtonIDs.insert(buttonId);
}

int (*_TransferData_Button)(uintptr_t instance, void *trans);
int TransferData_Button(uintptr_t instance, void *trans) {
    if (instance != NULL) {
        saveButtonData(instance);
    }
    return _TransferData_Button(instance, trans);
}

uintptr_t dwKillEffectID;
std::unordered_set<uint32_t> writtenKillEffectIDs;

void saveKillEffectData(uintptr_t instance) {
    uint32_t effectId = *(uint32_t *)(instance + dwKillEffectID);
    if (writtenKillEffectIDs.find(effectId) != writtenKillEffectIDs.end()) return;
    std::stringstream data;
    data << "Kill Effect ID: " << effectId << "\n";
    saveToFile(data.str(), "KillEffectData.txt");
    writtenKillEffectIDs.insert(effectId);
}

int (*_TransferData_KillEffect)(uintptr_t instance, void *trans);
int TransferData_KillEffect(uintptr_t instance, void *trans) {
    if (instance != NULL) {
        saveKillEffectData(instance);
    }
    return _TransferData_KillEffect(instance, trans);
}
