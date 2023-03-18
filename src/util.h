//
// Created by 26685 on 2023/3/18.
//

#ifndef MYLSM_TREE_UTIL_H
#define MYLSM_TREE_UTIL_H

#include <iostream>
#include <cstring>

template<typename T>
void deal_data(T t, uint8_t* &data, int& page_size) {
    uint8_t* u8 = new uint8_t[sizeof(t)];
    u8 = (uint8_t*)&t;
    memcpy_s(data, page_size, u8, sizeof(t));
    data += sizeof(t);
    page_size -= sizeof(t);
}
#endif //MYLSM_TREE_UTIL_H
