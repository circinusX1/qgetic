// --------------------------------------------------------------------------------------
// Copyright (c) 2000-2005 Zalsoft Inc
// Copyright: zalsoft Inc
// --------------------------------------------------------------------------------------

#ifndef __BASECONT_H__
#define __BASECONT_H__

#include <baselib.h>
#include <baseutils.h>
#include <set>


using namespace std;
//------------------------------------------------------------------------------------
/*
#define _XIOSBASE_
#define _XLOCNUM_
#define _STREAMBUF_
#define _IOS_
#define _OSTREAM_
#define _ISTREAM_
#define _IOSTREAM_
*/
//------------------------------------------------------------------------------------
#include <sys/socket.h>
#include <resolv.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <fstream>
#include <math.h>

#include <iostream>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <deque>
using namespace std;

//------------------------------------------------------------------------------------

typedef int32_t (*PFNSORT)(const void* e1, const void* e2);


class el_arr32{
public:
    int32_t _index;
};

template <class T, int32_t sizesz = 1024>
class array_t
{
public:
    array_t():_elems(0){}
    ~array_t(){}
	void ClearPtr(){ while(--_elems>=0){delete t[_elems]; t[_elems]=0;}_elems=0;}
	//T& operator[](int32_t idx){return t[idx];}
    T operator[](int32_t idx)const{return t[idx];}
	T At(int32_t idx)const{return t[idx];}
	void Set(T& tt, int32_t idx){assert(idx < _elems);t[idx] = tt;tt->_index =idx;}
	T Push(T el){if(_elems < sizesz){el->_index = _elems;t[_elems] = el; return t[_elems++];}return 0;}
    T Pop(){return t[--_elems];}
	T* GetBuff(){return t;}
	void Remove(T pt){
		assert(_elems>0);
		int32_t index = pt->_index;
		if(index != _elems-1)
		{
			t[index] = t[_elems-1];
			t[index]->_index = index;
		}
		_elems--;
	}

	void RemoveIdx(int32_t idx){
		assert(_elems>0);
		if(idx != _elems-1)
		{
			t[idx] = t[_elems-1];
			t[idx]->_index = idx;
		}
		_elems--;
	}
	void Reset(){
		_elems=0;
	}
    INLN int32_t Count()const {
		return _elems;
	}
	int32_t ElemSize(){
		return sizeof(T);
	}
    void FullSort(PFNSORT sfn){
        qsort((void*)t, _elems, ElemSize(), sfn);
    }
    u_bool32 PartialSort(PFNSORT sfn){
        for(int32_t i=1; i<_elems; i++){
            if(sfn(t[i-1], t[i]))
            {REAL
                SWITCHI((uint32_t)t[i-1],(uint32_t)t[i]);
                return true;
            }
        }
        return false;
    }
protected:
	T	t[sizesz];
	int32_t _elems;
};

//------------------------------------------------------------------------------------
template <class T, int32_t sizesz = 128>
class NO_VT raw_array_t
{
public:
    raw_array_t():_elems(0){}
    ~raw_array_t(){DeleteElements();}
	void DeleteElements(){}
	T& operator[](int32_t idx){return t[idx];}
	T& At(int32_t idx){ return t[idx];}
	int32_t     Set(T& tt , int32_t idx){assert(idx < _elems); t[idx] = tt;}
    int32_t     Push(T el){if(_elems < sizesz) {t[_elems++] = el; return (_elems-1);} return -1;}
    T&      Pop(){return t[--_elems];}
    T&      Last(){assert(_elems);return t[_elems-1];}
    T&      NextLast(){return t[_elems];}
	T*      GetBuff(){return t;}
	void Remove(int32_t idx){assert(_elems);t[idx] = t[_elems-1];_elems--;}
	void Remove(T& e){for(int32_t i=0;i<_elems;i++){if(t[i] == e){Remove(i);return;}}}
    void Reset(){_elems=0;}
    INLN int32_t Size(){return _elems;}
	int32_t ElemSize(){return sizeof(T);}
    void Increment(){++_elems;}
protected:
	T	t[sizesz];
	int32_t _elems;
};


