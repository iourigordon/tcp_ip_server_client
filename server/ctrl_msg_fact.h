#ifndef __CTRL_MSG_FACT_H__
#define __CTRL_MSG_FACT_H__

#include <endian.h>
#include <netinet/in.h>

typedef enum _ctrl_msg_id {
    CTRL_MSG_ADD_CLIENT,
    CTRL_MSG_ACK,
    CTRL_MSG_NACK,
    CTRL_MSG_SHUT_DOWN
}ctrl_msg_id;


class ctrl_msg;

class ctrl_msg_fact 
{
    public:
        static ctrl_msg*        create_msg(ctrl_msg_id Id);
        static ostringstream&   serialize_message(ctrl_msg* Msg);
        static ctrl_msg*        deserialize_stream(istringstream& InStream);


    private:
        ctrl_msg_fact();
        ~ctrl_msg_fact();
        
        ctrl_msg_fact(ctrl_msg_fact& Rhs);
        ctrl_msg_fact& operator=(ctrl_msg_fact& Rhs);

        friend class ctrl_msg;
        friend class ctrl_msg_add_client;
        friend class ctrl_msg_ack;
        friend class ctrl_msg_nack;
        friend class ctrl_msg_shut_down;

        static uint64_t hton(uint64_t n) { return htobe64(n); }
        static uint32_t hton(uint32_t n) { return htonl(n); }
        static uint16_t hton(uint16_t n) { return htons(n); }
        static int      hton(int n)      { return htonl(n); }

        static uint64_t ntoh(uint64_t n) { return be64toh(n); }
        static uint32_t ntoh(uint32_t n) { return ntohl(n); }
        static uint16_t ntoh(uint16_t n) { return ntohs(n); }
        static int      ntoh(int n)      { return ntohl(n); }


        template<class T>
        static void serialize_type(T Val, size_t ByteSize, ostringstream& Stream) {
            size_t size = hton(ByteSize);
            Stream.write((const char*)&size,sizeof(size_t));
            Stream << Val;
        }

        template<class T>
        static void serialize_type(T Val, ostringstream& Stream) {
            size_t size = hton(sizeof Val);
            Val = hton(Val);
            Stream.write((const char*)&size, sizeof(size_t));
            Stream.write((const char*)&Val,sizeof Val);
        }

        template<class T>
        static T deserialize_type(istringstream& Stream) {
            T val{};
    
            size_t size_val;
            size_t size = sizeof val;

            Stream.read((char*)&size_val,sizeof size);
            Stream.read((char*)&val,ntoh(size_val));

            return ntoh(val);
        }

        template<class T>
        static void deserialize_type(istringstream& Stream, T& Val) {
            size_t size_val;
            size_t size = sizeof size_val;

            Stream.read((char*)&size_val,sizeof size);
            size_val = ntoh(size_val);

            Stream >> Val;
        }
};

#endif
