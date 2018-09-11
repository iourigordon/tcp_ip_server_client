#ifndef __CTRL_MESSAGE_H__
#define __CTRL_MESSAGE_H__

class ctrl_msg
{
    public:

        friend class ctrl_msg_fact;

        ctrl_msg() {};
        ctrl_msg(ctrl_msg_id Id): m_MsgId(Id) {};

        int get_msg_id() {return m_MsgId;}

        virtual ostringstream& serialize();

    protected:

        int m_MsgId;
        ostringstream m_Stream;

    private:
        virtual void deserialize(istringstream& ser_stream);
};

class ctrl_msg_add_client: public ctrl_msg
{
    public:
        friend class ctrl_msg_fact;

        ctrl_msg_add_client():ctrl_msg(CTRL_MSG_ADD_CLIENT) {};
        ctrl_msg_add_client(int SockId, string ClientIpAddr);

        ostringstream& serialize();

        void set_socket_id(int SockId) {m_SockId = SockId;}
        int  get_socket_id() {return m_SockId;}

        void set_client_ip_addr(string& IpAddr) {m_ClientIpAddr = IpAddr;}
        string& get_client_ip_addr() {return m_ClientIpAddr;}

    private:
        
        void deserialize(istringstream& SerStream);

        int m_SockId;
        string m_ClientIpAddr;
};

class ctrl_msg_ack: public ctrl_msg
{
    public:
        friend class ctrl_msg_fact;

        ctrl_msg_ack():ctrl_msg(CTRL_MSG_ACK) {};
        ostringstream& serialize();

    private:
        void deserialize(istringstream& ser_stream);

};

class ctrl_msg_nack : public ctrl_msg
{
    public:
        friend class ctrl_msg_fact;

        ctrl_msg_nack():ctrl_msg(CTRL_MSG_NACK) {};
        ostringstream& serialize();

    private:
        void deserialize(istringstream& ser_stream);
};

#endif