//------------------------------------------------------------------------------------
template <class T> class pool_t
{
public:
    static void CreatePool(uint32_t cap, u_bool32 threadSafe=0)
	{
        if(0 != threadSafe)_cs = new mutex();
        else               _cs = 0;
		_pvect = new vector<T*>();
		_pvect->reserve(cap);
		_pbhead = (T*) ::malloc(cap * sizeof(T));
        _nCapacity = cap;
		T* pw	= _pbhead;
        for(uint32_t i=0; i< cap; i++){
			_pvect->push_back(pw++);
		}
	}

	static void DestroyPool(){
		if(0==_pvect)return;
		_pvect->clear();
		delete _pvect;
		::free((void*)_pbhead);
		_pbhead=0;
        delete _cs;
	}

	void* operator new(size_t sz)
	{
		if(T::_pvect->size() > 0)
        {
            AutoLock q(_cs);
			int32_t szisz = _pvect->size();
			T* pvb = (T*)_pvect->back();
			_pvect->pop_back();
			// szisz = _pvect->size();
            ++pool_t<T>::_inUse;
			return (void*)(pvb);
		}
        return 0;
	}

	void operator delete(void* pv){
        AutoLock q(_cs);
        --pool_t<T>::_inUse;
		_pvect->push_back((T*)pv);
        // int32_t szisz = _pvect->size();
	}
    static int32_t Capacity(){
        return _nCapacity;
    }
    static int32_t Size(){
        return _inUse;
    }
	static T*			_pbhead;
	static vector<T*>*	_pvect;
    static uint32_t		    _nCapacity;
    static uint32_t		    _inUse;
    static mutex       *_cs;
};

template <class T> T*				pool_t<T>::_pbhead;
template <class T> vector<T*>*      pool_t<T>::_pvect;
template <class T> uint32_t				pool_t<T>::_nCapacity;
template <class T> uint32_t				pool_t<T>::_inUse;
template <class T> mutex*  		    pool_t<T>::_cs;



//------------------------------------------------------------------------------------
template <class T> class DPool // dynamic pool gets from heap when out of objs
{
public:
    class U : public T
    {
    public:
        U():_polled(0){};
        virtual ~U(){};
        int32_t     _polled;
    };
public:
    static void CreatePool(uint32_t cap, u_bool32 threadSafe=0)
	{
        if(0 != threadSafe)_cs = new mutex();
        else               _cs = 0;
		_pvect = new vector<U*>();
		_pvect->reserve(cap);
		_pbhead = (U*) ::malloc(cap * sizeof(U));
        _nCapacity = cap;
		U* pw	= _pbhead;
        for(uint32_t i=0; i< cap; i++){
			_pvect->push_back(pw++);
		}
	}

	static void DestroyPool(){
		if(0==_pvect)return;
		_pvect->clear();
		delete _pvect;
		::free((void*)_pbhead);
		_pbhead=0;
        delete _cs;
	}

	void* operator new(size_t sz)
	{
		if(T::_pvect->size() > 0)
        {
            AutoLock q(_cs);
			int32_t szisz = _pvect->size();
			U* pvb = (U*)_pvect->back();
			_pvect->pop_back();
			++DPool<T>::_inUse;
            ((U*)pvb)->_polled = szisz;
			return (void*)(pvb);
		}
        else
        {
            U* pu = ::new U();
            pu->_polled=-1;
            return pu;
        }
	}

	void operator delete(void* pv){
        if( ((U*)pv)->_polled !=-1)
        {
            AutoLock q(_cs);
            --DPool<T>::_inUse;
		    _pvect->push_back((U*)pv);
        }
        else
            delete pv;
	}
    static int32_t Capacity(){
        return _nCapacity;
    }
    static int32_t Size(){
        return _inUse;
    }
	static U*			_pbhead;
	static vector<U*>*	_pvect;
    static uint32_t		    _nCapacity;
    static uint32_t		    _inUse;
    static mutex       *_cs;
};

