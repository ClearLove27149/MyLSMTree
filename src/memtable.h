//
// Created by 26685 on 2023/3/18.
//
#pragma once
#ifndef MYLSM_TREE_MEMTABLE_H
#define MYLSM_TREE_MEMTABLE_H

#include "skiplist.hpp"

namespace LSMtree {
    namespace Memory {
        template<typename K, typename V>
        class MemTable {
        public:
            MemTable() = delete;
            MemTable(int max_level, int nums_kv);
            ~MemTable();
            int size();

            bool insert_element(K& key, V& value);
            bool delete_element(K key);
            bool search_element(K& key, V& value);

            bool transfer_sequences();
            bool get_sequences(std::vector<KVPair<K,V>>& seq);
            bool is_active() {
                return active;
            }
        private:
            int _nums_kv;
            int _total_kv;
            bool active;
            bool transfer_finish;
            SkipList<K, V>* skiplist;
            std::vector<KVPair<K,V>> sequences;
        };

        template<typename K, typename V>
        MemTable<K, V>::MemTable(int max_level, int nums_kv) {
            skiplist = new SkipList<K, V>(max_level);
            active = true;
            transfer_finish = false;
            _nums_kv = nums_kv;
            _total_kv = 0;
        }
        template<typename K, typename V>
        MemTable<K, V>::~MemTable() {
            if (skiplist != NULL) {
                delete skiplist;
                skiplist = NULL;
            }
        }
        template<typename K, typename V>
        int MemTable<K, V>::size() {
            return skiplist->size();
        }
        template<typename K, typename V>
        bool MemTable<K, V>::insert_element(K &key, V &value) {
            std::cout << "total_kv : "<<_total_kv<<std::endl;
            if (!active) return false;

            if (skiplist->insert_element(key, value)) {
                _total_kv++;
                if (_total_kv >= _nums_kv) {
                    active = false;
                }
                return true;
            }
            return false;
        }
        template<typename K, typename V>
        bool MemTable<K, V>::delete_element(K key) {
            if (!active) return false;
            return skiplist->delete_element(key);
        }
        template<typename K, typename V>
        bool MemTable<K,V>::search_element(K &key, V &value) {
            if (!transfer_finish) {
                std::cout<<"Search from skiplist"<<std::endl;
                return skiplist->search_element(key, value);
            } else {
                // 从pairkv查
                std::cout << "Search from sequences"<<std::endl;
                int l=0, r= sequences.size();
                while (l<r) {
                    int mid = l + (r-l)/2;
                    KVPair<K, V> kvpair = sequences[mid];
                    if (kvpair.first == key) {
                        value = kvpair.second;
                        return true;
                    } else if (kvpair.first < key) {
                        l = mid + 1;
                    } else {
                        r = mid -1;
                    }
                }
                return false;
            }
        }
        template<typename K, typename V>
        bool MemTable<K, V>::transfer_sequences() {
            if (skiplist->get_elements(sequences)) {
                transfer_finish = true;
                delete skiplist;
                skiplist = NULL;
                return true;
            }
            return false;
        }
        template<typename K, typename V>
        bool MemTable<K, V>::get_sequences(std::vector<KVPair<K, V>> &seq) {
            if (transfer_finish) {
                seq = this->sequences;
                return true;
            }
            return false;
        }
    }
}
#endif //MYLSM_TREE_MEMTABLE_H
