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

connections::
connections(int ChldSock, int MaxConnections): 
m_MaxConnections(MaxConnections),m_ChldSock(ChldSock)
{   
    m_MaxFd = m_ChldSock;
    m_FdDescMap[m_ChldSock].desc = "Control";
}


connections*
connections::get_connection(int ChldSock, int MaxConnections)
{
    if (connections_pool == NULL)
        connections_pool = new connections(ChldSock,MaxConnections);

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

    print_debug("Connections Proc is running");

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
                            if (!process_ctrl_msg(buff,ret))
                                return 0;
                        }
                    } else {
                        ostringstream dbg_strm;
                        ret = ::read(fd->first,buff,BUF_LENGTH);
                        if (ret == 0) {
                            dbg_strm << "Client " << fd->second.desc << " socket closed";
                            print_debug(dbg_strm.str().c_str());
                            if (close(fd->first)) {
                                cout << "ERROR: closing socket " << strerror(errno) << endl;
                            }
                            fd = m_FdDescMap.erase(m_FdDescMap.find(fd->first)); 
                        } else if (ret == -1) {
                            cerr << "ERROR: reading from socket " << strerror(errno) << endl;
                        } else {
                            dbg_strm  << "Received \"" << buff << "\" from " << fd->second.desc;
                            print_debug(dbg_strm.str().c_str());
                            fd->second.num_msgs++;
                            ostringstream reply;
                            reply << "Received " << fd->second.num_msgs;
                            if ((ret = write(fd->first,reply.str().c_str(),reply.str().size())) == -1) {
                                cerr << "ERROR: error replying " << strerror(errno) << endl;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int
connections::
process_ctrl_msg(char* Buff,  int BuffLength)
{
    istringstream in_stream(string(Buff,BuffLength));
    ctrl_msg* msg = ctrl_msg_fact::deserialize_stream(in_stream);

    switch(msg->get_msg_id()) {
        case CTRL_MSG_ADD_CLIENT: {
            if ((m_MaxConnections+1)>m_FdDescMap.size()) { 
                ctrl_msg_add_client* client_msg = dynamic_cast<ctrl_msg_add_client*>(msg);
                ostringstream dbg_strm;
                dbg_strm << "Request for IP addr = " << client_msg->get_client_ip_addr();
                print_debug(dbg_strm.str().c_str());
                string client_ip = client_msg->get_client_ip_addr();
                delete client_msg;
                msg = ctrl_msg_fact::create_msg(CTRL_MSG_ACK);
                ostringstream& msg_stream = ctrl_msg_fact::serialize_message(msg);
                if (write(m_ChldSock,msg_stream.str().c_str(),msg_stream.str().size()) != -1) {
                    int sock_id = connections_ctrl::receive_client_sock(m_ChldSock);
                    m_FdDescMap[sock_id].desc = client_ip;
                    print_debug("Socket fd is received, stream added");
                }
            } else {
                delete msg;
                msg = ctrl_msg_fact::create_msg(CTRL_MSG_NACK);
                ostringstream& msg_stream = ctrl_msg_fact::serialize_message(msg);
                if (write(m_ChldSock,msg_stream.str().c_str(),msg_stream.str().size()) != -1) {
                    print_debug("cannot add more clients, sending NACK");
                }              
            }
            break;
        }
        case CTRL_MSG_SHUT_DOWN:
            print_debug("Got Shut Down Message, closing fds");
            for (map<int,client_info>::iterator fd = m_FdDescMap.begin(); fd != m_FdDescMap.end(); fd++) {
                if (close(fd->first) != 0) {
                    cerr << "ERROR closing " << fd->first << endl;
                }
            }
            print_debug("Connections Proc is Over");
            m_FdDescMap.clear();
            delete msg;
            return 0;
        }
    delete msg;
    return 1;
}

void
connections::
print_debug(const char* DebugString)
{
    cout << "Proc " << getpid() << ": " << DebugString << endl;
}
