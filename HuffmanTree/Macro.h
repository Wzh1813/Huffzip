#pragma once

#include "../Bintree/BinTree.h"
#include "HuffChar.h"
#include <list>

#define HuffTree BinTree<HuffChar> //哈夫曼树
typedef std::list<HuffTree*> HuffForest; //哈夫曼森林
