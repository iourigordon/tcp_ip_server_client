#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#include <map>
#include <vector>
#include <cstddef>
#include <sstream>
#include <iostream>

using namespace std;

#include "connections.h"
#include "ctrl_msg_fact.h"
#include "ctrl_message.h"
#include "connections_ctrl.h"

#define BUF_LENGTH 1024

connections* connections::connections_pool = NULL;

connections::connections(int ChldSock): m_ChldSock(ChldSock)
{   
    m_MaxFd = m_ChldSock;


    m_FdDescMap[m_ChldSock].desc = "Control";
}


connections*
connections::get_connection(int ChldSock)
{
    if (connections_pool == NULL)
        connections_pool = new connections(ChldSock);

    return connections_pool;
}

void
connections::delete_connection()
{
    if (connections_pool)
        delete connections_pool;
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
        m_MaxFd = 0;
        for (map<int,client_info>::iterator fd = m_FdDescMap.begin(); fd != m_FdDescMap.end(); fd ++) {
            FD_SET(fd->first,&working_set);
            m_MaxFd = max(fd->first,m_MaxFd);
        }
        memset(buff,0,BUF_LENGTH);
        if (select(m_MaxFd+1,&working_set,NULL,NULL,NULL) != -1) {
            for (map<int,client_info>::iterator fd = m_FdDescMap.begin(); fd != m_FdDescMap.end(); fd++) {
                if (FD_ISSET(fd->first,&working_set)) {
                    if (fd->first == m_ChldSock) {
                        if ((ret = ::read(fd->first,buff,BUF_LENGTH)) != -1) {
                            istringstream in_stream(string(buff,ret));
                            ctrl_msg* msg = ctrl_msg_fact::deserialize_stream(in_stream);

                            switch(msg->get_msg_id()) {
                                case CTRL_MSG_ADD_CLIENT: {
                                    ctrl_msg_add_client* client_msg = dynamic_cast<ctrl_msg_add_client*>(msg);
                                    cout << "Request for IP addr = " << client_msg->get_client_ip_addr() << endl;
                                    string client_ip = client_msg->get_client_ip_addr();
                                    delete client_msg;
                                    msg = ctrl_msg_fact::create_msg(CTRL_MSG_ACK);
                                    ostringstream& msg_stream = ctrl_msg_fact::serialize_message(msg);
                                    if (write(m_ChldSock,msg_stream.str().c_str(),msg_stream.str().size()) != -1) {
                                        int sock_id = connections_ctrl::receive_client_sock(fd->first);
                                        m_FdDescMap[sock_id].desc = client_ip;
                                        cout << "Socket fd is received, stream added" << endl;
                                    }
                                    break;
                                }
                                case CTRL_MSG_SHUT_DOWN:
                                    cout << "Got Shut Down Message, closing fds" << endl;
                                    for (map<int,client_info>::iterator fd = m_FdDescMap.begin(); fd != m_FdDescMap.end(); fd++) {
                                        if (close(fd->first) != 0) {
                                            cout << "ERROR closing " << fd->first << endl;
                                        }
                                    }
                                    cout << "Connections Proc is Over" << endl;
                                    m_FdDescMap.clear();
                                    delete msg;
                                    return 0;
                            }
                            delete msg;
                        }
                    } else {
                        ret = ::read(fd->first,buff,BUF_LENGTH);
                        if (ret == 0) {
                            cout << "Client " << fd->second.desc << " socket closed" << endl;
                            if (close(fd->first)) {
                                cout << "ERROR: closing socket " << strerror(errno) << endl;
                            }
                            fd = m_FdDescMap.erase(m_FdDescMap.find(fd->first)); 
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
