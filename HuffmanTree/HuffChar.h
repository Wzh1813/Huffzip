#pragma once

class HuffChar {
public:
    char ch;int weight; //字符 权重
    HuffChar(char c = '^',int w = 0) : ch(c),weight(w) {}
};