template <class T> typename DPool<T>::U*            DPool<T>::_pbhead;
template <class T> vector<typename DPool<T>::U*>*   DPool<T>::_pvect;
template <class T> uint32_t				                DPool<T>::_nCapacity;
template <class T> uint32_t				                DPool<T>::_inUse;
template <class T> mutex*  		                    DPool<T>::_cs;

//------------------------------------------------------------------------------------
#ifdef WIN32
#   pragma warning (disable:4786)
#endif
template <class X> class counted_ptr
{
public:
    explicit counted_ptr(X* p = 0) : _c(0) {if (p) _c = new cnt(p);}
    ~counted_ptr(){dec();}
    counted_ptr(const counted_ptr& r) throw(){add(r._c);}
    counted_ptr& operator=(const counted_ptr& r){if (this != &r){dec(); add(r._c);} return *this;}
    X& operator*()  const throw()   {return *_c->p;}
    X* operator->() const throw()   {return _c->p;}
private:
    struct cnt {
        cnt(X* p = 0, uint32_t c = 1) : p(p), c(c) {}
        X*          p;
        uint32_t        c;
    }* _c;
    void add(cnt* c) throw(){_c = c;if (c) ++c->c;}
    void dec(){ if (_c) {if (--_c->c == 0) {delete _c->p; delete _c;}_c = 0;}}
};

#ifdef WIN32
#   pragma warning (disable: 4786)
#endif
//------------------------------------------------------------------------------------
template <class T> struct RawBuff
{
    T*      _ptr;
    size_t  _size;
    RawBuff():_ptr(0),_size(0){}
	RawBuff(const RawBuff& r){
		_size = r._size;
		_ptr = new T[_size];
        ::memcpy(_ptr,r._ptr,_size*sizeof(T));
	}
    ~RawBuff(){
		delete[] _ptr;
		_ptr=0;
		_size=0;
	}
	RawBuff<T>& operator= (const RawBuff<T>& r){
		Destroy();
		_size = r._size;
		_ptr = new T[_size];
        ::memcpy(_ptr,r._ptr,_size*sizeof(T));
		return *this;
	}
	void Destroy(){
		delete[] _ptr;
		_ptr=0;
		_size=0;
	}
	T* operator&(){
		return _ptr;
	}

    T& operator[](size_t i){
		return _ptr[i];
	}

    void ByteReserve(size_t sz){
		_size = sz/sizeof(T);
		_ptr = new T[_size];
       // ::memset(_ptr,0,_size);
	}
    void ObjReserve(size_t sz){
		_size = sz;
        _ptr = new T[_size];
		size_t sizeoft = sizeof(T);
       // ::memset(_ptr, 0, sizeoft);
	}
    size_t Size(){return _size;}
    size_t ByteSize(){return _size*sizeof(T);}
};

//------------------------------------------------------------------------------------
template <class T, size_t SZ> class RawRing
{
public:
    RawRing(){
        _p   = new T[SZ];
        _gp  = _pp = _cnt = 0;
    }
    ~RawRing(){
        delete[] _p;
    }
    void Put(T& t){
        _p[_pp]=t;
        _pp = ++_pp % SZ;
        if(_gp == _pp)
            _gp = ++_gp % SZ;
        else
            ++_cnt;
    }
    T* Get(){
        if(_cnt){
            T* pret = &_p[_gp];
            _gp = ++_gp % SZ;
            --_cnt;
            return pret;
        }
        return 0;
    }
    int32_t GetTail(){return _pp;}
    int32_t GetHead(){return _gp;}
    T*  GetAt(int32_t i){return &_p[_gp];}
    int32_t Size(){return _cnt;}
    void Reset(){_gp=_pp=0;_cnt=0;}
private:
    T*  _p;
    int32_t _gp;    // get position
    int32_t _pp;    // put position
    int32_t _cnt;   // elems in riung
};

//------------------------------------------------------------------------------------
template <class T, class CLS, class CB, class PAR>
void forall(T& coll, CLS* pC, CB cbc, PAR& par)
{
    TYPENAME T::iterator b = coll.begin();
    TYPENAME T::iterator e = coll.end();
    for(;b!=e;b++){
		if(false == ((pC)->*cbc)(*b, par))
            break;
    }
}

