#ifndef LOGIN_H
#define LOGIN_H

enum Login_Flags {
    LOGIN_WAITING = 1<<0,
    LOGIN_FAILED  = 1<<1,
    LOGIN_SUCCESS = 1<<2
};

enum Login_Index {
    LOGIN_INDEX_ADDR,
    LOGIN_INDEX_PORT,
    LOGIN_INDEX_NAME,
    LOGIN_CNT_INDICES
};

struct Login {
    struct Platform_Connection *connection;
    u16 flags;
    u16 index;
    u16 lengths[LOGIN_CNT_INDICES];
    char values[LOGIN_CNT_INDICES][128];
};

#endif // LOGIN_H
