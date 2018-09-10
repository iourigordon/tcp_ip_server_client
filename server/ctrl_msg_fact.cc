#include <sstream>
#include <cstddef>

using namespace std;

#include "ctrl_msg_fact.h"
#include "ctrl_message.h"

ctrl_message*
ctrl_msg_fact::
create_msg(ctrl_message_id Id)
{
    switch(Id) {
        case CTRL_MSG_ADD_CLIENT:
            return new ctrl_message_add_client();
            break;
        case CTRL_MSG_ACK:
            return new ctrl_message_ack();
            break;
        case CTRL_MSG_NACK:
            return new ctrl_message_nack();
            break;
    }
    return NULL;
}

ctrl_message*
ctrl_msg_fact::
deserialize_stream(istringstream& InStream)
{
    ctrl_message* ctrl_msg = new ctrl_message();

    ctrl_msg->deserialize(InStream);
    switch (ctrl_msg->get_msg_id()) {
        case CTRL_MSG_ADD_CLIENT: {
            ctrl_message_add_client* msg = new ctrl_message_add_client();
            msg->deserialize(InStream);
            delete ctrl_msg;
            ctrl_msg = dynamic_cast<ctrl_message*>(msg);
            break;
        }
        case CTRL_MSG_ACK:
            break;
        case CTRL_MSG_NACK:
            break;
    }
    return ctrl_msg;
}