//------------------------------------------------------------------------------------
template <class T, class CB, class PAR>
void forall(T& coll,CB cbc,PAR* pd=0)
{
    TYPENAME T::iterator b = coll.begin();
    TYPENAME T::iterator e = coll.end();
    for(;b!=e;b++){
			if(false == (cbc)(*b,pd))
                break;
    }
}

//------------------------------------------------------------------------------------
template <class T, class CLS, class CB, class PAR, class PAR1>
void forall2(T& coll, CLS* pC, CB cbc, PAR& par, PAR1& par1)
{
    TYPENAME T::iterator b = coll.begin();
    TYPENAME T::iterator e = coll.end();
    for(;b!=e;b++){
		if(false == ((pC)->*cbc)(*b, par, par1))
            break;
    }
}

//------------------------------------------------------------------------------------
template <class T, class U> void append(T& a, U& b){
	FOREACH(TYPENAME U,b,pb) {
		a.push_back(*pb);
	}
}

//------------------------------------------------------------------------------------
template <class T, class P>
void appendif(T& a, T& b, P p){
	FOREACH(TYPENAME T,b,pb) {
        if((p)(*pb))
		    a.push_back(*pb);
	}
}

//------------------------------------------------------------------------------------
template <class T>void reverse(T& t){
	static T ret;
	ret.clear();
	ret =  t;
	t.clear();
	TYPENAME T::reverse_iterator b = ret.rbegin();
    TYPENAME T::reverse_iterator e = ret.rend();
    for(;b!=e;b++)
		t.push_back(*b);
}

//---------------------------------------------------------------------------------------
template <class T> class vvector : public vector<T>
{
public:
    typedef TYPENAME vector<T>::iterator vit;

	vvector(){_els=0;}
	vvector(int32_t sz){
	    _els=0;
	    this->vector<T>::reserve(sz);
	    }
	void push_back(const T& o){
		this->vector<T>::push_back(o);
		refresh();
	}
	void clear(){
		vector<T>::clear();
		_els=this->vector<T>::size();
	}
	void	 pop_back(){
		vector<T>::pop_back();
		refresh();
	}

    T& nextr(int32_t  idx){ //rw
        if(idx==this->vector<T>::size()-1)
            return this->vector<T>::at(0);
        return this->vector<T>::at(idx+1);
    }

    T& prevr(int32_t  idx){ //rw
        if(idx==0)
            return this->vector<T>::back();
        return this->vector<T>::at(idx-1);
    }

	vit erase( vit _P){
		vit it = this->vector<T>::erase(_P);
		refresh();
		return it;
	}
	void deleteelements(){
		FOREACH(TYPENAME vector<T>,(*this),pp)
			delete (*pp);
		clear();
        refresh();
	}
	void deleteelementsarr(){
		FOREACH(TYPENAME vector<T>,(*this),pp)
			delete[] (*pp);
		clear();
        refresh();
	}
    
    u_bool32 has(T& o){
        return findelement(o)!=this->vector<T>::end();
    }

    size_t getindex(T& o){
        vit it = findelement(o);
        if(it != this->vector<T>::end())
            return it-this->vector<T>::begin();
        return -1;
    }

    vit findelement(const T& o){
        return find(this->vector<T>::begin(),this->vector<T>::end(),o);
    }
    vvector<T>& operator <<(const T& o){
        this->push_back(o);
        return *this;
    }
    void refresh(){
        _els=this->vector<T>::size();
    };
    void destroy()
    {
        clear();
        _Destroy(this->vector<T>::begin(), this->vector<T>::end());
    }

    int32_t push_unique(T& o, u_int32_t* idx=0){
        return push_unique((const T&)o, (size_t*) idx);
    }

    int32_t push_unique(const T& o, u_int32_t* idx=0){
        return push_unique(o, (size_t*) idx);
    }

    int32_t push_unique(const T& o, size_t* idx=0){
        if(!this->vector<T>::size())
        {
            this->push_back(o);
            refresh();
            if(idx)*idx=0;
            return 1;
        }
        else
        {
            vit el = findelement(o);
            if( el == this->vector<T>::end()){
                if(idx)* idx=this->vector<T>::size();
                this->vector<T>::push_back(o);
                refresh();
                return 1;
            }
            if(idx)*idx=el-this->vector<T>::begin();
        }
        return 0;
    }
    int32_t erase_if( T& o){
        vit fi = findelement(o);
        if(fi  != this->vector<T>::end()){
            this->erase(fi);
            refresh();
            return 1;
        }return 0;
    }
    void reverse()
    {
        vvector<T>   rev = *this;
        this->vector<T>::clear();
        TYPENAME vector<T>::reverse_iterator b = rev.rbegin();
        TYPENAME vector<T>::reverse_iterator e = rev.rend();
        for(;b!=e;b++)
        {
            (*this) <<(*b);
        }
    }
private:
	size_t		_els;
};

