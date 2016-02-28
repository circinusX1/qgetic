/**
    File:    isound.h
    Author:  Marius Chincisan
    Desc:    Interface definition for sound plugins DLL's
    History:
*/

#ifndef __Isound_H__
#define __Isound_H__

#include "../baselib/baseutils.h"
#include "../baselib/basecont.h"


//---------------------------------------------------------------------------------------
// One sound sample
class NO_VT Itape
{
public:
	Itape(){}
	virtual ~Itape(){}
public: 
	virtual u_bool32 D3Sound()=0;
	virtual void D3Sound(u_bool32 b)=0;
    virtual void D3Params(f32_t minDistance, f32_t maxDistance)=0;
    virtual void D3Pos(f32_t x, f32_t y, f32_t z)=0;
	virtual void SetVolume(f32_t x)=0;
	virtual void AddRef()=0;
	virtual int32_t  Release()=0;
};

//---------------------------------------------------------------------------------------
// The sound player. PLay tapes.
class NO_VT Isound
{
public:
	virtual u_bool32	Create(HWND hwnd, const tchar* srcPath, int32_t maxSounds)=0;
	virtual void	Destroy()=0;
	virtual Itape*	CreateTape(char *filename, u_bool32 is3DSound)=0;
	virtual void	DestroyTape(Itape* p)=0;
	virtual void	PlayTape(const Itape *audio, u_int32_t numRepeats)=0;
	virtual void	StopTape(const Itape *audio)=0;
	virtual void	SetListenerPos(f32_t cameraX, f32_t cameraY, f32_t cameraZ)=0;
	virtual void	SetListenerRolloff(f32_t rolloff)=0;
	virtual void	SetListenerOrientation(f32_t forwardX, f32_t forwardY, f32_t forwardZ, f32_t topX, f32_t topY, f32_t topZ)=0;
};



#endif // __Isound_H__


