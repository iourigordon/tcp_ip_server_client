#include <endian.h>
#include <netinet/in.h>

#include <sstream>
#include <iostream>

using namespace std;

#include "ctrl_msg_fact.h"
#include "ctrl_message.h"

uint64_t hton(uint64_t n) { return htobe64(n); }
uint32_t hton(uint32_t n) { return htonl(n); }
uint16_t hton(uint16_t n) { return htons(n); }
int      hton(int n)      { return htonl(n); }

uint64_t ntoh(uint64_t n) { return be64toh(n); }
uint32_t ntoh(uint32_t n) { return ntohl(n); }
uint16_t ntoh(uint16_t n) { return ntohs(n); }
int      ntoh(int n)      { return ntohl(n); }

ostringstream&
ctrl_message::serialize()
{
    m_Stream.str("");
    m_Stream.clear();

    serialize_type<int>(m_MsgId);
    return m_Stream; 
}

void
ctrl_message::deserialize(istringstream& ser_stream)
{
    m_MsgId = deserialize_type<int>(ser_stream);
}

template<class T>
void
ctrl_message::serialize_type(T Val, size_t ByteSize)
{
    size_t size = ::hton(ByteSize);
    m_Stream.write((const char*)&size,sizeof(size_t));

    m_Stream << Val;
}

template<class T>
void
ctrl_message::serialize_type(T Val)
{
    size_t size = ::hton(sizeof Val);
    Val = :: hton(Val);
    m_Stream.write((const char*)&size, sizeof(size_t));
    m_Stream.write((const char*)&Val,sizeof Val);
}

template<class T>
T
ctrl_message::deserialize_type(istringstream& Stream)
{
    T val;
    
    size_t size_val;
    size_t size = sizeof val;

    Stream.read((char*)&size_val,sizeof size);
    Stream.read((char*)&val,::ntoh(size_val));

    return ::ntoh(val);
}

template<class T>
void
ctrl_message::deserialize_type(istringstream& Stream, T& Val)
{
    size_t size_val;
    size_t size = sizeof size_val;

    Stream.read((char*)&size_val,sizeof size);
    size_val = ::ntoh(size_val);

    Stream >> Val;
}


ctrl_message_add_client::
ctrl_message_add_client(int SockId, string ClientIpAddr):
                        ctrl_message(CTRL_MSG_ADD_CLIENT), m_SockId(SockId),
                        m_ClientIpAddr(ClientIpAddr)
{
}

ostringstream&
ctrl_message_add_client::
serialize()
{
    m_Stream.str("");
    m_Stream.clear();

    serialize_type<int>(m_MsgId);
    serialize_type<int>(m_SockId);
    serialize_type<string>(m_ClientIpAddr,m_ClientIpAddr.size());

    return m_Stream;
}

void
ctrl_message_add_client::
deserialize(istringstream& SerStream)
{
    m_SockId = deserialize_type<int>(SerStream);
    deserialize_type<string>(SerStream,m_ClientIpAddr);
    cout << "Deserialized m_MsgId " << m_MsgId << endl; 
    cout << "Deserialized m_SockId " << m_SockId << endl;
    cout << "Deserialized m_ClientIpAddr " << m_ClientIpAddr << endl;
}

ostringstream&
ctrl_message_ack::
serialize()
{
    serialize_type<int>(m_MsgId);
    return m_Stream;
}

void
ctrl_message_ack::
deserialize(istringstream& SerStream)
{
}

ostringstream&
ctrl_message_nack::
serialize()
{
    serialize_type<int>(m_MsgId);
    return m_Stream;
}

void
ctrl_message_nack::
deserialize(istringstream& SerStream)
{
}
