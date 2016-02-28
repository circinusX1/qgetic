/*
    Marius Chincisan Aug 1999. socket class
*/
#include "sock.h"
#ifndef _WIN32
#   include <ifaddrs.h>
#   include <sys/socket.h>
#   include <netdb.h>
#endif //

//-----------------------------------------------------------------------------
u_int32_t       sock::_tout = 2000;


//-----------------------------------------------------------------------------
void sock::Init()
{
#ifdef WIN32
    WSADATA     WSAData;
    WSAStartup (MAKEWORD(1,1), &WSAData);
#endif
}

SADDR_46 sock::sip2ip(const char* sip, u_int16_t port)
{
    int32_t a,b,c,d,e=port;
    if(4<=sscanf(sip,"%u.%u.%u.%u:%d",&a,&b,&c,&d,&e))
    {
        a&=0xff;
        b&=0xff;
        c&=0xff;
        d&=0xff;
        return SADDR_46(u_int32_t(a<<24|b<<16|c<<8|d), (u_int16_t)e);
    }
    return SADDR_46((u_int32_t)0, 0);
}
//-----------------------------------------------------------------------------

void sock::Uninit()
{
#ifdef WIN32
    WSACleanup();
#endif
}

//-----------------------------------------------------------------------------
u_bool32 sock::CTime(void* , u_int32_t time)
{
    return time < sock::_tout;
}

SADDR_46 sock::dnsgetip(const char* sip, char* out, int32_t port)
{
    if(isdigit(sip[0]))
    {
        int32_t a=256,b=256,c=256,d=256,e=port;

        ::sscanf(sip,"%d.%d.%d.%d:%d",&a,&b,&c,&d,&e);

        if(a<256 && b<256 && c<256 && d<256)
        {
            SADDR_46 p = sock::sip2ip(sip);
            if(p.ip4() != 0)
            {
                return p;
            }
        }
    }
    struct hostent*  hostent = ::gethostbyname(sip);
    if(hostent==0)
    {
        int32_t dwbo = inet_addr(sip);
        hostent = gethostbyaddr((char*)&dwbo, (int32_t)sizeof(dwbo), AF_INET );
    }
    if(hostent)
    {
        SADDR_46	sin;
        ::memcpy((char*)&(sin.sin_addr), hostent->h_addr, hostent->h_length);
        if(out)
        {
            ::sprintf(out,"%s", inet_ntoa((struct in_addr)sin.sin_addr));
        }
        sin.set_port(port);
        return sin;
    }
    return SADDR_46((u_int32_t)0,0);
}

u_bool32 sock::dnsgetname(u_int32_t uip, char* out)
{
    struct sockaddr_in addr;
    struct hostent *h;

    addr.sin_family=AF_INET;
    addr.sin_port=htons(80);
    addr.sin_addr.s_addr=uip;

    h=gethostbyaddr((char*)&(addr.sin_addr),
                    sizeof(struct in_addr),
                    AF_INET );

    if(h!=NULL)
    {
        strcpy(out, h->h_name);
        return true;
    }
    return false;
}


//-----------------------------------------------------------------------------
sock::sock()
{
    _thesock     = -1;
    _set         = 0;
    _error       = 0;
    _blocking    = 0;
    _buffers[0] = 0;
    _buffers[1] = 0;
};

//-----------------------------------------------------------------------------
sock::~sock()
{
    destroy();
};

void  sock::attach(int32_t s)
{
    _thesock = s;
}

//-----------------------------------------------------------------------------
int32_t sock::set_blocking(const u_int32_t block)
{
    if(isopen())
    {
        _blocking = block;
        int32_t flags = fcntl(_thesock, F_GETFL, 0);
        if(!block)
            fcntl(_thesock, F_SETFL, flags | O_NONBLOCK);
        else
            fcntl(_thesock, F_SETFL, flags & ~O_NONBLOCK);
    }
    return 0;
}

//-----------------------------------------------------------------------------
SOCKET sock::create(int32_t , int32_t , const char* )
{

    return (SOCKET)-1;
}

//-----------------------------------------------------------------------------
char* sock::GetLocalIP()
{
    static char localip[512];

    memset(localip,0,sizeof(localip));
#ifdef _WIN32
    sockaddr_in locSin = {0};
    hostent*    pHe;
    char        szBuf[256];
    u_int32_t       dwSize = __sfz(char, szBuf);

    locSin.sin_family = AF_INET;
    if(-1 == gethostname((char*)szBuf, (int32_t)dwSize))
    {
        sprintf(localip, "ghname err %d", errno);
        return localip;
    }
    pHe = gethostbyname(szBuf);
    if (pHe == NULL)
    {
        sprintf(localip, "ghbyname(%s) err %d", szBuf, errno);
        return localip;
    }
    ::memcpy((char*)&(locSin.sin_addr), pHe->h_addr,pHe->h_length);
    ::sprintf(localip,"%s", inet_ntoa((struct in_addr)locSin.sin_addr));

#else
    char    tmp[128];
    struct  ifaddrs* ifAddrStruct = 0;
    void    *tmpAddrPtr = 0;

    getifaddrs(&ifAddrStruct);
    while (ifAddrStruct != 0)
    {
        if (ifAddrStruct->ifa_addr->sa_family==AF_INET && strcmp(ifAddrStruct->ifa_name, "lo0")!=0)
        {
            tmpAddrPtr = (void*)&((SADDR_46 *)ifAddrStruct->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, tmp, (sizeof(tmp)-1));
            if(strlen(localip) + strlen(tmp) < (sizeof(localip)-1) )
            {
                ::strcat(localip, tmp);
                break;
                ::strcat(localip, ",");
            }
        }
        ifAddrStruct = ifAddrStruct->ifa_next;
    }
#endif //
    return localip;
}

