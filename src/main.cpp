#include <iostream>
#include "lsm.h"

using namespace LSMtree;
using namespace LSMtree::Memory;
using namespace LSMtree::Disk;

// todo 处理右值问题
void test_lsm() {
    int c=0;
    LSM<int, std::string> lsm(1024*1024, 3, 6, 3);
    for (int i=0;i<=20;i++) {
        std::string s = std::to_string(i);
        if (lsm.insert_element(i, s)) {
            c++;
        }
    }
}
int main() {
    test_lsm();
    return 0;
}
