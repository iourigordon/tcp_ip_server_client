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
ctrl_msg_add_client(int SockId, string ClientIpAddr):
                        ctrl_msg(CTRL_MSG_ADD_CLIENT), m_SockId(SockId),
                        m_ClientIpAddr(ClientIpAddr)
{
}

ostringstream&
ctrl_msg_add_client::
serialize()
{
    m_Stream.str("");
    m_Stream.clear();

    ctrl_msg_fact::serialize_type<int>(m_MsgId,m_Stream);
    ctrl_msg_fact::serialize_type<int>(m_SockId,m_Stream);
    ctrl_msg_fact::serialize_type<string>(m_ClientIpAddr,m_ClientIpAddr.size(),m_Stream);

    return m_Stream;
}

void
ctrl_msg_add_client::
deserialize(istringstream& SerStream)
{
    m_SockId = ctrl_msg_fact::deserialize_type<int>(SerStream);
    ctrl_msg_fact::deserialize_type<string>(SerStream,m_ClientIpAddr);
    cout << "Deserialized m_MsgId " << m_MsgId << endl; 
    cout << "Deserialized m_SockId " << m_SockId << endl;
    cout << "Deserialized m_ClientIpAddr " << m_ClientIpAddr << endl;
}

ostringstream&
ctrl_msg_ack::
serialize()
{
    ctrl_msg_fact::serialize_type<int>(m_MsgId,m_Stream);
    return m_Stream;
}

void
ctrl_msg_ack::
deserialize(istringstream& SerStream)
{
}

ostringstream&
ctrl_msg_nack::
serialize()
{
    ctrl_msg_fact::serialize_type<int>(m_MsgId,m_Stream);
    return m_Stream;
}

void
ctrl_msg_nack::
deserialize(istringstream& SerStream)
{
}
