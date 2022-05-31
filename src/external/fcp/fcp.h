#ifndef FCP_H
#define FCP_H

// NOTE:
// FCP_S_... means package made in server and coming from server
// FCP_C_... means package made in client and coming from client


/*********************
 *  client packages  *
 *********************/

#define FCP_C_LOGIN           0
#define FCP_C_CHAT_MESSAGE    1
struct FCP_C_Login_Desc {
    u16 type;
    u16 name_len;
};

struct FCP_C_Chat_Message_Desc {
    u16 type;
    u16 message_len;
};



/*********************
 *  server packages  *
 *********************/
 
// types
#define FCP_S_STATUS       0
#define FCP_S_CHAT_MESSAGE 1

// status
#define FCP_S_ERROR    0
#define FCP_S_SUCCESS  1

struct FCP_S_Status {
    u16 type;
    u16 status;
};

struct FCP_S_Chat_Message_Desc {
    u16 type;
    u16 sender_len;
    u16 message_len;
};


#endif // FCP_H
