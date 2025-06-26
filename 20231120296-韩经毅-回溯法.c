#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

typedef struct {
    int id;
    double weight;
    double value;
    double density;
} Item;

typedef struct {
    int level;
    int choice;
    double currentWeight;
    double currentValue;
} StackFrame;

int *bestChoice;
double bestValue = 0.0;
int n;
double capacity;
Item *items;
int stackSize = 0;
StackFrame *stack = NULL;

// 比较函数：按价值密度降序
int compare(const void *a, const void *b) {
    Item *itemA = (Item*)a;
    Item *itemB = (Item*)b;
    return (itemA->density > itemB->density) ? -1 : 1;
}

// 生成物品数据
void generateItems() {
    srand(time(NULL));
    items = (Item*)malloc(n * sizeof(Item));
    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;
        items[i].weight = 1.0 + (double)rand() / RAND_MAX * 99.0;
        items[i].value = 100.0 + (double)rand() / RAND_MAX * 900.0;
        items[i].density = items[i].value / items[i].weight;
    }
    qsort(items, n, sizeof(Item), compare);
}

// 计算上界函数
double bound(int level, double currentWeight, double currentValue) {
    double remaining = capacity - currentWeight;
    double boundValue = currentValue;
    
    while (level < n && items[level].weight <= remaining) {
        remaining -= items[level].weight;
        boundValue += items[level].value;
        level++;
    }
    
    if (level < n) {
        boundValue += (remaining / items[level].weight) * items[level].value;
    }
    
    return boundValue;
}

// 动态扩展栈空间
void expandStack(int neededSize) {
    int newSize = stackSize == 0 ? 1024 : stackSize * 2;
    while (newSize < neededSize) newSize *= 2;
    
    StackFrame *newStack = (StackFrame*)realloc(stack, newSize * sizeof(StackFrame));
    if (!newStack) {
        size_t requested_kb = (size_t)newSize * sizeof(StackFrame) / 1024;
        printf("内存分配失败，请求大小=%zu KB\n", requested_kb);
        exit(1);
    }
    
    stack = newStack;
    stackSize = newSize;
    
    size_t current_kb = (size_t)stackSize * sizeof(StackFrame) / 1024;
    printf("栈空间扩展至%d个元素（%zu KB）\n", stackSize, current_kb);
}

// 记录最优选择
void recordBestChoice(int top) {
    memset(bestChoice, 0, n * sizeof(int));
    for (int i = 0; i <= top; i++) {
        if (stack[i].choice == 0 && stack[i].level < n) {
            bestChoice[stack[i].level] = 1;
        }
    }
}

// 迭代回溯法（动态栈实现）
void iterativeBacktrack() {
    int top = -1; // 栈顶指针
    
    // 初始栈帧
    expandStack(1);
    stack[0].level = 0;
    stack[0].choice = -1;
    stack[0].currentWeight = 0.0;
    stack[0].currentValue = 0.0;
    top = 0;
    
    while (top >= 0) {
        StackFrame *frame = &stack[top];
        
        if (frame->choice == -1) {
            // 尝试选择当前物品
            frame->choice = 0;
            if (frame->currentWeight + items[frame->level].weight <= capacity) {
                // 检查栈空间是否足够
                if (top + 1 >= stackSize) expandStack(top + 2);
                
                StackFrame newFrame;
                newFrame.level = frame->level + 1;
                newFrame.choice = -1;
                newFrame.currentWeight = frame->currentWeight + items[frame->level].weight;
                newFrame.currentValue = frame->currentValue + items[frame->level].value;
                
                stack[++top] = newFrame;
                
                // 更新最优解
                if (newFrame.currentValue > bestValue) {
                    bestValue = newFrame.currentValue;
                    recordBestChoice(top);
                }
            }
        } else if (frame->choice == 0) {
            // 尝试不选当前物品
            frame->choice = 1;
            double b = bound(frame->level + 1, frame->currentWeight, frame->currentValue);
            if (b > bestValue) {
                if (top + 1 >= stackSize) expandStack(top + 2);
                
                StackFrame newFrame;
                newFrame.level = frame->level + 1;
                newFrame.choice = -1;
                newFrame.currentWeight = frame->currentWeight;
                newFrame.currentValue = frame->currentValue;
                
                stack[++top] = newFrame;
            }
        } else {
            // 回溯
            top--;
        }
    }
}

// 输出结果
void printResults(double timeUsed) {
    int count = 0;
    double totalWeight = 0.0;
    
    for (int i = 0; i < n; i++) {
        if (bestChoice[i]) {
            count++;
            totalWeight += items[i].weight;
        }
    }
    
    printf("\n===== 回溯法求解结果 =====\n");
    printf("物品数量: %d\n", n);
    printf("背包容量: %.2f\n", capacity);
    printf("最大总价值: %.2f\n", bestValue);
    printf("执行时间: %.2f ms\n", timeUsed);
    printf("选中物品数量: %d\n", count);
    printf("------------------------\n");
    printf("总重量: %.2f / %.2f\n", totalWeight, capacity);
    printf("------------------------\n");
}

// 主函数
int main() {
    printf("===== 0-1背包问题-回溯法 =====\n");
    printf("请输入物品数量n: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("错误：物品数量必须为正整数\n");
        return 1;
    }
    
    printf("请输入背包容量C: ");
    if (scanf("%lf", &capacity) != 1 || capacity <= 0) {
        printf("错误：背包容量必须为正数\n");
        return 1;
    }
    
    generateItems();
    bestChoice = (int*)malloc(n * sizeof(int));
    if (!bestChoice) {
        printf("内存分配失败\n");
        return 1;
    }
    memset(bestChoice, 0, n * sizeof(int));
    
    printf("已生成%d个物品，背包容量=%.2f\n", n, capacity);
    
    clock_t start = clock();
    iterativeBacktrack();
    clock_t end = clock();
    double timeUsed = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    
    printResults(timeUsed);
    
    free(items);
    free(bestChoice);
    if (stack) free(stack);
    
    return 0;
}