//-----------------------------------------------------------------------------
template <class T> T* element(T* table2D, int32_t x, int32_t y, int32_t xM)
{
    return (table2D + (y * xM + x));
}
//-----------------------------------------------------------------------------
typedef basic_string<char> tstring;
typedef basic_string<char> t_string;

//-----------------------------------------------------------------------------
template <class T, size_t SZ>class Bucket
{
public:
    Bucket(){}
    ~Bucket(){}
    array_t<T>&    Array(int32_t i){return _arrays[i];}
    int32_t             Size(){return SZ;}

public:
    array_t<T>         _arrays[SZ];
};

//------------------------------------------------------------------------------
// shyncronized queue
template <class T> class cs_deque : public deque<T>
{
public:
    cs_deque(const char *pn){
        ::strcpy(_name,pn);
        _maxsz = 256;
        _actl  = 0;
    };
    virtual ~cs_deque(){
            this->deque<T>::clear();
    };

    void SetMaxSize(int32_t ms){
        _maxsz = ms;
    }
    int32_t    Count(){
        AutoLock q(&_cs);
         return this->deque<T>::size();
    }

    void Put(T row){
        _cs.mlock();
        this->deque<T>::push_back(row);
        _actl = this->deque<T>::size();
        _cs.munlock();
        _hmq.snotify();
    }

    int32_t TryGet(T* prow)
    {
        if(_cs.try_lock())
        {
            _actl = this->deque<T>::size();
		    if(_actl)
		    {
                *prow = this->deque<T>::at(0);
			    this->deque<T>::pop_front();
		    }
            _cs.mlock();
        }
        if(_actl)
        {
            _hmq.snotify();
        }
        return _actl;
    }
    int32_t Get(T* prow)
    {
        _cs.mlock();
        _actl = this->deque<T>::size();
		if(_actl)
		{
            *prow = this->deque<T>::at(0);
			this->deque<T>::pop_front();
		}
        _cs.munlock();
        if(_actl)
        {
            _hmq.snotify();
        }
        return _actl;
    }

    void Lock(){_cs.mlock();}
    void Unlock(){_cs.munlock();}
    int32_t Pick(T* prow)
    {
        _actl = this->deque<T>::size();
		if(_actl)
		{
            *prow = this->deque<T>::at(0);
			this->deque<T>::pop_front();
		}
        return _actl;
    }
    void Clear()
    {
        this->deque<T>::clear();
    }

    mutex            _cs;
    semaphore        _hmq;
    int32_t              _maxsz;
    int32_t              _actl;
    char             _name[32];
};

// makes member index access of the element at position index to be able to
// access them from script engine
template <class T> class Buf
{
public:
    explicit Buf():_pt(0){}
    explicit Buf(T* p):_pt(p){}

    operator const T*()const{return _pt;}
    operator T*(){return _pt;}
    const Buf<T>&  operator =( T* p){_pt=p; return this;}
    T&  at(int32_t idx){return *(_pt+idx);}
    const T&  operator[](int32_t idx)const{return *(_pt+idx);}
    T&  operator[](int32_t idx){return *(_pt+idx);}
private:
    T* _pt;
};






#endif //__BASECONT_H__

