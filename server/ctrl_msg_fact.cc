#include <sstream>
#include <cstddef>

using namespace std;

#include "ctrl_msg_fact.h"
#include "ctrl_message.h"

ctrl_msg*
ctrl_msg_fact::
create_msg(ctrl_msg_id Id)
{
    switch(Id) {
        case CTRL_MSG_ADD_CLIENT:
            return new ctrl_msg_add_client();
            break;
        case CTRL_MSG_ACK:
            return new ctrl_msg_ack();
            break;
        case CTRL_MSG_NACK:
            return new ctrl_msg_nack();
            break;
        case CTRL_MSG_SHUT_DOWN:
            return new ctrl_msg_shutdown();
            break;
    }
    return NULL;
}

ostringstream&
ctrl_msg_fact::
serialize_message(ctrl_msg* Msg)
{
    return Msg->serialize();
}

ctrl_msg*
ctrl_msg_fact::
deserialize_stream(istringstream& InStream)
{
    ctrl_msg* msg_base;
    int msg_id = ctrl_msg_fact::deserialize_type<int>(InStream);

    switch (msg_id) {
        case CTRL_MSG_ADD_CLIENT: {
            ctrl_msg_add_client* msg = new ctrl_msg_add_client();
            msg->deserialize(InStream);
            msg_base = dynamic_cast<ctrl_msg*>(msg);
            break;
        }
        case CTRL_MSG_ACK: {
            ctrl_msg_ack* msg = new ctrl_msg_ack();
            msg_base = dynamic_cast<ctrl_msg_ack*>(msg); 
            break;
        }
        case CTRL_MSG_NACK: {
            ctrl_msg_nack* msg = new ctrl_msg_nack();
            msg_base = dynamic_cast<ctrl_msg_nack*>(msg);
            break;
        }
        case CTRL_MSG_SHUT_DOWN: {
            ctrl_msg_shutdown* msg = new ctrl_msg_shutdown();
            msg_base = dynamic_cast<ctrl_msg_shutdown*>(msg);
            break;
        }
    }
    return msg_base;
}
