//
// Created by 26685 on 2023/3/18.
//
#pragma once
#ifndef MYLSM_TREE_SKIPLIST_HPP
#define MYLSM_TREE_SKIPLIST_HPP

#include <iostream>
#include <cmath>
#include <mutex>
#include <thread>
#include <cstring>
#include <vector>

namespace LSMtree {
    namespace Memory {
        template<typename K, typename V>
        using KVPair = std::pair<K, V>;

        std::mutex mtx;
        template<typename K, typename V>
        class SkipListNode {
        public:
            SkipListNode() { };
            SkipListNode(K k, V v, int level) : key(k), value(v),node_level(level) {
                forward = new SkipListNode<K, V> * [level+1];
                memset(forward, 0, sizeof(SkipListNode<K,V>*) * (level+1));
            }
            ~SkipListNode() {
                delete []forward;
            }
            K get_key() const {
                return key;
            }
            V get_Value() const {
                return value;
            }
            int get_node_level() const {
                return node_level;
            }
            SkipListNode<K, V> **forward;
            int node_level;
        private:
            K key;
            V value;
        };
        //
        template<typename K, typename V>
        class SkipList {
        public:
            SkipList() = delete;
            SkipList(int max_level) : _max_level(max_level), _element_count(0), _skip_list_level(0) {
                K k;
                V v;
                this->_header = new SkipListNode<K, V>(k, v, _max_level);
            }
            ~SkipList() {
                delete _header;
            }
            int size() {
                return _element_count;
            }
            int get_random_level() {
                int k=1;
                while (rand() % 2) {
                    k++;
                }
                k = (k < _max_level) ? k : _max_level;
                return k;
            }
            SkipListNode<K, V>* create_node(K key, V value, int level) {
                SkipListNode<K, V> *node = new SkipListNode<K, V>(key, value, level);
                return node;
            }

            bool insert_element(const K key, const V value) {
                mtx.lock();
                SkipListNode<K, V> *cur = _header;
                SkipListNode<K, V>* update[_max_level+1];
                // 1.从跳表最大level处搜索
                memset(update, 0, sizeof(SkipListNode<K, V>*) *(_max_level+1));
                for (int i = _max_level; i >= 0; --i) {
                    while (cur->forward[i] != NULL && cur->forward[i]->get_key() < key) {
                        cur = cur->forward[i];
                    }
                    update[i] = cur;
                }
                // 2.到达level 0
                cur = cur->forward[0];
                if (cur != NULL && cur->get_key() == key) {
                    std::cout << "Failly insert key: " << key <<" exist"<<std::endl;
                    mtx.unlock();
                    return false;
                }
                //3.找到插入位置，要么是结尾，要么是中间
                if (cur == NULL || cur->get_key() > key) {
                    // 随机化level
                    int level = get_random_level();
                    if (level > _skip_list_level) {
                        for (int i = _skip_list_level; i < level+1; ++i) {
                            update[i] = _header;
                        }
                        _skip_list_level = level;
                    }
                    SkipListNode<K, V>* node = new SkipListNode<K, V>(key, value, level);
                    for (int i=0;i<=level;i++) {
                        node->forward[i] = update[i]->forward[i];
                        update[i]->forward[i] = node;
                    }
                    std::cout << "Successfully insert key: "<<key<<", value: "<<value<<std::endl;
                    _element_count++;
                }
                mtx.unlock();
                return true;
            }
            // delete
            bool delete_element(K key) {
                mtx.lock();
                SkipListNode<K, V> *cur = _header;
                SkipListNode<K, V>* update[_max_level+1];
                // 1.从跳表最大level处搜索
                memset(update, 0, sizeof(SkipListNode<K, V>*) *(_max_level+1));
                for (int i = _max_level; i >= 0; --i) {
                    while (cur->forward[i] != NULL && cur->forward[i]->get_key() < key) {
                        cur = cur->forward[i];
                    }
                    update[i] = cur;
                }
                // 2.到达level 0
                cur = cur->forward[0];
                if (cur != NULL && cur->get_key() == key) {
                    // find
                    // 从最底层向上删除当前节点，即调整forward
                    for (int i=0;i<=cur->get_node_level();i++) {
                        update[i]->forward[i] = cur->forward[i];
                    }
                    while (_skip_list_level > 0 && _header->forward[_skip_list_level] == 0) {
                        _skip_list_level--;
                    }
                    std::cout << "Successfully deleted key: " << key <<std::endl;
                    mtx.unlock();
                    return true;
                }
                std::cout <<"Failly deleted key: "<<key<<std::endl;
                mtx.unlock();
                return false;
            }
            // search
            bool search_element(K key, V& value) {
                mtx.lock();
                SkipListNode<K, V> *cur = _header;
                SkipListNode<K, V>* update[_max_level+1];
                // 1.从跳表最大level处搜索
                memset(update, 0, sizeof(SkipListNode<K, V>*) *(_max_level+1));
                for (int i = _max_level; i >= 0; --i) {
                    while (cur->forward[i] != NULL && cur->forward[i]->get_key() < key) {
                        cur = cur->forward[i];
                    }
                    update[i] = cur;
                }
                // 2.到达level 0
                cur = cur->forward[0];
                if (cur != NULL && cur->get_key() == key) {
                    value = cur->get_Value();
                    std::cout << "Successfully search key: "<<key<<", value: "<<value<<std::endl;
                    mtx.unlock();
                    return true;
                } else {
                    std::cout << "Failly search key: "<<key<<std::endl;
                    mtx.unlock();
                    return false;
                }
            }
            //print
            void print_skiplist() {
                std::cout << "\n********Skip List*********"<<"\n";
                for (int i=0;i<=_skip_list_level;++i) {
                    SkipListNode<K, V>* node = _header->forward[i];
                    std::cout << "Level "<<i<<": ";
                    while (node != NULL) {
                        std::cout <<node->get_key()<<":"<<node->get_Value()<<";";
                        node = node->forward[i];
                    }
                    std::cout << std::endl;
                }
            }
            bool get_elements(std::vector<KVPair<K,V>>& sequences) {
                SkipListNode<K,V>* node = _header->forward[0];
                while (node != NULL) {
                    auto key = node->get_key();
                    auto value = node->get_Value();
                    sequences.push_back(std::make_pair(key, value));
                    node = node->forward[0];
                }
                if (_element_count == sequences.size()) {
                    return true;
                } else {
                    sequences.clear();
                    return false;
                }
            }

        private:
            int _max_level; //允许的最大level
            int _skip_list_level; // 当前跳表的level
            int _element_count;
            SkipListNode<K, V>* _header;
        };
    }
}

#endif //MYLSM_TREE_SKIPLIST_HPP
