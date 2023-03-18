//
// Created by 26685 on 2023/3/18.
//
#pragma once
#ifndef MYLSM_TREE_BLOOMFILTER_H
#define MYLSM_TREE_BLOOMFILTER_H

#include <iostream>
#include <bitset>
#include <functional>
#include <vector>

#include "hash/murmurhash.h"

namespace LSMTree {
    namespace Util {
#define BITSET_SIZE 100

        typedef std::string K;
        class BloomFilter {
            // k为哈系函数个数，m为布隆过滤器长度，n为插入元素个数，p为误报率
        public:
            BloomFilter(std::string type, int k) {
                if (type == "murmurhash") {
                    hash_func = murmurhash;
                } else {
                    //
                }
                bf.reset();
                _k = k;
                // todo 随机化k个seed
                seeds = {0x12da77c8, 0x6357e0a6, 0xe5ce223e};
            }
            BloomFilter(const BloomFilter& other) {
                hash_func = other.hash_func;
                seeds = other.seeds;
                _k = other._k;
                bf = other.bf;
            }
            BloomFilter& operator= (const BloomFilter& other) {
                hash_func = other.hash_func;
                seeds = other.seeds;
                _k = other._k;
                bf = other.bf;
                return *this;
            }
            ~BloomFilter() { };
            bool isExist(K key) {
                for (int i=0;i<_k;i++) {
                    uint32_t res = hash_func(key.c_str(), key.length(), seeds[i]);
                    std::cout<<"res: "<<std::bitset<sizeof(res)*8>(res) <<std::endl;
                    if (bf[res % BITSET_SIZE] == 1) {
                        return true;
                    }
                }
                return false;
            }
            bool addKey(K key) {
                for (int i=0;i<_k;i++) {
                    uint32_t res = hash_func(key.c_str(), key.length(), seeds[i]);
                    std::cout<<"res: "<<std::bitset<sizeof(res)*8>(res) <<std::endl;
                    bf.set(res % BITSET_SIZE);
                }
                return true;
            }
            bool delKey(K key) {
                return true;
            }
            std::bitset<BITSET_SIZE+1> toBinary() {
                return bf;
            }
        private:
            int _k;
            std::vector<uint32_t> seeds;
            std::bitset<BITSET_SIZE+1> bf;
            std::function<uint32_t(const char*, uint32_t, uint32_t)> hash_func;

        };


        void test_bloomfilter() {
            BloomFilter bloomFilter(std::string("murmurhash"), 3);
            bloomFilter.addKey(std::string("libin"));
            bloomFilter.addKey(std::string("lipin"));

            std::cout<<bloomFilter.isExist(std::string("libin")) <<std::endl;
            std::cout<<bloomFilter.isExist(std::string("lipin")) <<std::endl;
            std::cout<<bloomFilter.isExist(std::string("hexiaolin")) <<std::endl;

            std::cout<<bloomFilter.toBinary()<<std::endl;
        }
    }
}

#endif //MYLSM_TREE_BLOOMFILTER_H
