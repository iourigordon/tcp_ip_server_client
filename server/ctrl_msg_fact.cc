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
    }
    return NULL;
}

ctrl_msg*
ctrl_msg_fact::
deserialize_stream(istringstream& InStream)
{
    ctrl_msg* msg_base = new ctrl_msg();

    msg_base->deserialize(InStream);
    switch (msg_base->get_msg_id()) {
        case CTRL_MSG_ADD_CLIENT: {
            ctrl_msg_add_client* msg = new ctrl_msg_add_client();
            msg->deserialize(InStream);
            delete msg_base;
            msg_base = dynamic_cast<ctrl_msg*>(msg);
            break;
        }
        case CTRL_MSG_ACK:
            break;
        case CTRL_MSG_NACK:
            break;
    }
    return msg_base;
}
