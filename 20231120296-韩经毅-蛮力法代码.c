#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 物品结构体定义
typedef struct {
    int id;             // 物品编号
    double weight;      // 物品重量
    double value;       // 物品价值
} Item;

// 生成随机物品数据
void generateItems(Item items[], int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;
        // 生成1-100之间的重量，保留两位小数
        items[i].weight = (rand() % 100) + 1.0;
        // 生成100-1000之间的价值，保留两位小数
        items[i].value = 100.0 + (rand() % 9001) / 10.0;
    }
}

// 蛮力法：枚举所有可能的子集
double bruteForce(Item items[], int n, double capacity, int *selected) {
    double maxValue = 0.0;
    int bestSet[100000] = {0}; // 存储最优解的物品索引
    int bestCount = 0;
    
    // 计算需要枚举的子集数量：2^n
    unsigned long long totalSubsets = 1;
    for (int i = 0; i < n; i++) {
        totalSubsets <<= 1; // 等价于totalSubsets *= 2
        // 提示：当子集数量过大时可能需要极长时间
        if (totalSubsets > 1000000000 && i < n-1) {
            printf("警告：物品数量n=%d，子集数量达%llu，可能需要极长时间计算\n", 
                   n, totalSubsets);
        }
    }
    
    // 枚举所有2^n个子集
    for (unsigned long long i = 0; i < totalSubsets; i++) {
        double totalWeight = 0.0;
        double totalValue = 0.0;
        int currentSet[100000] = {0};
        int count = 0;
        
        // 检查当前子集的每个物品
        for (int j = 0; j < n; j++) {
            if (i & (1ULL << j)) { // 使用1ULL避免int溢出
                totalWeight += items[j].weight;
                totalValue += items[j].value;
                currentSet[count++] = j;
            }
        }
        
        // 更新最优解
        if (totalWeight <= capacity && totalValue > maxValue) {
            maxValue = totalValue;
            bestCount = count;
            for (int k = 0; k < count; k++) {
                bestSet[k] = currentSet[k];
            }
        }
    }
    
    // 记录选中的物品
    selected[0] = bestCount;
    for (int k = 0; k < bestCount; k++) {
        selected[k + 1] = bestSet[k];
    }
    
    return maxValue;
}

// 输出结果函数
void printResults(Item items[], int n, double capacity, double totalValue, 
                  double timeUsed, int *selected) {
    printf("\n蛮力法求解0-1背包问题结果:\n");
    printf("------------------------\n");
    printf("背包容量: %.2f\n", capacity);
    printf("物品数量: %d\n", n);
    printf("最大总价值: %.2f\n", totalValue);
    printf("执行时间: %.2f ms\n", timeUsed);
    printf("选中物品数量: %d\n", selected[0]);
    printf("选中物品详情:\n");
    
    double totalWeight = 0.0;
    for (int i = 1; i <= selected[0]; i++) {
        int idx = selected[i];
        printf("  物品ID: %d, 重量: %.2f, 价值: %.2f\n",
               items[idx].id, items[idx].weight, items[idx].value);
        totalWeight += items[idx].weight;
    }
    
    printf("------------------------\n");
    printf("总重量: %.2f / %.2f\n", totalWeight, capacity);
    printf("------------------------\n");
}

int main() {
    int n;                      // 物品数量
    double capacity;            // 背包容量
    Item items[100000];         // 物品数组
    int selected[100000] = {0}; // 存储选中的物品索引，第一个元素是数量
    
    // 输入物品数量和背包容量
    printf("请输入物品数量n: ");
    scanf("%d", &n);
    printf("请输入背包容量C: ");
    scanf("%lf", &capacity);
    
    // 生成随机物品数据
    generateItems(items, n);
    printf("已生成%d个物品，背包容量=%.2f\n", n, capacity);
    
    // 测试蛮力法
    clock_t start, end;
    double timeUsed;
    start = clock();
    double bruteValue = bruteForce(items, n, capacity, selected);
    end = clock();
    timeUsed = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    
    // 输出结果
    printResults(items, n, capacity, bruteValue, timeUsed, selected);
    
    return 0;
}
