struct Rectangle {
    f32 x;
    f32 y;
    f32 width;
    f32 height;
};

struct Chat_Prompt {
    u16 len;
    char input[32];
};

struct Chat_Message {
    char name[32];
    char msg[128];
};

struct Chat_History {
    s32 base;
    s32 cnt;
    struct Chat_Message messages[64];
};

struct Chat {
    struct Chat_Prompt prompt;
    struct Chat_History history;
};