//-----------------------------------------------------------------------------
int32_t sock::get_option(int32_t option)
{
    if(isopen())
    {
        int32_t optionVal;
        socklen_t optLen = sizeof(optionVal);
        if(0 == getsockopt(_thesock, SOL_SOCKET, option, (char*)&optionVal, &optLen))
            return optionVal;
    }
    return -1;
}


//-----------------------------------------------------------------------------
//err = ::setsockopt(_thesock, SOL_SOCKET, SO_SNDBUF, (char *) &bfSz, sizeof(bfSz));
int32_t sock::set_option(int32_t option, int32_t optionval)
{
    if(isopen())
        return  setsockopt (_thesock, SOL_SOCKET, option, &optionval, sizeof (optionval));
    return -1;
}

//-----------------------------------------------------------------------------
int32_t  sock::select_receive(char* buff, int32_t length, int32_t toutms)
{
    return select_receive((u_int8_t*) buff, length, toutms);
}

//-----------------------------------------------------------------------------
int32_t  sock::select_receive(u_int8_t* buff, int32_t length, int32_t toutms)
{
    timeval  tv   = {0, toutms*1000};
    fd_set   rdSet;

    _error = 0;
    FD_ZERO(&rdSet);
    FD_SET(_thesock, &rdSet);
    int32_t nfds = (int32_t)_thesock+1;
    int32_t sel = ::select(nfds, &rdSet, 0, 0, &tv);
    if(sel > 0)
    {
        int32_t bytes = receive(buff, length);
        if(0 == bytes)
        {
            return 0; //remote closed the socket
        }
        return bytes;
    }
    else if(sel < 0)
    {
        _error = errno;
        return 0; //select error. we close
    }
    return -1; // no data this time
}


tcp_sock::~tcp_sock()
{
}

//-----------------------------------------------------------------------------
char* tcp_sock::ssock_addrip()
{
    ::strcpy(_sip, inet_ntoa((struct in_addr)_remote_sin.sin_addr));
    return _sip;
}


//-----------------------------------------------------------------------------
int32_t tcp_sock::getsocketport()const
{
    return (int32_t)htons(_remote_sin.sin_port);
}

//-----------------------------------------------------------------------------
const SADDR_46& tcp_sock::getsocketaddr()const
{
    return _remote_sin;
}

//-----------------------------------------------------------------------------
char* tcp_sock::getsocketaddr_str(char* pAddr)const
{
    ::strcpy(pAddr, inet_ntoa((struct in_addr)_remote_sin.sin_addr));
    return pAddr;
}

int32_t tcp_sock::receiveall(const u_int8_t* buff, const int32_t length)
{
    int32_t shot = 0;
    int32_t toreceive = length;
    int32_t got   = 0;

    _error = 0;
    while(toreceive)
    {
        shot = receive((u_int8_t*)(buff+got), toreceive);
        if(shot <= 0)
            break;
        toreceive -= shot;
        got += shot;
    }
    return got;

}

//-----------------------------------------------------------------------------
//returns 0 for success
int32_t     tcp_sock::sendall(const u_int8_t* buff, int32_t length, int32_t tout)
{
    int32_t shot = 0;
    int32_t sent = 0;

    _error = 0;
    int32_t toutnonblock = (tout/10) + 1; //8 sec
    while(length > 0)
    {
        shot = ::send(_thesock,(char *)buff+sent, length, 0);
        if(shot <= 0)
        {
            _error = errno;
            if(_error==0)break;
            if(_error == EAGAIN || _error==11)
            {
                if(--toutnonblock < 0)
                {
                    destroy();
                    break; //
                }
                usleep(10000); // 10 milliseconds
                continue;
            }
            destroy();
            break;
        }
        length -= shot;
        sent   += shot;
    }
    return length;
}

//-----------------------------------------------------------------------------
int32_t tcp_sock::send(const char* buff, const int32_t length, int32_t , const char* )
{
    _set &= ~0x2;
    return send((u_int8_t*)buff, length, 0 , 0 );
}
//-----------------------------------------------------------------------------
int32_t tcp_sock::send(const u_int8_t* buff, const int32_t length, int32_t , const char* )
{
    _error = 0;
    assert(length);
    _set &= ~0x2;
    return ::send(_thesock,(char *)buff, length, 0);
}

