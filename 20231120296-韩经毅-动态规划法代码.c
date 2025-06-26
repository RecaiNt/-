#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// 物品结构体定义（符合文档要求的重量/价值范围）
typedef struct {
    int id;             // 物品编号
    double weight;      // 重量（1-100，两位小数）
    double value;       // 价值（100-1000，两位小数）
} Item;

// 生成随机物品数据（严格遵循文档范围）
void generateItems(Item items[], int n) {
    srand(time(NULL) + n);
    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;
        items[i].weight = 1.0 + (rand() % 100);         // 1-100整数重量
        items[i].value = 100.0 + (rand() % 9001) / 10.0; // 100-1000价值（两位小数）
    }
}

// 动态规划法核心（修复段错误问题）
double dynamicProgramming(Item items[], int n, double capacity, int *selected) {
    // 1. 容量精度处理与边界预留
    double capacity_rounded = round(capacity * 100) / 100; // 保留两位小数
    int cap = (int)capacity_rounded;
    if (capacity_rounded > cap) cap++;       // 处理x.999999场景
    cap += 200;                              // 预留200个额外位置防越界
    
    // 2. 动态内存分配与初始化
    double *dp = (double*)malloc((cap + 1) * sizeof(double));
    if (!dp) { printf("内存分配失败\n"); exit(1); }
    memset(dp, 0, (cap + 1) * sizeof(double));
    
    int *path = (int*)malloc(n * sizeof(int));
    if (!path) { free(dp); printf("内存分配失败\n"); exit(1); }
    memset(path, 0, n * sizeof(int));
    
    // 3. 物品重量合法性检查（文档要求1-100）
    for (int i = 0; i < n; i++) {
        if (items[i].weight < 1.0 || items[i].weight > 100.0) {
            printf("警告：物品%d重量=%.2f非法，已跳过\n", i+1, items[i].weight);
            continue;
        }
    }
    
    // 4. 动态规划核心逻辑（带双重边界检查）
    for (int i = 0; i < n; i++) {
        for (int j = cap; j >= items[i].weight; j--) {
            // 检查当前j是否越界
            if (j < 0 || j > cap) continue;
            
            int prev = j - (int)items[i].weight;
            // 检查前一个索引是否越界
            if (prev < 0 || prev > cap) continue;
            
            // 安全更新dp数组
            if (dp[j] < dp[prev] + items[i].value) {
                dp[j] = dp[prev] + items[i].value;
                path[i] = 1;
            }
        }
    }
    
    // 5. 回溯选中物品
    int count = 0;
    double remainingCapacity = capacity;
    for (int i = n - 1; i >= 0 && remainingCapacity > 0; i--) {
        if (path[i] && items[i].weight <= remainingCapacity) {
            if (count >= n) {
                printf("错误：选中物品数超过n=%d\n", n);
                free(path); free(dp); exit(1);
            }
            selected[count++] = i;
            remainingCapacity -= items[i].weight;
        }
    }
    selected[0] = count;
    
    // 6. 结果索引合法性检查（段错误修复点）
    int result_idx = (int)capacity;
    if (result_idx < 0 || result_idx > cap) {
        printf("错误：结果索引%d越界，cap=%d\n", result_idx, cap);
        free(path); free(dp); exit(1);
    }
    
    double result = dp[result_idx];
    free(path);
    free(dp);
    return result;
}

// 结果输出函数（符合文档格式要求）
void printResults(Item items[], int n, double capacity, double totalValue, 
                  double timeUsed, int *selected) {
    printf("\n===== 动态规划法求解结果 =====\n");
    printf("物品数量: %d\n", n);
    printf("背包容量: %.2f\n", capacity);
    printf("最大总价值: %.2f\n", totalValue);
    printf("执行时间: %.2f ms\n", timeUsed);
    printf("选中物品数量: %d\n", selected[0]);
    printf("------------------------\n");
    
    double totalWeight = 0.0;
    for (int i = 1; i <= selected[0]; i++) {
        int idx = selected[i];
        totalWeight += items[idx].weight;
    }
    printf("总重量: %.2f / %.2f\n", totalWeight, capacity);
    printf("------------------------\n");
}

int main() {
    int n;
    double capacity;
    Item items[320000];        // 支持最大32万物品
    int *selected = NULL;      // 选中物品索引数组
    
    printf("===== 0-1背包问题-动态规划法 =====\n");
    printf("请输入物品数量n: ");
    scanf("%d", &n);
    if (n <= 0 || n > 320000) {
        printf("错误：物品数量需在1-320000之间\n");
        return 1;
    }
    
    printf("请输入背包容量C: ");
    scanf("%lf", &capacity);
    if (capacity <= 0) {
        printf("错误：背包容量需大于0\n");
        return 1;
    }
    
    // 动态分配结果数组
    selected = (int*)malloc((n + 1) * sizeof(int));
    if (!selected) {
        printf("错误：结果数组分配失败\n");
        return 1;
    }
    memset(selected, 0, (n + 1) * sizeof(int));
    
    // 生成物品数据
    generateItems(items, n);
    printf("已生成%d个物品，背包容量=%.2f\n", n, capacity);
    
    // 计时开始
    clock_t start = clock();
    double maxValue = dynamicProgramming(items, n, capacity, selected);
    clock_t end = clock();
    double timeUsed = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    
    // 输出结果
    printResults(items, n, capacity, maxValue, timeUsed, selected);
    
    // 释放内存（避免泄漏）
    free(selected);
    return 0;
}
