#include <sstream>
#include <iostream>

using namespace std;

#include "ctrl_msg_fact.h"
#include "ctrl_message.h"

ostringstream&
ctrl_msg::serialize()
{
    m_Stream.str("");
    m_Stream.clear();

    ctrl_msg_fact::test();
    ctrl_msg_fact::serialize_type<int>(m_MsgId,m_Stream);
    return m_Stream;
}

void
ctrl_msg::deserialize(istringstream& ser_stream)
{
    m_MsgId = ctrl_msg_fact::deserialize_type<int>(ser_stream);
}

ctrl_msg_add_client::
ctrl_msg_add_client(string ClientIpAddr):
                    ctrl_msg(CTRL_MSG_ADD_CLIENT), m_ClientIpAddr(ClientIpAddr)
{
}

ostringstream&
ctrl_msg_add_client::
serialize()
{
    m_Stream.str("");
    m_Stream.clear();

    ctrl_msg_fact::serialize_type<int>(m_MsgId,m_Stream);
    ctrl_msg_fact::serialize_type<string>(m_ClientIpAddr,m_ClientIpAddr.size(),m_Stream);

    return m_Stream;
}

void
ctrl_msg_add_client::
deserialize(istringstream& SerStream)
{
    ctrl_msg_fact::deserialize_type<string>(SerStream,m_ClientIpAddr);
}
