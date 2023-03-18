## MyLSMTree

### 1、结构

基于跳表的LSMTree

![img](https://pic2.zhimg.com/80/v2-37576525d52091fd713bb13556c92861_720w.webp)

### 2、细节

#### （1）skiplist

#### （2）MemTable

每个memtable关联1个跳表做内存中的操作，memtable满了后转成imemtable，在新的memtable上操作。内存中memtable超过上限，则写入disk

#### （3）SSTable

每个sstable关联imemtable，分布在不同层中。每层sstable超过上限需要合并。

#### （4）BloomFilter

基于bitset的布隆过滤器



### 3、todo

合并，两种基本策略：size-tiered和leveled

#### （1）size-tiered

![img](https://pic2.zhimg.com/80/v2-bedb057fde7a4ce4d5be2ea34fe86f59_720w.webp)

#### （2）leveled

![img](https://pic3.zhimg.com/80/v2-5f8de2e435e979936693631617a60d16_720w.webp)