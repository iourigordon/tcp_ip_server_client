#ifndef __CTRL_MESSAGE_H__
#define __CTRL_MESSAGE_H__

class ctrl_msg
{
    public:

        friend class ctrl_msg_fact;

        ctrl_msg() {};
        ctrl_msg(ctrl_msg_id Id): m_MsgId(Id) {};

        int get_msg_id() {return m_MsgId;}

    protected:
        virtual ostringstream& serialize();
        virtual void deserialize(istringstream& InStream);

        int m_MsgId;
        ostringstream m_Stream;
};

class ctrl_msg_add_client: public ctrl_msg
{
    public:
        friend class ctrl_msg_fact;

        ctrl_msg_add_client():ctrl_msg(CTRL_MSG_ADD_CLIENT) {};
        ctrl_msg_add_client(string ClientIpAddr);

        void set_client_ip_addr(string& IpAddr) {m_ClientIpAddr = IpAddr;}
        string& get_client_ip_addr() {return m_ClientIpAddr;}
    
    protected:
        ostringstream& serialize();
        void deserialize(istringstream& InStream);

    private:
        string m_ClientIpAddr;
};

class ctrl_msg_ack: public ctrl_msg
{
    public:
        friend class ctrl_msg_fact;
        ctrl_msg_ack():ctrl_msg(CTRL_MSG_ACK) {};
};

class ctrl_msg_nack : public ctrl_msg
{
    public:
        friend class ctrl_msg_fact;
        ctrl_msg_nack():ctrl_msg(CTRL_MSG_NACK) {};
};

class ctrl_msg_shutdown : public ctrl_msg
{
    public:
        friend class ctrl_msg_fact;
        ctrl_msg_shutdown():ctrl_msg(CTRL_MSG_SHUT_DOWN) {};
};

#endif
