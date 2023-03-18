//
// Created by 26685 on 2023/3/18.
//
#pragma once
#ifndef MYLSM_TREE_SSTABLE_H
#define MYLSM_TREE_SSTABLE_H

#include <stdint.h>
#include <iostream>
#include <fstream>
#include "memtable.h"
#include "util.h"

namespace LSMtree {
    namespace Disk {
        using namespace Memory;
#define SSTable_Size 1024*1024

        template<class K, class V>
        class SSTable {
        public:
            SSTable() = delete;
            SSTable(int level) {
                disk_level = level;
                data = NULL;
                end_index = 0;
            }

            void to_SSTable(MemTable<K, V>& memtable) {
                std::vector<KVPair<K,V>> seq;
                bool ok = memtable.get_sequences(seq);
                int page_size = SSTable_Size;
                if (seq.size() != 0) {
                    data = new uint8_t [SSTable_Size];
                    memset(data, 0, SSTable_Size);
                    std::cout << "------------start to sstable***********"<<std::endl;
                    for (KVPair<K,V> kv : seq) {
                        K key = kv.first;
                        uint8_t *k_u8;
                        deal_data(key, data, page_size);
                        V value = kv.second;
                        uint8_t *v_u8;
                        deal_data(value, data, page_size);
                    }
                }
                data_st = data - (SSTable_Size - page_size);
                end_index =(SSTable_Size - page_size);
            }
            ~SSTable() {
                if (data_st != NULL) {
                    delete data_st;
                    data_st = NULL;
                }
            }

            void Write(int _index) {
                std::string filename = "test-"+std::to_string(disk_level)+"-"+std::to_string(_index)+".bin";
                std::ofstream outf(filename, std::ios::out | std::ios::binary);
                outf.write(reinterpret_cast<const char*>(data_st), end_index);
                outf.close();
            }
            void Read(int _index) {
                //todo
            }

        private:
            uint8_t* data_st;
            uint8_t* data;
            int end_index;
            int disk_level;
        };
    }
}
#endif //MYLSM_TREE_SSTABLE_H
