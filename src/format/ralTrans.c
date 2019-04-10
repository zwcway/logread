//
// Created by Administrator on 2019/4/10.
//

#include <stack.h>
#include "format.h"


int parse_ralTrans(Log *log, const char *line) {
    char *steper = (char *)line;
    char *start = 0, *end = 0, *tmp = 0, *key = 0;
    unsigned char stch = 0;
    unsigned long keyLen = 0, valLen = 0;
    int count = 0;
    Stack *stack;
    Log_field *field;

    // 跳过起始空格
    while(*steper == OP_SPER) steper++;

    STACK_INIT(stack);

    L_INIT_FIELD(field);
    log->value = field;

    key = steper;

    do {
        switch (*steper) {
            case OP_DQOUTE:
            case OP_QOUTE:
                break;
            case OP_SPER:
                // 跳过连续空格
                while (*(steper + 1) == OP_SPER) steper++;

                if (STACK_IS_EMPTY(stack)) {
                    if(!key) key = steper + 1;
                }
                break;
            case OP_OPEN:
                if (key) {
                    keyLen = steper - key;
                    field->key = sub_trim(key, keyLen);
                    key = 0;
                }

                // 重复时取最先的一个
//                if (*(steper - 1) == OP_OPEN) continue;

                if (STACK_IS_EMPTY(stack)) {
                    start = steper;
                }

                PUSH(stack, *steper);
                break;
            case OP_CLOSE:
                // 重复时取最后一个
//                while (*(steper + 1) == OP_CLOSE) steper++;

                POP(stack, &stch);

                if (STACK_IS_EMPTY(stack)) {
                    // 栈结束，表示一个完整的键值遍历完成
                    end = steper;
                    if (start) {
                        valLen = end - start;
                        // 值
                        tmp = sub_str(start + 1, valLen - 1);
                        parse_field(field, tmp);
                        count ++;

                    }
                    if(!key) key = steper + 1;
                    start = 0;
                }
                break;
            default:
                if (STACK_IS_EMPTY(stack)) {

                }
                break;
        }
    } while(!is_eof(++steper));

    //结尾存在字符串
    if (key && !is_end(key)) {
        log->extra = sub_trim(key, steper - key);
    }

    STACK_FREE(stack);

    return count;
}

int format_ralTrans(Log *log, const char *log_line, unsigned long lineno) {
    if (!strstr(log_line, ":[")) return FORMATER_FAILED;

    int colcnt = 0;
    char *logline = (char *)log_line;
    char *stt1, *stt2, *tmp;

    colcnt += parse_ralTrans(log, logline);

    return colcnt;
}