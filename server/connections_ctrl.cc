#include <unistd.h>
#include <sys/select.h>

#include <string>
#include <vector>
#include <cstddef>
#include <sstream>
#include <iostream>

using namespace std;

#include "connections_ctrl.h"
#include "ctrl_msg_fact.h"
#include "ctrl_message.h"

#define BUFF_LENGTH 1024

add_new_conn_action
connections_ctrl::
add_client(int SockId, string IpAddr)
{
    int ret;
    fd_set conn_set;
    char buff[BUFF_LENGTH];

    if (m_Procs.empty())
        return CREATE_CONN;

    ctrl_msg_add_client* msg = dynamic_cast<ctrl_msg_add_client*>(ctrl_msg_fact::create_msg(CTRL_MSG_ADD_CLIENT));
    msg->set_socket_id(SockId);
    msg->set_client_ip_addr(IpAddr);
    ostringstream& msg_stream = ctrl_msg_fact::serialize_message(msg);
    for(vector<proc_io>::iterator& curr = m_Procs.begin();curr<m_Procs.end();curr++) {
        if (write(curr.m_ToChld,msg_stream.str().c_str(),msg_stream.str().size()) != -1) {
            FD_ZERO(&conn_set);
            FD_SET(curr.m_ToPrnt,&conn_set);
            if (select(curr.m_ToPrnt+1,&conn_set,NULL,NULL,NULL) != -1) {
                if (FD_ISSET(curr.m_ToPrnt,&conn_set) && ((ret = read(curr.m_ToPrnt,buff,BUFF_LENGTH)) != -1)) {
                    istringstream in_stream(string(buff,ret));
                    ctrl_msg* msg = ctrl_msg_fact::deserialize_stream(in_stream);
                    if (msg->get_msg_id() == CTRL_MSG_ACK) {
                        delete msg;
                        return SUCCESS;
                    }
                    delete msg;
                }
            }
        }
    }

    return OUT_OF_CONN;
}
