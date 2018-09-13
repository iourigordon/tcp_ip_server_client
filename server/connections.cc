#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#include <map>
#include <cstddef>
#include <sstream>
#include <iostream>

using namespace std;

#include "connections.h"
#include "ctrl_msg_fact.h"
#include "ctrl_message.h"

#define BUF_LENGTH 1024

connections* connections::connections_pool = NULL;

connections::connections(int ctrl_in, int ctrl_out):
                         m_CtrlIn(ctrl_in), m_CtrlOut(ctrl_out)
{   
    m_MaxFd = m_CtrlIn;

    m_FdDescMap[m_CtrlIn].desc = "Control In";
    m_FdDescMap[m_CtrlOut].desc = "Control Out";
}

connections::~connections()
{
}

connections*
connections::get_connection(int ctrl_in, int ctrl_out)
{
    if (connections_pool == NULL)
        connections_pool = new connections(ctrl_in,ctrl_out);

    return connections_pool;
}

int
connections::add_connection(int client_socket)
{
    return 0;
}

int
connections::run()
{
    int ret;
    fd_set working_set;
    char buff[BUF_LENGTH];

    cout << "Connections Proc is running" << endl;

    for(;;) {
        FD_ZERO(&working_set);
        for (map<int,client_info>::iterator fd = m_FdDescMap.begin(); fd != m_FdDescMap.end(); fd ++) {
            if (fd->first != m_CtrlOut){
                FD_SET(fd->first,&working_set);
                m_MaxFd = max(fd->first,m_MaxFd);
            }
        }
        memset(buff,0,BUF_LENGTH);
        if (select(m_MaxFd+1,&working_set,NULL,NULL,NULL) != -1) {
            for (map<int,client_info>::iterator fd = m_FdDescMap.begin(); fd != m_FdDescMap.end(); fd++) {
                if (FD_ISSET(fd->first,&working_set)) {
                    if (fd->first == m_CtrlIn) {
                        if ((ret = ::read(fd->first,buff,BUF_LENGTH)) != -1) {
                            istringstream in_stream(string(buff,ret));
                            ctrl_msg* msg = ctrl_msg_fact::deserialize_stream(in_stream);

                            if (msg->get_msg_id() == CTRL_MSG_ADD_CLIENT) {
                                ctrl_msg_add_client* client_msg = dynamic_cast<ctrl_msg_add_client*>(msg);
                                cout << "SockID = " << client_msg->get_socket_id() << "; IP addr = " << client_msg->get_client_ip_addr() << endl;
                                m_FdDescMap[client_msg->get_socket_id()].desc = client_msg->get_client_ip_addr();
                            }
                        }
                    } else {
                        ret = ::read(fd->first,buff,BUF_LENGTH);
                        if (ret == 0) {
                            cout << "Client " << fd->second.desc << " socket closed" << endl;
                            if (close(fd->first)) {
                                cout << "ERROR: closing socket " << strerror(errno) << endl;
                            }
                            m_FdDescMap.erase(m_FdDescMap.find(fd->first)); 
                        } else if (ret == -1) {
                            cout << "ERROR: reading from socket " << strerror(errno) << endl;
                        } else {
                            cout << "Received \"" << buff << "\" from " << fd->second.desc << endl;
                            fd->second.num_msgs++;
                            ostringstream reply;
                            reply << "Received " << fd->second.num_msgs;
                            if ((ret = write(fd->first,reply.str().c_str(),reply.str().size())) == -1) {
                                cout << "ERROR: error replying " << strerror(errno) << endl;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
