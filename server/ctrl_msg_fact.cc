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

