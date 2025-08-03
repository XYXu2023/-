#include<stdio.h>
#include<stdlib.h>
#include<time.h>  // 用于时钟计时

#define SEARCH_TIMES 1000  // 搜索次数，增加测试量

// 定义元素数量和哈希表大小的不同范围
#define N1 10000
#define N2 50000
#define N3 100000
#define N4 500000
#define N5 1000000

#define SIZE1 20000
#define SIZE2 100000
#define SIZE3 500000
#define SIZE4 1000000
#define SIZE5 2000000

#define EMPTY -1   // 空位置标记

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
        addr = (key + i * i) % p;  // 二次探测
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
    // 定义测试数据集
    int test_sizes[] = {N1, N2, N3, N4, N5};
    int table_sizes[] = {SIZE1, SIZE2, SIZE3, SIZE4, SIZE5};

    // 测试数据准备
    for (int i = 0; i < 5; i++) {
        int N = test_sizes[i];
        int SIZE = table_sizes[i];
        int *keys = (int*)malloc(N * sizeof(int));
        HashEntry *Ha = (HashEntry*)malloc(SIZE * sizeof(HashEntry));
        
        // 随机生成数据
        for (int j = 0; j < N; j++) {
            keys[j] = rand() % 10000000;  // 随机生成数据
        }

        // 初始化并构建哈希表
        InitialHash(Ha, SIZE);
        for (int j = 0; j < N; j++) {
            InsertHash(Ha, keys[j], SIZE);
        }

        // 测试数据准备（包含存在的和不存在的key）
        int test_keys[] = {11, 37, 93, 99, 52, 100};
        int test_count = sizeof(test_keys) / sizeof(test_keys[0]);
        
        clock_t start, end;
        double hash_time, brute_time;
        int hash_steps, brute_steps;
        int total_hash_steps = 0, total_brute_steps = 0;
        
        // 单次搜索对比
        printf("Testing with %d elements and Hash Table Size %d\n", N, SIZE);
        printf("%-10s %-15s %-15s %-15s %-15s\n", 
               "Key", "Hash Found", "Hash Steps", "Brute Found", "Brute Steps");
        
        // 单次搜索对比
        for (int j = 0; j < test_count; j++) {
            int key = test_keys[j];
            
            // 哈希表查找
            start = clock();
            int hash_pos = SearchHash(Ha, key, SIZE, &hash_steps);
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
        for (int j = 0; j < SEARCH_TIMES; j++) {
            for (int k = 0; k < test_count; k++) {
                int dummy_steps;
                SearchHash(Ha, test_keys[k], SIZE, &dummy_steps);
            }
        }
        end = clock();
        hash_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        
        // 暴力搜索测试
        start = clock();
        for (int j = 0; j < SEARCH_TIMES; j++) {
            for (int k = 0; k < test_count; k++) {
                int dummy_steps;
                BruteForceSearch(keys, N, test_keys[k], &dummy_steps);
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
        
        // 释放内存
        free(keys);
        free(Ha);
    }

    return 0;
}