//-----------------------------------------------------------------------------
int32_t tcp_sock::receive(char* buff, int32_t length, int32_t , const char* )
{
    _set &= ~0x1;
    _error = 0;
    int32_t rb = ::recv(_thesock,(char *)buff, length, 0);
    if(rb<0)
    {
        _error = errno;
        if(errno==EAGAIN || errno== EWOULDBLOCK)
        {
            return -1;
        }
        return 0;
    }
    return rb;
}

//-----------------------------------------------------------------------------
int32_t tcp_sock::receive(u_int8_t* buff, int32_t length, int32_t , const char* )
{
    _error = 0;
    _set &= ~0x1;
    int32_t rb = ::recv(_thesock,(char *)buff, length, 0);
    if(rb<0)
    {
        _error = errno;
        if(errno==EAGAIN || errno== EWOULDBLOCK)
        {
            std::cout << "sock again \n";
            return -1;
        }
        return 0;
    }
    return rb;
}

//-----------------------------------------------------------------------------
void sock::destroy()
{
    //std::cout << "destroy sock " <<_thesock<< "\n";
    _set = 0;
    if((int32_t)_thesock > 0)
    {
        int32_t k = 32;
        while(-1==::close(_thesock) && --k>0)
        {
            _error = errno;
            usleep(0xFFFF);
        }
    }
    _thesock = -1;
}

void    tcp_srv_sock::destroy()
{
    if(!isopen())   return;
    char buff[256];
    int32_t  n = 0;

    set_blocking(0);
    shutdown(_thesock, 0x3);
    do
    {
        n = recv(_thesock,(char *)buff, 256, 0);
    }
    while (n > 0);
    shutdown(_thesock, 0x3);
    sock::destroy();
}

tcp_cli_sock::tcp_cli_sock()
{
    _hostent    = 0;
    _connecting = 0;
    _connected  = 0;
    _connecting = 0;
}

tcp_cli_sock::tcp_cli_sock(const tcp_cli_sock& s):tcp_sock()
{
    _thesock    = s._thesock;
    _error      = s._error;
    _blocking   = s._blocking;
    _connected  = s._connected;
    _connecting  = s._connecting;
    ::memcpy(&_local_sin, &s._local_sin, sizeof(_local_sin));
    ::memcpy(&_remote_sin, &s._remote_sin, sizeof(_remote_sin));

}

tcp_cli_sock& tcp_cli_sock::operator=(const tcp_cli_sock& s)
{
    if(this != &s)
    {
        _thesock       = s._thesock;
        _error      = s._error;
        _blocking   = s._blocking;
        _connected  = s._connected;
        _connecting  = s._connecting;
        ::memcpy(&_local_sin, &s._local_sin, sizeof(_local_sin));
        ::memcpy(&_remote_sin, &s._remote_sin, sizeof(_remote_sin));
    }
    return *this;
}

u_bool32 tcp_cli_sock::is_really_connected()
{
    struct sockaddr saddr;
    socklen_t sz = 0;// ; = sizeof(_remote_sin);
    if(0 == getpeername(_thesock, (struct sockaddr*)&saddr , &sz))
    {
        //::memcpy(&_remote_sin, &saddr, sizeof(_remote_sin));
        _connected=1;
        return true;
    }
    _connected=0;
    return false;
}


tcp_cli_sock::~tcp_cli_sock()
{
    destroy();
}


void    tcp_cli_sock::destroy()
{
    if(!isopen())   return;
    _connecting = 0;
    _connected = 0;
    char buff[128];
    int32_t  n = 0;
    set_blocking(0);
    //std::cout << "closing socket " << _thesock << "\n";
    shutdown(_thesock, 0x3);
    do
    {
        n = recv(_thesock,(char *)buff,128, 0);
    }
    while (n > 0);
    shutdown(_thesock, 0x3);
    sock::destroy();
}

/*
//-----------------------------------------------------------------------------
const char*  tcp_cli_sock::tcp_cli_sock()
{
    ::t_sprintf(_sip,"%d.%d.%d.%d",
          (int32_t)(_remote_sin.sin_addr.s_addr&0xFF),
          (int32_t)(_remote_sin.sin_addr.s_addr&0xFF00)>>8,
          (int32_t)(_remote_sin.sin_addr.s_addr&0xFF0000)>>16,
          (int32_t)(_remote_sin.sin_addr.s_addr&0xFF000000)>>24);
    return _sip;
}
*/
//-----------------------------------------------------------------------------
tcp_sock::tcp_sock()
{
    _sip[0] = 0;
}

//-----------------------------------------------------------------------------
SOCKET tcp_sock::create(int32_t , int32_t opt, const char* )
{
    _error = 0;
    ::memset(&_local_sin,0,sizeof(_local_sin));
    ::memset(&_remote_sin,0,sizeof(_remote_sin));
    if((int32_t)_thesock != -1)
        return (SOCKET)-1;
    _thesock = ::socket(AF_INET, SOCK_STREAM, 0);
    if((int32_t)_thesock < 0)
        _error = errno;
    if(opt)
    {
        int32_t one = 1;
        ::setsockopt(_thesock, SOL_SOCKET, /*SO_REUSEADDR*/opt, (char *)&one, sizeof(one));
        if(_buffers[0] && _buffers[1])
        {
            set_option(SO_SNDBUF,_buffers[0]);
            set_option(SO_RCVBUF,_buffers[1]);
            _buffers[0] = _buffers[1] = 0;
        }
    }
    return _thesock;
}

