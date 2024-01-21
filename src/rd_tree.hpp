#pragma once

struct rbnode {
    enum color : bool { red, black } color;
    rbnode *left = nullptr, *right = nullptr;
    void* item = nullptr;
};
