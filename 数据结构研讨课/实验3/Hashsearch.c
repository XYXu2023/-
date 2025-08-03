#include<stdio.h>
#include<stdlib.h>
#include<time.h>  // 用于时钟计时

#define N 100000  // 增大数据个数
#define SIZE 150000  // 增大哈希表大小
#define EMPTY -1   // 空位置标记
#define SEARCH_TIMES 10000  // 搜索次数，增加测试量

typedef struct {
    int data;
    int isUsed;    // 标记是否已使用
} HashEntry;

// 初始化哈希表
void InitialHash(HashEntry ha[], int m) {
    for (int i = 0; i < m; i++) {
        ha[i].data = EMPTY;
        ha[i].isUsed = 0;
    }
}

// 哈希表插入（二次探测法）
void InsertHash(HashEntry ha[], int key, int p) {
    int addr = key % p;
    int startaddr = addr;
    int i = 1;
    while (ha[addr].isUsed != 0) {
        addr = (key + i * i) % p;
        if (addr == startaddr) {
            printf("Hash table is FULL!\n");
            return;
        }
        i++;
    }
    ha[addr].data = key;
    ha[addr].isUsed = 1;
}

// 哈希表查找（二次探测法）
int SearchHash(HashEntry ha[], int key, int p, int *steps) {
    int addr = key % p;
    int startaddr = addr;
    int i = 1;
    *steps = 1;
    
    while (ha[addr].isUsed != 0) {
        if (ha[addr].data == key) {
            return addr;  // 找到返回位置
        }
        addr = (key + i * i) % p;
        if (addr == startaddr) {
            return -1;  // 未找到
        }
        i++;
        (*steps)++;
    }
    return -1;  // 未找到
}

// 暴力搜索查找
int BruteForceSearch(int keys[], int n, int key, int *steps) {
    *steps = 0;
    for (int i = 0; i < n; i++) {
        (*steps)++;
        if (keys[i] == key) {
            return i;  // 返回数组下标
        }
    }
    return -1;  // 未找到
}

int main() {
    // 随机生成更多的元素
    int keys[N];
    for (int i = 0; i < N; i++) {
        keys[i] = rand() % 100000;  // 随机生成数据
    }

    HashEntry Ha[SIZE];
    int m = SIZE;
    int p = SIZE;
    
    // 初始化并构建哈希表
    InitialHash(Ha, m);
    for (int i = 0; i < N; i++) {
        InsertHash(Ha, keys[i], p);
    }

    // 测试数据准备（包含存在的和不存在的key）
    int test_keys[] = {11, 37, 93, 99, 52, 100};
    int test_count = sizeof(test_keys) / sizeof(test_keys[0]);
    
    clock_t start, end;
    double hash_time, brute_time;
    int hash_steps, brute_steps;
    int total_hash_steps = 0, total_brute_steps = 0;
    
    // 单次搜索对比
    printf("Single Search Comparison:\n");
    printf("%-10s %-15s %-15s %-15s %-15s\n", 
           "Key", "Hash Found", "Hash Steps", "Brute Found", "Brute Steps");
    
    // 单次搜索对比
    for (int i = 0; i < test_count; i++) {
        int key = test_keys[i];
        
        // 哈希表查找
        start = clock();
        int hash_pos = SearchHash(Ha, key, p, &hash_steps);
        end = clock();
        hash_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        
        // 暴力查找
        start = clock();
        int brute_pos = BruteForceSearch(keys, N, key, &brute_steps);
        end = clock();
        brute_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        
        total_hash_steps += hash_steps;
        total_brute_steps += brute_steps;
        
        printf("%-10d %-15d %-15d %-15d %-15d\n", 
               key, hash_pos, hash_steps, brute_pos, brute_steps);
    }
    
    // 多次搜索性能对比
    printf("\nPerformance Comparison (%d searches):\n", SEARCH_TIMES);
    
    // 哈希表搜索测试
    start = clock();
    for (int i = 0; i < SEARCH_TIMES; i++) {
        for (int j = 0; j < test_count; j++) {
            int dummy_steps;
            SearchHash(Ha, test_keys[j], p, &dummy_steps);
        }
    }
    end = clock();
    hash_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // 暴力搜索测试
    start = clock();
    for (int i = 0; i < SEARCH_TIMES; i++) {
        for (int j = 0; j < test_count; j++) {
            int dummy_steps;
            BruteForceSearch(keys, N, test_keys[j], &dummy_steps);
        }
    }
    end = clock();
    brute_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Hash Table Search: %.6f seconds\n", hash_time);
    printf("Brute Force Search: %.6f seconds\n", brute_time);
    printf("Speed Ratio: %.2f:1 (Hash:Brute)\n", brute_time / hash_time);
    
    printf("\nAverage Steps per Search:\n");
    printf("Hash Table: %.2f steps\n", (double)total_hash_steps / test_count);
    printf("Brute Force: %.2f steps\n", (double)total_brute_steps / test_count);
    
    return 0;
}
