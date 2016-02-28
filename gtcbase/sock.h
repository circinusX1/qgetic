/*
    Author Marius Chincisan Aug 2008
*/
#ifndef __SOCKXL_H__
#define __SOCKXL_H__


#ifdef _WIN32

#   pragma warning (disable: 4267)
#   pragma warning (disable: 4244)
#else
#   include <GNU_LINUX/os.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <netinet/in.h>
#   include <fstream>
#endif //_WIN32

using namespace std;

#ifndef _WIN32
    typedef int32_t SOCKET;
#else
    typedef int32_t socklen_t;
#endif //

//---------------------------------------------------------------------------------------
#define MAKE_IP(a_,b_,c_,d_) (u_int32_t)( (a_ <<24) | (b_<<16) | (c_<<8) | d_)


//---------------------------------------------------------------------------------------
typedef u_bool32 (*CancelCB)(void* pVoid, u_int32_t time);

#ifdef WIN32
    #define EINPROGRESS         WSAEINPROGRESS
    #define WOULDBLOCK          WSAEWOULDBLOCK
#else
    #define WOULDBLOCK          EINPROGRESS
#endif //

//---------------------------------------------------------------------------------------
class tcp_sock;

typedef struct sockaddr_in  SA_46;

//fake till migrate to 4 and 6
/*
    _remote_sin.sin_family		= AF_INET;
    _remote_sin.sin_addr.s_addr	= htonl(ip.ip4());
    _remote_sin.sin_port		= htons(port);
*/
//---------------------------------------------------------------------------------------
#ifndef IPV6

#define _IPR(asin)  (asin.sin_addr.s_addr)
#define _IP(asin)  htonl(asin.sin_addr.s_addr)
#define _PORT(asin)  htons(asin.sin_port)


//---------------------------------------------------------------------------------------
// holds in network order
//---------------------------------------------------------------------------------------
struct SADDR_46 : public SA_46
{
    SADDR_46(){clear();}
    void clear(){memset(this,0, sizeof(*this)); sin_family=AF_INET; _mask=0xFFFFFFFF;}

    SADDR_46(u_int32_t r, u_int16_t port=0, uint32_t mask=0xFFFFFFFF){
        sin_addr.s_addr = htonl(r);
        sin_port = htons(port);
        sin_family=AF_INET;
        _mask = mask;
    }
    SADDR_46(const SADDR_46& r){::memcpy(this, &r, sizeof(*this));}

    SADDR_46(const char* r, u_int16_t port=0, uint32_t mask=0xFFFFFFFF){
        char* pm = 0;
        if((pm = (char*)strchr(r,'/')) != 0) //has range ip
        {
            *pm++=0;
            sin_addr.s_addr = inet_addr(r);
            sin_port = htons(port);
            sin_family=AF_INET;
            int32_t msk = pow(2,::atoi(pm))-1;
            _mask=htonl(~msk);
        }
        else
        {
            sin_addr.s_addr = inet_addr(r);
            sin_port = htons(port);
            sin_family=AF_INET;
            _mask = mask;
        }
    }
    SADDR_46(const SA_46& sa){::memcpy(this, &sa, sizeof(*this));}
    SADDR_46& operator =(const SADDR_46& r){
        ::memcpy(this, &r, sizeof(*this));
        return *this;
    }
    u_bool32 compare_range_ip(const SADDR_46& r)const
    {
        int32_t m = _mask & r._mask;
        return ((sin_addr.s_addr&m) == (r.sin_addr.s_addr&m));
    }
    u_bool32 operator != (const SADDR_46& r)const
    {
        int32_t m = _mask & r._mask;
        if(sin_port == 0 || r.sin_port == 0)
        {
            return (sin_addr.s_addr&m) != (r.sin_addr.s_addr&m);
        }
        u_int64_t bigint = ((((u_int64_t)(sin_addr.s_addr&m)<<32)) | (u_int64_t)sin_port);
        u_int64_t bigintr = ((((u_int64_t)(r.sin_addr.s_addr&m))<<32) | (u_int64_t)r.sin_port);
        return bigint != bigintr;
    }
    u_bool32 operator == (const SADDR_46& r)const
    {
        int32_t m = _mask & r._mask;
        if(sin_port == 0 || r.sin_port == 0)
        {

             return ((sin_addr.s_addr&m) == (r.sin_addr.s_addr&m));
        }
        u_int64_t bigint = ((((u_int64_t)sin_addr.s_addr&m)<<32) | (u_int64_t)sin_port);
        u_int64_t bigintr = ((((u_int64_t)r.sin_addr.s_addr&m)<<32) | (u_int64_t)r.sin_port);
        return bigint == bigintr;
    }
    u_bool32 operator < (const SADDR_46& r)const{
        int32_t m = _mask & r._mask;
        if(sin_port == 0 || r.sin_port==0)
        {
            return (sin_addr.s_addr&m) < (r.sin_addr.s_addr&m);
        }
        u_int64_t bigint = ((((u_int64_t)sin_addr.s_addr&m)<<32) | (u_int64_t)sin_port);
        u_int64_t bigintr = ((((u_int64_t)r.sin_addr.s_addr&m)<<32) | (u_int64_t)r.sin_port);
        return bigint < bigintr;
    }
    u_bool32 operator > (const SADDR_46& r)const{
        int32_t m = _mask & r._mask;
        if(sin_port == 0 || r.sin_port==0)
        {
            return (sin_addr.s_addr & m) > (r.sin_addr.s_addr & m);
        }
        u_int64_t bigint = ((((u_int64_t)sin_addr.s_addr&m)<<32) | (u_int64_t)sin_port);
        u_int64_t bigintr = ((((u_int64_t)r.sin_addr.s_addr&m)<<32) | (u_int64_t)r.sin_port);
        return bigint > bigintr;
    }
    void set(u_int32_t r,  u_int16_t port=0){sin_addr.s_addr = htonl(r); sin_port = htons(port);}
    void set_port(u_int16_t port){sin_port = htons(port);}
    u_int32_t ip4()const{return htonl(sin_addr.s_addr);}    //ret in normal order
    u_int32_t port()const{return htons(sin_port);}          //ret in normal order
    u_bool32 empty()const{return sin_addr.s_addr==0;}