//-----------------------------------------------------------------------------
int32_t tcp_sock::listen(int32_t maxpending)
{
    _error = 0;
    int32_t rv = ::listen(_thesock, maxpending);
    if((int32_t)-1 == rv)
    {
        _error = errno;
        return -1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
tcp_srv_sock::tcp_srv_sock() {}

tcp_srv_sock::~tcp_srv_sock() {}
//-----------------------------------------------------------------------------
SOCKET tcp_srv_sock::create(int32_t port, int32_t opt, const char* )
{
    _error = 0;
    ::memset(&_local_sin,0,sizeof(_local_sin));
    ::memset(&_remote_sin,0,sizeof(_remote_sin));
    _thesock = ::socket(AF_INET, SOCK_STREAM, 0);
    if(_thesock <= 0)
    {
        _error = errno;
        return (SOCKET)-1;
    }
    _local_sin.sin_family		= AF_INET;
    _local_sin.sin_addr.s_addr	= htonl(INADDR_ANY);
    _local_sin.sin_port		    = htons(n_port = port);
    if(::bind(_thesock,(struct sockaddr*)&_local_sin,sizeof(_local_sin)) < 0)
    {
        _error =  errno;
        destroy();
        return (SOCKET)-1;
    }
    if(opt)
    {
        int32_t one = 1;
        ::setsockopt(_thesock, SOL_SOCKET, /*SO_REUSEADDR*/opt, (char *)&one, sizeof(one));
    }
    return _thesock;
}

//-----------------------------------------------------------------------------
SOCKET tcp_srv_sock::accept(tcp_cli_sock& cliSock)
{
    _error = 0;
    socklen_t clilen = (socklen_t)sizeof(cliSock._remote_sin);
    cliSock._thesock = ::accept(_thesock,
                                (struct sockaddr*)&cliSock._remote_sin,
                                &clilen);
    if((int32_t)cliSock._thesock < 0)
    {
        _error = errno;
    }
    if(_buffers[0] && _buffers[1])
    {
        set_option(SO_SNDBUF,_buffers[0]);
        set_option(SO_RCVBUF,_buffers[1]);
    }

    //cout << "new cli sock: " << cliSock._thesock << "\n";
    return cliSock._thesock;
}



//-----------------------------------------------------------------------------
SOCKET tcp_cli_sock::create(int32_t , int32_t opt, const char* )
{
    _hostent = 0;
    _thesock = ::socket(AF_INET, SOCK_STREAM, 0);
    if(_thesock <= 0)
    {
        _error = errno;
        return -1;
    }
    if(opt)
    {
        int32_t one = 1;
        ::setsockopt(_thesock, SOL_SOCKET, /*SO_REUSEADDR*/opt, (char *)&one, sizeof(one));
    }
    return _thesock;
}


int32_t tcp_cli_sock::try_connect(const char* sip, int32_t port)
{
    sockaddr_in    locSin;

    _connecting = 1;
    _error      = 0;
    _hostent = ::gethostbyname(sip);
    if(_hostent==0)
    {
        int32_t dwbo = inet_addr(sip);
        _hostent = gethostbyaddr((char*)&dwbo, (int32_t)sizeof(dwbo), AF_INET );
    }
    if(_hostent)
    {

        ::memcpy((char*)&(locSin.sin_addr), _hostent->h_addr, _hostent->h_length);
        if((int32_t)_thesock != (int32_t)-1)
        {
            destroy();
        }
        _error = 0;
        _thesock = ::socket(AF_INET, SOCK_STREAM, 0);
        if((int32_t)_thesock < 0)
        {
            _connecting = 0;
            _error = errno;
            return _error;
        }
        //set_blocking(0);
        _remote_sin.sin_family		= AF_INET;
        _remote_sin.sin_addr.s_addr	= locSin.sin_addr.s_addr;
        _remote_sin.sin_port		= htons(port);
        //printf("%s\n",IP2STR(_remote_sin));

        if(-1 == ::connect(_thesock, (const struct sockaddr*)&_remote_sin, sizeof(_remote_sin)))
        {
            _error = errno;
            if(_error==EINPROGRESS || _error == WOULDBLOCK)
            {
                _connecting = 1;
            }
            return -1;
        }
        _connecting = 0;
        return 0;
    }
    _connecting = 0;
    return -1;
}

/*
int32_t tcp_cli_sock::raw_connect(const char* suip4, int32_t port)
{
    if(_ttatoi(suip4))
    {
        return raw_connect(htonl(inet_addr(suip4)), port);
    }
    sockaddr_in locSin;
    _hostent = ::gethostbyname(suip4);

    ::memcpy((char*)&(locSin.sin_addr), _hostent->h_addr, _hostent->h_length);
    return raw_connect(htonl(inet_addr(Ip2str(locSin))), port);
}
*/
int32_t tcp_cli_sock::raw_connect(u_int32_t ip4,  int32_t port)
{
    SADDR_46 sadr(ip4,port);
    return raw_connect(sadr);
}

int32_t tcp_cli_sock::raw_connect(const SADDR_46&  saddr, int32_t tout)
{
    _error   = 0;
    _connecting = 0;
    _error   = 0;
    _thesock = ::socket(AF_INET, SOCK_STREAM, 0);
    if((int32_t)_thesock < 0)
    {
        _error = errno;
        return _error;
    }
    if(_buffers[0] && _buffers[1])
    {
        set_option(SO_SNDBUF,_buffers[0]);
        set_option(SO_RCVBUF,_buffers[1]);
    }

    set_blocking(0);
    memcpy(&_remote_sin, &saddr, sizeof(_remote_sin));
    int32_t rv = ::connect(_thesock, (const struct sockaddr*)&_remote_sin, sizeof(_remote_sin));
    set_blocking(_blocking);
    if(-1 == rv)
    {
        _error = errno;
        if(_error==EINPROGRESS || _error == WOULDBLOCK)
        {
            if(tout)
            {
                fd_set  fdWr;
                time_t  t = time(0);
                while(time(0) - t < tout)
                {

                    FD_ZERO(&fdWr);
                    FD_SET((u_int32_t)_thesock, &fdWr);
                    int32_t     nfds = (int32_t)_thesock+1;
                    timeval tv{0,0xFFF};

                    nfds = ::select(nfds,0,&fdWr,0,&tv);
                    if(nfds > 0)
                    {
                        if(FD_ISSET(_thesock, &fdWr))
                        {
                            set_blocking(_blocking);
                            return 0;   // no error
                        }
                    }
                    usleep(0xFFF);
                    FD_ZERO(&fdWr);
                }
            }
            _connecting=1;
            return 0;
        }
        return -1;//error
    }
    return 0; //connected
}

void      tcp_cli_sock::raw_sethost(const SADDR_46& uip4)
{
    memcpy(&_remote_sin, (SA_46*)&uip4, sizeof(_remote_sin));
}

int32_t     tcp_cli_sock::raw_connect_sin()
{
    _error = 0;
    _connecting = 0;
    _error = 0;
    _thesock = ::socket(AF_INET, SOCK_STREAM, 0);
    if((int32_t)_thesock < 0)
    {
        _error = errno;
        return _error;
    }
    if(_buffers[0] && _buffers[1])
    {
        set_option(SO_SNDBUF,_buffers[0]);
        set_option(SO_RCVBUF,_buffers[1]);
        _buffers[0] = _buffers[1] = 0;
    }
    _connecting = 0;
    const int32_t bloking = _blocking;
    set_blocking(0);
    int32_t rv = ::connect(_thesock, (const struct sockaddr*)&_remote_sin, sizeof(_remote_sin));
    set_blocking(bloking);
    if(-1 == rv)
    {
        _error = errno;
        if(_error==EINPROGRESS || _error == WOULDBLOCK)
        {
            _connecting = 1;
            return 0;
        }
        return -1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
int32_t tcp_cli_sock::i4connect(const SADDR_46&  addr, CancelCB cbCall, void* pUser)
{
    int32_t             err;
    fd_set          fdWr;


    if((int32_t)_thesock != (int32_t)-1)
    {
        destroy();
    }
    _error = 0;
    _thesock = ::socket(AF_INET, SOCK_STREAM, 0);
    if((int32_t)_thesock < 0)
    {
        _error = errno;
        return _error;
    }
    memcpy(&_remote_sin, &addr, sizeof(_remote_sin));

    if(_hostent==0)
    {
        _hostent = gethostbyaddr((char*)&addr, (int32_t)sizeof(int32_t), AF_INET );
    }

    // non blocking node couse we can cancel it by Cancel f_call
    if(0==cbCall) cbCall = sock::DefCBCall;

    set_blocking(0);
    time_t  ti = time(0);
    _connecting = 0;
    err = ::connect(_thesock, (const struct sockaddr*)&_remote_sin, sizeof(_remote_sin));
    if(err==-1 )
    {
        _error = errno;
    }
    if(_error==EINPROGRESS || _error == WOULDBLOCK)
    {
        _connecting = 1;
        while(cbCall(pUser, time(0)-ti))
        {
            int32_t     nfds = (int32_t)_thesock+1;
            timeval tv = {1, 0};

            FD_ZERO(&fdWr);
            FD_SET((u_int32_t)_thesock, &fdWr);
            err = ::select(nfds,0,&fdWr,0,&tv);
            if(err > 0)
            {
                if(FD_ISSET(_thesock, &fdWr))
                {
                    set_blocking(_blocking);
                    _connecting = 0;
                    return 0;   // no error
                }
            }
            usleep(10000);
            FD_ZERO(&fdWr);
        }
    }
    if(err==-1)
    {
        destroy();
        return -1;
    }
    _connecting = 0;
    return 0;
}

//-----------------------------------------------------------------------------
int32_t tcp_cli_sock::s4connect(const char* sip, int32_t port, CancelCB cbCall, void* pUser)
{
    int32_t             err;
    fd_set          fdWr;
    timeval         tv = {1, 0};

    if(0==cbCall)
        cbCall = sock::DefCBCall;

    if((int32_t)_thesock != (int32_t)-1)
    {
        destroy();
    }
    _error = 0;
    _thesock = ::socket(AF_INET, SOCK_STREAM, 0);
    if((int32_t)_thesock < 0)
    {
        _error = errno;
        return _error;
    }
    if(0==_hostent)
        _hostent = ::gethostbyname(sip);
    _remote_sin.sin_family		= AF_INET;
    _remote_sin.sin_port		= htons(port);
#ifdef _WIN32
    _remote_sin.sin_addr.s_addr	= inet_addr(sip);
#else
    inet_aton(sip, &_remote_sin.sin_addr);
#endif //
    set_blocking(0);
    _connecting = 0;
    err = ::connect(_thesock, (const struct sockaddr*)&_remote_sin, sizeof(_remote_sin));
    if(err==-1)
    {
        _error = errno;
    }
    if(_error == EINPROGRESS || _error == WOULDBLOCK)
    {
        _connecting = 1;

        if(pUser==(void*)-1)
        {
            return 0;
        }

        int32_t nfds = (int32_t)_thesock+1;
        time_t     ti = time(0);
        int32_t tdiff = time(0)-ti;
        while(cbCall(pUser, tdiff))
        {
            FD_ZERO(&fdWr);
            FD_SET((u_int32_t)_thesock, &fdWr);
            tv.tv_sec = 1;
            err = ::select(nfds,0,&fdWr,0,&tv);
            if(err > 0)
            {
                if(FD_ISSET(_thesock, &fdWr))
                {
                    set_blocking(1);
                    _connecting = 0;
                    return 0;   // no error
                }
            }
            usleep(10000);
            tdiff = time(0)-ti;
        }
    }
    if(err==-1)
    {
        _connecting = 0;
        destroy();
        return -1;
    }
    _connecting = 0;
    return 0;
}

//-----------------------------------------------------------------------------
int32_t tcp_cli_sock::connect(const char* sip, int32_t port, CancelCB cbCall, void* pUser)
{
    _connecting = 1;
    _error = 0;
    sockaddr_in    locSin;
    _hostent = ::gethostbyname(sip);
    if(_hostent==0)
        _hostent = gethostbyaddr(sip, (int32_t)strlen(sip), AF_INET );
    if(_hostent)
    {
        ::memcpy((char*)&(locSin.sin_addr), _hostent->h_addr, _hostent->h_length);
        return s4connect(Ip2str(locSin.sin_addr.s_addr),
                         port, cbCall, pUser);
    }
    _connecting = 0;
    _error = errno;
    return -1;
}


//-----------------------------------------------------------------------------
u_bool32 sock::DefCBCall(void*, u_int32_t dw)
{
    return dw < sock::_tout;
}

//-----------------------------------------------------------------------------
SOCKET udp_sock::create(int32_t port, int32_t proto, const char* addr)
{
    _error    = 0;
    _thesock = ::socket(AF_INET, SOCK_DGRAM, proto);
    if((int32_t)-1 == (int32_t)_thesock)
        return -1;
    ::memset(&_local_sin, 0, sizeof(_local_sin));

    _local_sin.sin_family        = AF_INET;
    _local_sin.sin_addr.s_addr   = addr ? inet_addr(addr): htonl(INADDR_ANY);
    _local_sin.sin_port          = htons(port);
    return _thesock;
}

int32_t  udp_sock::bind(const char* addr, int32_t port)
{
    if(addr)
        _local_sin.sin_addr.s_addr = inet_addr(addr);
    if(port)
        _local_sin.sin_port = htons(port);

    assert(_local_sin.sin_port > 0); //did you pass in at create the addr and port
    if(::bind(_thesock,(struct sockaddr *)&_local_sin, sizeof(_local_sin)))
    {
        printf("udp-sock-bind-error\n");
        perror("bind error \n");
        _error = errno;
        return -1;
    }
    return _thesock;
}

//-----------------------------------------------------------------------------
int32_t  udp_sock::send(const char* buff, const int32_t length, int32_t port, const char* ip)
{
    return send((const u_int8_t*)buff, length, port, ip);
}

//-----------------------------------------------------------------------------
int32_t  udp_sock::send(const u_int8_t* buff, const int32_t length, int32_t port, const char* ip)
{
    int32_t snd = -1;
    _error  = 0;

    if(_connected)
    {
        snd = ::send(_thesock,(char *)buff, length, 0);
    }
    else if(_bind)
    {
        ////printf("<-to %s : %d\n", IP2STR(_remote_sin.sin_addr.s_addr), htons(_remote_sin.sin_port));

        snd = ::sendto(_thesock, (char*)buff, length, 0,
                       (struct sockaddr  *) &_remote_sin,
                       sizeof (_remote_sin)) ;
    }
    else
    {
        _remote_sin.sin_port        = htons (port);
        _remote_sin.sin_family      = AF_INET;
        if(ip)
            _remote_sin.sin_addr.s_addr = inet_addr(ip); // direct
        else
            _remote_sin.sin_addr.s_addr = inet_addr("255.255.255.255");

        ////printf("<-to %s : %d\n", IP2STR(_remote_sin.sin_addr.s_addr), htons(_remote_sin.sin_port));

        snd = ::sendto(_thesock, (char*)buff, length, 0,
                       (struct sockaddr  *) &_remote_sin,
                       sizeof (_remote_sin)) ;
    }
    if(-1 == snd)
    {
        _error = errno;
    }
    return snd;
}

//-----------------------------------------------------------------------------
int32_t  udp_sock::send(const char* buff, const int32_t length, const SADDR_46& rsin)
{
    return send((const u_int8_t*) buff, length, rsin);
}

//-----------------------------------------------------------------------------
int32_t  udp_sock::send(const u_int8_t* buff, const int32_t length, const SADDR_46& rsin)
{
    int32_t snd ;
    _error = 0;
    if(_connected)
    {
        snd = ::send(_thesock,(char *)buff, length, 0);
    }
    else
    {
        ////printf("<-to %s : %d\n", IP2STR(rsin.sin_addr.s_addr), htons(rsin.sin_port));
        snd = ::sendto(_thesock, (char*)buff, length, 0,
                       (struct sockaddr  *) &rsin,
                       sizeof (rsin)) ;
    }
    if(-1 == snd)
    {
        _error = errno;
    }
    return snd;
}



//-----------------------------------------------------------------------------
int32_t  udp_sock::receive(char* buff, int32_t length, SADDR_46& rsin)
{
    return receive((u_int8_t*) buff, length, rsin);
}

//-----------------------------------------------------------------------------
int32_t  udp_sock::receive(u_int8_t* buff, int32_t length, SADDR_46& rsin)
{
    int32_t rcv;
    if(_connected)
    {
        rcv = ::recv(_thesock,(char *)buff, length, 0);
    }
    else
    {
        socklen_t iRecvLen=(socklen_t)sizeof(rsin);
        rcv =  (int32_t)::recvfrom (_thesock,
                                (char*)buff,
                                length,
                                0,
                                (struct sockaddr  *) &rsin,
                                &iRecvLen);
    }
    if(rcv==-1)
    {
        _error = errno;
    }
    return rcv;
}

//-----------------------------------------------------------------------------
int32_t  udp_sock::receive(char* buff, int32_t length, int32_t port, const char* ip)
{
    if(_bind)
    {
        socklen_t iRecvLen=(socklen_t)sizeof(_remote_sin);
        int32_t rcv =  (int32_t)::recvfrom (_thesock,
                                    (char*)buff,
                                    length,
                                    0,
                                    (struct sockaddr  *) &_remote_sin,
                                    &iRecvLen);
        return rcv;
    }
    return receive((u_int8_t*)buff, length, port, ip);
}

//-----------------------------------------------------------------------------
int32_t  udp_sock::receive(u_int8_t* buff, int32_t length, int32_t port, const char* ip)
{
    _error  = 0;
    int32_t rcv ;
    if(_connected)
    {
        rcv = ::recv(_thesock,(char *)buff, length, 0);
    }
    else
    {
        if(ip)
        {
            _remote_sin.sin_addr.s_addr = inet_addr(ip);
            _remote_sin.sin_port        = htons (port);
            _remote_sin.sin_family      = AF_INET;
            socklen_t iRecvLen                = sizeof(_remote_sin);
            rcv =  (int32_t)recvfrom (_thesock,
                                  (char*)buff, length,
                                  0,
                                  (struct sockaddr  *) &_remote_sin,
                                  &iRecvLen);

        }
        else
        {
            memset(&_remote_sin,0,sizeof(_remote_sin));
            socklen_t iRecvLen   = sizeof(_remote_sin);
            rcv =  (int32_t)recvfrom (_thesock, (char*)buff, length,
                                  0,
                                  (struct sockaddr  *) &_remote_sin,
                                  &iRecvLen);

        }
    }

    if(rcv==-1)
    {
        _error = errno;
    }
    return rcv;
}

//-----------------------------------------------------------------------------
char* udp_sock::ssock_addrip()
{
    ::strcpy(_sip, inet_ntoa((struct in_addr)_remote_sin.sin_addr));
    return _sip;
}


int32_t  udp_sock::set_rsin(const char* sip, int32_t port)
{
    _connected      = 0;

    _remote_sin.sin_family		= AF_INET;
    _remote_sin.sin_addr.s_addr	= inet_addr(sip);
    _remote_sin.sin_port		= htons(port);
    _bind = true;
    return 0;
}


int32_t  udp_sock::connect(const char* sip, int32_t port, CancelCB cbCall, void* pUser)
{
    int32_t             err;
    fd_set          fdWr;
    timeval         tv = {1, 0};
    _connected      = 0;

    _remote_sin.sin_family		= AF_INET;
    _remote_sin.sin_addr.s_addr	= inet_addr(sip);
    _remote_sin.sin_port		= htons(port);
    memset(&_remote_sin.sin_zero, 0, sizeof(_remote_sin.sin_zero));

    // non blocking node couse we can cancel it by Cancel f_call
    set_blocking(0);
    time_t         ti = time(0);
    err = ::connect(_thesock, (const struct sockaddr*)&_remote_sin, sizeof(_remote_sin));
    if(0==cbCall)
        cbCall = sock::DefCBCall;

    while(cbCall(pUser, time(0)-ti))
    {
        FD_ZERO(&fdWr);
        FD_SET((u_int32_t)_thesock, &fdWr);
        int32_t nfds = (int32_t)_thesock+1;

        err = ::select(nfds,0,&fdWr,0,&tv);
        if(err > 0)
        {
            if(FD_ISSET(_thesock, &fdWr))
            {
                set_blocking(1);
                _connected = 1;
                return 0;   // no error
            }
        }
        usleep(10000);
    }
    destroy();
    return -1;
}


//-----------------------------------------------------------------------------
/*
// ip range 224.0.0.0 to 238.255.255.255 as
SOCKET udp_group_sock::create(int32_t opt)
{
    _error = 0;
    ::memset(&_mcastGrp,0,sizeof(_mcastGrp));
    _groupmember = FALSE;

    // from server
    _local_sin.sin_family      = AF_INET;
    _local_sin.sin_port        = htons (0);
    _local_sin.sin_addr.s_addr = htonl (INADDR_ANY);

    if (::bind (_thesock, (struct sockaddr*) &_local_sin,
              sizeof (_local_sin)) == -1)
    {
        _error =  errno;
        destroy();
        return -1;
    }
    int32_t iOptVal = opt;  // time to live (in the net)
    if (setsockopt (_thesock, IPPROTO_IP, IP_MULTICAST_TTL,
                    (char  *)&iOptVal, sizeof (int32_t)) == -1)
    {
        _error =  errno;
        destroy();
        return -1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
int32_t udp_group_sock::send(const u_int8_t* buff, int32_t length, int32_t port, const char* ipGrp)
{
    int32_t snd;

    _error = 0;
    _remote_sin.sin_family = AF_INET;
    _remote_sin.sin_port = htons (port);
    _remote_sin.sin_addr.s_addr = inet_addr(ipGrp);
    snd = ::sendto (_thesock, (char*)buff, length, 0,
                    (struct sockaddr  *) &_remote_sin, sizeof (_remote_sin)) ;
    if(snd < 0)
    {
        _error = errno;
    }
    return snd;
}

//-----------------------------------------------------------------------------
int32_t udp_group_sock::receive(u_int8_t* pbuff, int32_t length, int32_t port, const char* ipGrp)
{
    _error = 0;
    int32_t iRecvLen = sizeof (_remote_sin);
    int32_t i =  (int32_t)recvfrom (_thesock, (char*)pbuff, length, 0,
                            (struct sockaddr  *) &_remote_sin, &iRecvLen);
    if(i==-1)
    {
        _error = errno;
    }
    return i;
}

//-----------------------------------------------------------------------------
void    udp_group_sock::destroy()
{
    if(_groupmember)
    {
        _error = ::setsockopt (_thesock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                              (char  *)&_mcastGrp, sizeof (_mcastGrp)) ;
    }
    udp_sock::destroy();
}

//-----------------------------------------------------------------------------
int32_t udp_group_sock::join(const char* ipGrp, int32_t port)
{
    if(_thesock != -1)
        return -1;

    _error = 0;
    _local_sin.sin_family = AF_INET;
    _local_sin.sin_port = htons (port);
    _local_sin.sin_addr.s_addr = htonl (INADDR_ANY);
    int32_t one = 1;
    ::setsockopt(_thesock, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));
    int32_t bnd = ::bind (_thesock, (struct sockaddr  *) &_local_sin, sizeof (_local_sin));
    if (bnd == -1)
    {
        _error =  errno;
        destroy();
        return bnd;
    }

    //join the multicast group
    _mcastGrp.imr_multiaddr.s_addr = inet_addr (ipGrp);
    _mcastGrp.imr_interface.s_addr = INADDR_ANY;
    bnd = ::setsockopt (_thesock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char  *)&_mcastGrp, sizeof (_mcastGrp)) ;
    if (bnd == -1)
    {
        _error =  errno;
        destroy();
        return bnd;
    }
    _groupmember = TRUE;
    return 0;
}
*/

const char* SADDR_46::c_str()const
{
    static char nothreadsafe[32];
    ::strcpy(nothreadsafe, IP2STR(*this));
    return nothreadsafe;
}
