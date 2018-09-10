#ifndef __CTRL_MSG_FACT_H__
#define __CTRL_MSG_FACT_H__

typedef enum _ctrl_message_id {
    CTRL_MSG_ADD_CLIENT,
    CTRL_MSG_ACK,
    CTRL_MSG_NACK
}ctrl_message_id;

class ctrl_message;

class ctrl_msg_fact 
{
    public:

        static ctrl_message*  create_msg(ctrl_message_id Id);
        static ctrl_message*  deserialize_stream(istringstream& InStream);
};

#endif