    const SADDR_46& operator>>(ofstream& o) const
    {
        char fmt[64];
        if(_mask!=0xFFFFFFFF)
        {
            int32_t bits=0;
            uint32_t m = ~_mask;
            while((m&(0x1)) == 0x1)
            {
                m>>=1;
                ++bits;
            }
            sprintf(fmt, "%d/%d:%d\n", ip4(), bits , port());
        }
        else
        {
            sprintf(fmt, "%d:%d\n", ip4(), port());
        }
        o.write(fmt, strlen(fmt));
        return *this;
    }

    SADDR_46& operator<<(ifstream& i)
    {
        std::string s;
        std::getline(i, s);
        int32_t uip=0, port=0;
        if(s.find('/'))//has range of ip's
        {
            int32_t bits;
            _mask=0xFFFFFFFF;
            sscanf(s.c_str(),"%d/%d:%d",&uip, &bits, &port);
            int32_t msk = pow(2,bits)-1;
            _mask=htonl(~msk);

        }
        else
        {
            _mask=0xFFFFFFFF;
            sscanf(s.c_str(),"%d:%d",&uip, &port);
        }
        if(uip)
            set(uip, (u_int16_t)port);
        return *this;
    }
    const char* c_str()const;

    uint32_t _mask;
};

#else

#endif

//---------------------------------------------------------------------------------------
class sock
{
public:

    static      u_bool32 DefCBCall(void*,u_int32_t);
    static void Init();
    static void Uninit();
    static char*  GetLocalIP();
    static u_bool32 CTime(void* pT, u_int32_t time);
    static SADDR_46 dnsgetip(const char* sip, char* out=0, int32_t port=0);
    static SADDR_46 sip2ip(const char* sip, u_int16_t port=0);
    static u_bool32 dnsgetname(u_int32_t uip, char* out);
    sock();
    virtual ~sock();
    virtual SOCKET  create(int32_t port, int32_t opt=0, const char* inetaddr=0);
    virtual void    destroy();
    virtual int32_t     send(const char* buff, const int32_t length, int32_t port=0, const char* ip=0  )=0;
    virtual int32_t     send(const char* buff, const int32_t length, const  SADDR_46& rsin)=0;
    virtual int32_t     send(const u_int8_t* buff, const int32_t length, int32_t port=0, const char* ip=0  )=0;
    virtual int32_t     send(const u_int8_t* buff, const int32_t length, const  SADDR_46& rsin)=0;
    virtual int32_t     receive(u_int8_t* buff, int32_t length, int32_t port=0, const char* ip=0  )=0;
    virtual int32_t     receive(u_int8_t* buff, int32_t length,  SADDR_46& rsin)=0;
    virtual int32_t     select_receive(u_int8_t* buff, int32_t length, int32_t toutms);
    virtual int32_t     receive(char* buff, int32_t length, int32_t port=0, const char* ip=0  )=0;
    virtual int32_t     receive(char* buff, int32_t length,  SADDR_46& rsin)=0;
    virtual int32_t     select_receive(char* buff, int32_t length, int32_t toutms);
    int32_t             detach(){int32_t s = _thesock; _thesock=-1; return s;}
    void            attach(int32_t s);
    int32_t             set_blocking(const u_int32_t block);
    int32_t             set_option(int32_t option, int32_t value);
    int32_t             get_option(int32_t option);
    SOCKET&         socket() {return _thesock;}
    u_bool32            isopen()const{return (int32_t)_thesock > 0;}
    int32_t             error()const{return _error;}
    u_bool32            is_blocking(){return _blocking;}
    void            pre_set(int32_t sb, int32_t rb){_buffers[0]=sb;_buffers[1]=rb;}
    void            reset(){_set = 0;};
    int32_t&            set(){return _set;};
    SADDR_46&        ip46(){return _ipfixit;}
    void            set(int32_t mask){_set|=mask;};
     SADDR_46& Rsin(){return _remote_sin;}
     SADDR_46& Lsin(){return _local_sin;}
protected:
    SOCKET          _thesock;
    int32_t             _set;
    int32_t             _error;
    int32_t             _blocking;
    int32_t             _buffers[2];
    SADDR_46         _ipfixit;
    SADDR_46	        _local_sin;	   // source
    SADDR_46	        _remote_sin;          // dest
    static  u_int32_t   _tout;
};

