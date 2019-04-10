//
// Created by Administrator on 2019/3/16.
//

#ifndef LOGR_STACK_H
#define LOGR_STACK_H

#define STACK_SIZE  20

typedef struct Stack {
    unsigned char a[STACK_SIZE];
    int top;
} Stack;

#define STACK_INIT(stack) do { \
stack = (Stack *)malloc(sizeof(Stack)); \
stack->top = 0; \
}while(0)

#define PUSH(stack, data) do { \
if (stack->top < STACK_SIZE) stack->a[stack->top++] = (unsigned char)data; \
}while(0)

#define POP(stack, data)        do { \
if (stack->top > 0) *data = stack->a[--stack->top]; \
}while(0)

#define STACK_IS_EMPTY(stack)     stack->top == 0

#define STACK_FREE(stack) do { \
free(stack); stack = 0; \
}while(0)

#endif //LOGR_STACK_H
