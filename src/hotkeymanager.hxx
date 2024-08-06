#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include <map>
#include <utility>

extern std::map<int, std::pair<int, int>> keyToGrid;

void addKeyToGrid(int key, int id, int clipNumber);
void updateKeyToGrid(int oldKey, int newkey, int id, int clipNumber);
void removeKeyFromGrid(int key, int id, int clipNumber);
int getCurrentKeyForClip(int id, int clipNumber);

#endif // HOTKEYMANAGER_H