//---------------------------------------------------------------------------------------
class tcp_srv_sock;
class tcp_sock : public sock
{
public:
    friend class tcp_srv_sock;
    tcp_sock();
    virtual ~tcp_sock();
    virtual SOCKET  create(int32_t port, int32_t opt=0, const char* inetaddr=0);
    virtual int32_t     send(const char* buff, const int32_t length, int32_t port=0, const char* ip=0  );
    virtual int32_t     send(const char* buff, const int32_t length, const  SADDR_46& rsin){UNUS(rsin); return send(buff, length);};
    virtual int32_t     send(const u_int8_t* buff, const int32_t length, int32_t port=0, const char* ip=0  );
    virtual int32_t     send(const u_int8_t* buff, const int32_t length, const  SADDR_46& rsin){UNUS(rsin) ;return send(buff, length);}
    virtual int32_t     receive(u_int8_t* buff, int32_t length, int32_t port=0, const char* ip=0  );
    virtual int32_t     receive(u_int8_t* buff, int32_t length,  SADDR_46& rsin){UNUS(rsin); return receive(buff,length);}
    virtual int32_t     receive(char* buff, int32_t length, int32_t port=0, const char* ip=0  );
    virtual int32_t     receive(char* buff, int32_t length,  SADDR_46& rsin){UNUS(rsin); return receive(buff,length);}
    virtual int32_t     sendall(const u_int8_t* buff,  int32_t length, int32_t tout=8000);
    virtual int32_t     sendall(const char* buff, int32_t length, int32_t tout=8000){return sendall((u_int8_t*)buff, length, tout);}
    int32_t             receiveall(const u_int8_t* buff, const int32_t length);

    char*           ssock_addrip();
    char*           getsocketaddr_str(char* pAddr)const;
    const SADDR_46&       getsocketaddr()const;
    int32_t             getsocketport()const;
    int32_t             listen(int32_t maxpending=64);

protected:

    char    _sip[128];
};


//---------------------------------------------------------------------------------------
class tcp_cli_sock;
class tcp_srv_sock : public tcp_sock
{
    int32_t n_port;
public:
    int32_t port()const{return n_port;}
    tcp_srv_sock();
    virtual ~tcp_srv_sock();
    virtual void    destroy();
    SOCKET     accept(tcp_cli_sock& cliSock);
    virtual SOCKET   create(int32_t port, int32_t opt=0, const char* inetaddr=0);
};

//---------------------------------------------------------------------------------------
class tcp_cli_sock : public tcp_sock
{
public:
    tcp_cli_sock();
    tcp_cli_sock(const tcp_cli_sock& s);
    tcp_cli_sock& operator=(const tcp_cli_sock& s);
    virtual         ~tcp_cli_sock();
    virtual SOCKET  create(int32_t port, int32_t opt=0, const char* inetaddr=0);
    virtual int32_t     raw_connect(const SADDR_46& uip4, int32_t tout=0);
    virtual int32_t     raw_connect(u_int32_t ip4,  int32_t port);
    virtual int32_t     raw_connect_sin();
    int32_t             try_connect(const char* sip, int32_t port);
    void            raw_sethost(const SADDR_46& uip4);
    int32_t             openconnection(const char* sip, int32_t port);
    int32_t             connect(const char* sip, int32_t port, CancelCB cbCall=sock::DefCBCall, void* pUser=0);
    int32_t             i4connect(const SADDR_46& ip, CancelCB cbCall=sock::DefCBCall, void* pUser=0);
    int32_t             s4connect(const char* sip, int32_t port, CancelCB cbCall=sock::DefCBCall, void* pUser=0);
    u_bool32            is_really_connected();
    u_bool32            check_connection()const {return _connected;};
    struct hostent* gethostent(){return _hostent;}
    virtual void    destroy();
    int32_t             isconnecting(){
        return _connecting;
    }
    virtual void            setconnected(){
        _connecting = 0;
        _connected = 1;
    }

protected:
     hostent  *_hostent;
    int32_t  _connecting;
    int32_t  _connected;
};

