#pragma once

#include <unordered_set>
#include <unordered_map>

void saveToFile(std::string content) {
    static bool firstWrite = false;
    static std::string path = std::string("/storage/emulated/0/Android/data/") + GetPackageName() + "/files/HeroSkinData.txt";
    static std::ofstream file(path, std::ios::app);

    if (!firstWrite) {  
        std::ifstream checkFile("/storage/emulated/0/Android/data/com.garena.game.kgvn/files/HeroSkinData.txt");
        if (checkFile.peek() == std::ifstream::traits_type::eof()) { // Kiểm tra file rỗng
            file << "/*\n"
                 << "\tCredit\n"
                 << "\t• Telegram: @TuanMeta\n"
                 << "\t• Telegram Channel: https://t.me/ZRTChannel\n"
                 << "\t• Telegram Group: https://t.me/ZRTGroup\n"
                 << "*/\n";
        }
        firstWrite = true;
    }

    if (file.is_open()) {
        file << content;
    }
}

static String *(*GetHeroName)(uint32_t heroId); //Hero Name
static String *(*GetSkinName)(uint32_t skinUniId); //Hero Skin

uintptr_t dwID; //field configId, eg: 15000
uintptr_t dwHeroID; //field HeroId, eg: 150

std::unordered_map<std::string, uint32_t> heroNameToId; // Lưu heroName xuất hiện đầu tiên
std::unordered_map<uint32_t, std::vector<std::pair<uint32_t, std::string>>> heroSkins;
std::unordered_set<uint32_t> writtenHeroIds; // Kiểm tra hero đã ghi tiêu đề chưa

void saveHeroData(uintptr_t instance) {
    uint32_t configId = *(uint32_t *)(instance + dwID);
    uint32_t heroId = *(uint32_t *)(instance + dwHeroID);

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

    // Ghi danh sách skin
    int index = heroSkins[heroId].size(); // Đếm số skin hiện tại
    data << index << ". " << skinName << " : " << configId << "\n";

    // Ghi vào file
    saveToFile(data.str());
}

int (*_TransferData)(uintptr_t instance, void *trans);
int TransferData(uintptr_t instance, void *trans) {
	if (instance != NULL) {
		saveHeroData(instance);
	}
	return _TransferData(instance, trans);
}
