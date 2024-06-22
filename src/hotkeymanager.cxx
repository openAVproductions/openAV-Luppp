#include "hotkeymanager.hxx"
#include <map>

std::map<int, std::pair<int, int>> keyToGrid = {
    {49, {0, 0}}, {50, {1, 0}}, {51, {2, 0}}, {52, {3, 0}}, {53, {4, 0}}, {54, {5, 0}}, {55, {6, 0}}, {56, {7, 0}}, // 1-8
    {113, {0, 1}}, {119, {1, 1}}, {101, {2, 1}}, {114, {3, 1}}, {116, {4, 1}}, {121, {5, 1}}, {117, {6, 1}}, {105, {7, 1}}, // q-i
    {97, {0, 2}}, {115, {1, 2}}, {100, {2, 2}}, {102, {3, 2}}, {103, {4, 2}}, {104, {5, 2}}, {106, {6, 2}}, {107, {7, 2}}, // a-k
    {122, {0, 3}}, {120, {1, 3}}, {99, {2, 3}}, {118, {3, 3}}, {98, {4, 3}}, {110, {5, 3}}, {109, {6, 3}}, {44, {7, 3}}, // z-,
};

void addKeyToGrid(int key, int id, int clipNumber) {
    keyToGrid[key] = {id, clipNumber};
}

int getCurrentKeyForClip(int id, int clipNumber) {
    for (const auto& pair : keyToGrid) {
        if (pair.second.first == id && pair.second.second == clipNumber) {
            return pair.first;
        }
    }
    return 0;
}