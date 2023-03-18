//
// Created by 26685 on 2023/3/18.
//
#pragma once
#ifndef MYLSM_TREE_LSM_H
#define MYLSM_TREE_LSM_H

#include <queue>
#include "memtable.h"
#include "sstable.h"


namespace LSMtree {
    using namespace Memory;
    using namespace Disk;


#define DEFAULT_MAX_LEVEL 32
    template<typename K, typename V>
    class LSM {
    private:
        std::vector<MemTable<K,V>*> immemtables; //active队尾
        int level_index[DEFAULT_MAX_LEVEL] = {0,}; // 保存每层已经有几个sstable
        int _page_size;
        int _nums_kv;
        int _max_level;
        int _max_mtable;
    public:
        LSM() = delete;
        LSM(int page_size, int nums_kv, int max_level, int max_mtable) :
        _page_size(page_size), _nums_kv(nums_kv), _max_level(max_level), _max_mtable(max_mtable) {
            MemTable<K, V>* memtable = new MemTable<K, V>(_max_level, _nums_kv);
            this->immemtables.push_back(memtable);
        }
        ~LSM() { };
        int get_memtables_nums() {
            return immemtables.size();
        }
        bool insert_element(K& key, V& value);
        bool query_element(K& key, V& value);
        bool delete_element(K& key);
        void compact() {
            // todo
        }
    };
    template<typename K, typename V>
    bool LSM<K, V>::insert_element(K &key, V &value) {
        // 取队尾，是否是active
        if (this->immemtables.back()->is_active()) {
            auto mt = this->immemtables.back();
            return mt->insert_element(key, value);
        }
        // not active, mtable个数是否超过
        if (this->immemtables.size() < _max_mtable) {
            MemTable<K, V>* memtable = new MemTable<K, V>(_max_level, _nums_kv);
            this->immemtables.push_back(memtable);
            return this->insert_element(key, value);
        }
        //todo 超过后，mtable写入disk
        std::cout<<"need insert "<<key<<std::endl;
        int index_temp = level_index[0];
        SSTable<K, V> ssTable(0);
        for (auto item : immemtables) {
            item->transfer_sequences();
            ssTable.to_SSTable(*item);
            ssTable.Write(index_temp++);
        }
        immemtables.clear();
        level_index[0] = index_temp;
        MemTable<K, V>*memtable = new MemTable<K, V>(_max_level, _nums_kv);
        this->immemtables.push_back(memtable);
        return this->insert_element(key, value);
        // todo compact
        compact();

    }

    template<typename K, typename V>
    bool LSM<K, V>::delete_element(K &key) {
        if (this->immemtables.back()->is_active()) {
            return this->immemtables.back()->delete_element(key);
        } else {
            return false;
        }
    }

    template<typename K, typename V>
    bool LSM<K, V>::query_element(K &key, V &value) {
        for (int i=immemtables.size()-1;i>=0;--i) {
            MemTable<K, V>* mt = immemtables[i];
            std::cout<<"----------search from memtable----------"<<std::endl;
            if (mt->search_element(key, value)) {
                return true;
            }
        }
        // todo 从sstable查
        return false;
    }
}
#endif //MYLSM_TREE_LSM_H
