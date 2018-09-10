#ifndef __CTRL_MESSAGE_H__
#define __CTRL_MESSAGE_H__

class ctrl_message
{
    public:
        ctrl_message() {};
        ctrl_message(ctrl_message_id Id): m_MsgId(Id) {};

        virtual ostringstream& serialize();
        virtual void deserialize(istringstream& ser_stream);

    protected:

        template<class T>
        void serialize_type(T Val, size_t ByteSize);

        template<class T>
        void serialize_type(T Val);

        template<class T>
        T deserialize_type(istringstream& Stream);

        template<class T>
        void deserialize_type(istringstream& Stream, T& Val);

        int m_MsgId;
        ostringstream m_Stream;
};

class ctrl_message_add_client: public ctrl_message
{
    public:
        ctrl_message_add_client():ctrl_message(CTRL_MSG_ADD_CLIENT) {};
            ctrl_message_add_client(int SockId, string ClientIpAddr);

        ostringstream& serialize();
        void deserialize(istringstream& SerStream);

        void set_socket_id(int SockId) {m_SockId = SockId;}
        int  get_socket_id() {return m_SockId;}

        void set_client_ip_addr(string& IpAddr) {m_ClientIpAddr = IpAddr;}
        string& get_client_ip_addr() {return m_ClientIpAddr;}

    private:
        int m_SockId;
        string m_ClientIpAddr;
};

class ctrl_message_ack: public ctrl_message
{
    public:
        ctrl_message_ack():ctrl_message(CTRL_MSG_ACK) {};
        ostringstream& serialize();
        void deserialize(istringstream& ser_stream);

};

class ctrl_message_nack : public ctrl_message
{
    public:
        ctrl_message_nack():ctrl_message(CTRL_MSG_NACK) {};

        ostringstream& serialize();
        void deserialize(istringstream& ser_stream);
};

#endif