//---------------------------------------------------------------------------------------
class udp_sock : public sock
{
public:
    udp_sock():_connected(0),_bind(0){}
    virtual ~udp_sock(){destroy();}
    virtual void    destroy(){sock::destroy();_connected=0;};
    virtual SOCKET  create(int32_t port, int32_t opt=0, const char* inetaddr=0);
    virtual int32_t     send(const char* buff, const int32_t length, int32_t port=0, const char* ip=0  );
    virtual int32_t     send(const char* buff, const int32_t length, const  SADDR_46& rsin);
    virtual int32_t     send(const u_int8_t* buff, const int32_t length, int32_t port=0, const char* ip=0  );
    virtual int32_t     send(const u_int8_t* buff, const int32_t length, const  SADDR_46& rsin);
    virtual int32_t     receive(u_int8_t* buff, int32_t length, int32_t port=0, const char* ip=0  );
    virtual int32_t     receive(u_int8_t* buff, int32_t length,  SADDR_46& rsin);
    virtual int32_t     receive(char* buff, int32_t length, int32_t port=0, const char* ip=0  );
    virtual int32_t     receive(char* buff, int32_t length,  SADDR_46& rsin);
    void            SetRsin(const  SADDR_46& in){::memcpy(&_remote_sin, &in, sizeof( SADDR_46));}
    int32_t             connect(const char* sip, int32_t port, CancelCB cbCall=sock::DefCBCall, void* pUser=0);
    int32_t             set_rsin(const char* sip, int32_t port);
    int32_t             bind(const char* ip=0, int32_t port=0);
    SADDR_46&          remote(){return _remote_sin;}
    void            remote(SADDR_46& s){memcpy(&_remote_sin,&s,sizeof(s));}
    char*           ssock_addrip();

protected:
    u_bool32    _connected;
    u_bool32    _bind;
    int32_t     _option;
    char    _sip[128];
};


//---------------------------------------------------------------------------------------
class udp_group_sock : public udp_sock
{
/*
    struct ip_mreq    _mcastGrp;
    u_bool32               _groupmember;

public:
    udp_group_sock(int32_t opt=0){};
    virtual ~udp_group_sock(){destroy();};
    virtual int32_t  create(int32_t opt=0);
    int32_t     join(const char* ipGrp, int32_t port);

    virtual int32_t     send(u_int8_t* buff, int32_t length, int32_t port=0, const char* ip=0  );
    virtual int32_t     receive(u_int8_t* buff, int32_t length, int32_t port=0, const char* ip=0  );
    virtual void    destroy();
    */
};

#define IS_SOCKK_ERR(err_)  err_ == WSAECONNRESET   ||\
                            err_ == WSAECONNABORTED ||\
                            err_ == WSAESHUTDOWN    ||\
                            err_ == WSAETIMEDOUT    ||\
                            err_ == WSAECONNREFUSED ||\
                            err_ == WSAEOPNOTSUPP   ||\
                            err_ == WSAENETDOWN     ||\
                            err_ == -1




class WsaInit
{
public:
    WsaInit()
    {
        sock::Init();
    }
    ~WsaInit()
    {
        sock::Uninit();
    }
};


class Ip2str
{
public:
    Ip2str(const SADDR_46& sa){
       u_int32_t dw = _IP(sa);
       ::sprintf( _s,"%u.%u.%u.%u:%d",//%04X:%04X:%04X:%04X:%04X:%04X",
                      (int32_t)(dw >> 24 & 0xFF),
                      (int32_t)((dw >>16) & 0xFF),
                      (int32_t)((dw >>8) & 0xFF),
                      (int32_t)((dw) & 0xFF),
                      htons(sa.sin_port));
    }

   Ip2str(const u_int32_t dw){
       ::sprintf( _s,"%u.%u.%u.%u:0",//%04X:%04X:%04X:%04X:%04X:%04X",
                     (int32_t)(dw >> 24 & 0xFF),
                      (int32_t)((dw >>16) & 0xFF),
                      (int32_t)((dw >>8) & 0xFF),
                      (int32_t)((dw) & 0xFF));
   }

   operator const char*(){return _s;}
private:
   char _s[128];
};
#define IP2STR(dwip_)   (const char*)Ip2str(dwip_)


#endif // !

