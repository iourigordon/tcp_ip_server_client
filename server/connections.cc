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

    m_FdDescMap[m_CtrlIn] = "Control In";
    m_FdDescMap[m_CtrlOut] = "Control Out";
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
        for (map<int,string>::iterator fd = m_FdDescMap.begin(); fd != m_FdDescMap.end(); fd ++) {
            if (fd->first != m_CtrlOut){
                FD_SET(fd->first,&working_set);
                m_MaxFd = max(fd->first,m_MaxFd);
            }
        }
        memset(buff,0,BUF_LENGTH);
        if (select(m_MaxFd+1,&working_set,NULL,NULL,NULL) != -1) {
            cout << "select returned" << endl;
            for (map<int,string>::iterator fd = m_FdDescMap.begin(); fd != m_FdDescMap.end(); fd++) {
                if (FD_ISSET(fd->first,&working_set)) {
                    if (fd->first == m_CtrlIn) {
                        if ((ret = ::read(fd->first,buff,BUF_LENGTH)) != -1) {
                            istringstream in_stream(string(buff,ret));
                            ctrl_msg* msg = ctrl_msg_fact::deserialize_stream(in_stream);

                            if (msg->get_msg_id() == CTRL_MSG_ADD_CLIENT) {
                                ctrl_msg_add_client* client_msg = dynamic_cast<ctrl_msg_add_client*>(msg);
                                cout << "SockID = " << client_msg->get_socket_id() << "; IP addr = " << client_msg->get_client_ip_addr() << endl;
                            }
                        }
                    }                
                }
            }
        }
    }
    return 0;
}
