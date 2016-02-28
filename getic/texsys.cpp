// File: TexHandler.cpp
// This code has been released for tutorial purposes. Do not use any of this code in
// any other sofware products.
//
// Started: Jan 2002
//---------------------------------------------------------------------------------------
#ifdef WINDOWS
#   include <io.h>
#endif
#include "stdafx.h"
#include "texsys.h"
#include "geticapp.h"

//--------[constructor]-------------------------------------------------------------------
u_bool32    TexSys::_GNoRefs       = false;

FILE* texture_fopen(const char *filename ,const char *mode)
{
    char loco[512];

    ::sprintf(loco,"%s%s",theApp->TexDir(),filename);
    if(access(loco,0)==0)
    {
        return _tfopen(loco,mode);
    }


    ::sprintf(loco,"%s%s",theApp->DocsDir(),filename);
    if(access(loco,0)==0)
    {
        return _tfopen(loco,mode);
    }

    ::sprintf(loco,"%sres/%s",theApp->DocsDir(),filename);
    if(access(loco,0)==0)
    {
        return _tfopen(loco,mode);
    }
    ::strcpy(loco,filename);
    return _tfopen(loco,mode);
}


//--------[constructor]-------------------------------------------------------------------
#define VALID_FILENAME(fstr_)   (fstr_ && fstr_[0] && fstr_[0]!=_G('*') )

//--------[constructor]-------------------------------------------------------------------
TexHandler::TexHandler(): n_x(0),n_y(0),n_bpp(0),p_pBuff(0),b_Filter(false),b_MipMap(true),
                          b_Ilum(false)
{

}

//--------[destructor]--------------------------------------------------------------
TexHandler::~TexHandler()
{
    Deallocate();
};

//--------[heap alocation fr buffer to store the image]-----------------------------------
u_bool32 TexHandler::CreateBuffer(int32_t width, int32_t height, int32_t deep)
{
    Deallocate();
    if(deep == 32)
    {
        n_x = width;
        n_y = height;
        n_bpp = 4;
        n_size = (n_x+16) * (n_y+1) * n_bpp;
        p_pBuff = Allocate((n_y+1), n_size);
        return Buffer()!=0;
    }
    else if(deep == 24)
    {
        n_x = width;
        n_y = height;
        n_bpp = 3;
        int32_t length = (width * 3 + 3) & ~3;
        n_size = length * n_y;
        p_pBuff = Allocate((n_y+1), n_size + length);
        return Buffer()!=0;
    }
    return false;
}
//--------------[resizes a texture]-------------------------------------------------
void TexHandler::Resize(int32_t nBytes, u_int8_t* pBufIn, int32_t wIn,
                           int32_t hIn, u_int8_t* pBufOut, int32_t wOut, int32_t hOut)
{
    f32_t sx = (f32_t) wIn/wOut;
    f32_t sy = (f32_t) hIn/hOut;
    u_int8_t* pTmp = pBufIn;

    for (int32_t y = 0; y < hOut; y++)
    {
        int32_t yy = (int32_t) (y*sy)*wIn;
        for (int32_t x = 0; x < wOut; x++)
        {
            int32_t xx = (int32_t) (x*sx);
            pTmp = pBufIn + (yy+xx)*nBytes;
            for (int32_t c = 0; c < nBytes; c++)
            {
                *pBufOut++ = *pTmp++;
            }
        }
    }
}

//----------------------------------------------------------------------------------
// places every color under RGB=16 (black) to 9, and everithing else to WHITE.
// Creates a black white mask
void TexHandler::MaskBW()
{
    u_int8_t* pw = p_pBuff[0];

    for(int32_t i=0;i<n_size;i+=n_bpp)
    {
        if(pw[i] < 16 && pw[1+i] < 16 && pw[i+2] < 16)
        {
            pw[i]   = 255;
            pw[i+1] = 255;
            pw[i+2] = 255;
        }
        else
        {
            pw[i]   = 0;
            pw[i+1] = 0;
            pw[i+2] = 0;
        }
    }
}


void  TexHandler::SwapRB(u_int8_t* pRGB, int32_t bpp, int32_t cx, int32_t cy)
{
    u_int8_t* pw = pRGB;
    u_int8_t  tmp;
    int32_t   sz = (cx * cy*bpp);

    for(int32_t i=0; i<sz;i+=bpp)
    {
        tmp     = pw[i];
        pw[i]   = pw[i+2];
        pw[i+2] = tmp;
    }
}

//----------------------------------------------------------------------------------------
void  TexHandler::SwapRB()
{
    u_int8_t* pw = p_pBuff[0];

    u_int8_t   tmp;
    for(int32_t i=0;i<n_size;i+=n_bpp)
    {
        tmp = pw[i];
        pw[i] = pw[i+2];
        pw[i+2] = tmp;
    }
}

u_bool32 TexHandler::LoadThisFile(const char *sFileName, u_int32_t flags)
{
    char sExt[16] = {0};

    u_bool32     br =0;
#if 0
    _tsplitpath(sFileName,sDummy,sDummy,sDummy,sExt);
    if (!stricmp(sExt,_T(".tga")))
        br = LoadTgaFile(sFileName, flags);
    else  if (!stricmp(sExt,_T(".jpg")))
        br = LoadJpegFile(sFileName, flags);
    else if (!stricmp(sExt,_T(".bmp")))
        br = LoadBmpFile(sFileName, flags);
    if(br && (flags & TEX_ALPHABW))
    {
        AlphaIt(0);
    }
#endif
    assert(0);
    return br;
}

//--------[dispatches the load to the private implementations]----------------------------
u_bool32 TexHandler::LoadFile(const char *sFileName, u_int32_t flags)
{
    if(!VALID_FILENAME(sFileName))
        return 0;

    HTEX iret;
    char newname[128]={0};
    if(!_ttcschr(sFileName,_G('.')))
    {
        _ttcscpy(newname, sFileName);
        _ttcscat(newname,_T(".jpg"));
        iret = LoadFile2(newname,flags);

        if(iret==0)
        {
            _ttcscpy(newname, sFileName);
            _ttcscat(newname,_T(".tga"));
            iret =LoadFile2(newname,flags);

            if(iret==0)
            {
                _ttcscpy(newname, sFileName);
                _ttcscat(newname,_T(".bmp"));
                iret =LoadFile2(newname,flags);
            }
        }
        if(iret)
            return iret;
    }
    iret = LoadFile2(sFileName,flags);
    if(iret==0)
    {
        iret = _dumptex;
    }
    else if(flags & TEX_ALPHABW)
    {
        AlphaIt(0);
    }
    return iret;
}


//----------------------------------------------------------------------------------------
int32_t TexHandler::LoadFile2(const char *sFileName, u_int32_t flags)
{
    const   char* phassl  = _ttcschr(sFileName,_G('\\'));
    const   char* phassl2 = _ttcschr(sFileName,_G('/'));

    if(_tcsstr(sFileName,"tga"))
        return LoadTgaFile(sFileName, flags);
    else if(_tcsstr(sFileName,"jpg"))
        return LoadJpegFile(sFileName, flags);
    else if(_tcsstr(sFileName,"bmp"))
        return LoadBmpFile(sFileName, flags);

    return false;
}
//--------------------------------------------------------------------------------------
u_bool32 TexHandler::LoadJpegFile(const char *sFileName, u_int32_t flags)
{

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];

    FILE *infile = fopen( sFileName, "rb" );
    u_int32_t location = 0;
    int32_t i = 0;

    if ( !infile )
    {
        return false;
    }
    cinfo.err = jpeg_std_error( &jerr );
    jpeg_create_decompress( &cinfo );
    jpeg_stdio_src( &cinfo, infile );
    jpeg_read_header( &cinfo, true );
    jpeg_start_decompress( &cinfo );

    CreateBuffer(cinfo.output_width, cinfo.output_height, 24);//cinfo.num_components*8 /*was 24*/);

    row_pointer[0] = (u_int8_t *)malloc( cinfo.output_width*cinfo.num_components );
    int32_t ln=0;
    while( cinfo.output_scanline < cinfo.image_height )
    {
        jpeg_read_scanlines( &cinfo, row_pointer, 1 );

        for( i=0; i<cinfo.image_width*cinfo.num_components;i++)
        {
            //p_pBuff[ln][location++] = (u_int8_t)row_pointer[0][i];
            p_pBuff[ln][i] = (u_int8_t)row_pointer[0][i];
        }
        ln++;
    }

    jpeg_finish_decompress( &cinfo );
    jpeg_destroy_decompress( &cinfo );
    free( row_pointer[0] );
    fclose( infile );

    Deallocate();
    return true;
}

//--------------------------------------------------------------------------------------
u_bool32 TexHandler::LoadTgaFile(const char *sFileName, u_int32_t flags)
{
    Deallocate();
    FILE* fp = texture_fopen(sFileName,_T("rb"));

    if(fp)
    {
        u_int8_t tgaStruct[18] = {0};
        fread((char *)tgaStruct, 18,1,fp);
        if(!((tgaStruct[16]==24 || tgaStruct[16]==32) && (tgaStruct[2]==2 || tgaStruct[2]==10)))
        {
            fclose(fp);
            return false;
        }

        int32_t x= (int32_t)(*((u_int16_t *)&tgaStruct[12]));
        int32_t y= (int32_t)(*((u_int16_t *)&tgaStruct[14]));
        if(tgaStruct[16]==24)
        {
            CreateBuffer(x, y, 24);
        }
        else
        {
            CreateBuffer(x, y, 32);
        }
        int32_t pixOrder = tgaStruct[17] & 0x30;
        if(tgaStruct[2]==2)
        {
            fread(Buffer(), n_size,1,fp);
        }
        else
        {
            u_int8_t *p = p_pBuff[0];
            int32_t idx = 0;
            while(idx < n_x * n_y)
            {
                u_int8_t by;
                fread(&by,1,1,fp);
                if(0 == (by & 0x80))
                {
                    by++;
                    fread(p, by * n_bpp,1,fp);
                    p += by * n_bpp;
                }
                else
                {
                    by = (by & 0x7f) + 1;
                    fread(p, n_bpp,1,fp);
                    u_int8_t* q = p;
                    for(int32_t i=1; i < by; i++)
                    {
                        q += n_bpp;
                        q[0] = p[0];
                        q[1] = p[1];
                        q[2] = p[2];
                    }
                    p = q;
                    p += n_bpp;
                }
                idx += by;
            }
        }

        if (pixOrder==0x00 || pixOrder==0x10)
        {
            (void)0;
        }
        else
        {
            Invert();
        }

        for(y=0; y<n_y; y++ )
        {
            for(int32_t p=0; p<n_x; p++)
            {
                int32_t xPos1 = p * n_bpp;
                u_int8_t bFlag = p_pBuff[y][xPos1 + 2];
                p_pBuff[y][xPos1+2]=p_pBuff[y][xPos1];
                p_pBuff[y][xPos1]=bFlag;
            }
            if(pixOrder==0x10 || pixOrder==0x30)
            {
                for(int32_t p=0; p<n_x/2; p++)
                {
                    int32_t xPos1=p*n_bpp;
                    int32_t xPos2=(n_x-1-p)*n_bpp;
                    for(int32_t i=0; i<n_bpp; i++)
                    {
                        u_int8_t bFlag=p_pBuff[y][xPos2+i];
                        p_pBuff[y][xPos2+i]=p_pBuff[y][xPos1+i];
                        p_pBuff[y][xPos1+i]=bFlag;
                    }
                }
            }
        }
        fclose(fp);
        return true;
    }
    Deallocate();
    return false;
}

//--------------------------------------------------------------------------------------
int32_t TexHandler::SaveTgaFile(const char *sFileName)
{
    FILE* fp=texture_fopen(sFileName,_T("wb"));

    if (0 != fp)
    {
        u_int8_t* ingLine = new u_int8_t[n_x*n_bpp];
        u_int8_t tgaStruct[18] = {0};

        tgaStruct[12]=(u_int8_t)(n_x%256);
        tgaStruct[13]=(u_int8_t)(n_x/256);
        tgaStruct[14]=(u_int8_t)(n_y%256);
        tgaStruct[15]=(u_int8_t)(n_y/256);
        tgaStruct[2]=2;
        tgaStruct[16]=8*n_bpp;
        fwrite((char *)&tgaStruct[0],18,1,fp);

        for(int32_t ii=n_y-1;ii>=0;ii-- )
        {
            for(int32_t jj=0;jj<n_x;jj++ )
            {
                ingLine[jj*n_bpp] = p_pBuff[ii][(jj*n_bpp+2)];
                ingLine[jj*n_bpp+1] = p_pBuff[ii][(jj*n_bpp+1)];
                ingLine[jj*n_bpp+2] = p_pBuff[ii][(jj*n_bpp)];
                if (n_bpp==4)
                   ingLine[jj*n_bpp+3] = p_pBuff[ii][(jj*n_bpp+3)];
            }
            n_bpp;
            int32_t bytes = fwrite((char*)ingLine, n_x, n_bpp, fp);
            if(n_bpp != bytes)
            {
                delete[]ingLine;
                fclose(fp);
                return false;
            }
        }
        fclose(fp);
        delete[]ingLine;
    }

    return true;
}

//--------------[maby i'll useit for dynamic lighting]-----------------------------------
u_bool32 TexHandler::CreateMap(f32_t mapScale)
{
    if (n_bpp == 4)
    {
        f32_t fraport=1.0f/255.0f;
        u_int8_t *uc = p_pBuff[0];
        for(int32_t ii=0;ii<n_x;ii++ )
        {
            for(int32_t jj=0;jj<n_y;jj++ )
            {
                f32_t c = uc[3] * fraport;
                f32_t cx = p_pBuff[ii][((jj+1)%n_x)*4+3] * fraport;
                f32_t cy = p_pBuff[(ii+1)%n_y][jj*4+3] * fraport;
                f32_t dcx = mapScale * (c - cx);
                f32_t dcy = mapScale * (c - cy);

                f32_t len = dcx*dcx + dcy*dcy + 1.0f;
                f32_t nz = (f32_t)(1.0/sqrt(len));
                f32_t nx = dcy*nz;
                f32_t ny = -dcx*nz;

                uc[0] = (u_int8_t)(128 + 127*nx);
                uc[1] = (u_int8_t)(128 + 127*ny);
                uc[2] = (u_int8_t)(128 + 127*nz);
                uc+=4;
            }
        }
        return true;
    }
    return false;
}

//--------------[maby i'll useit for dynamic lighting]-----------------------------------
u_bool32 TexHandler::MakeBW()
{
    int32_t    sz  = n_x*n_y*3;
    u_int8_t** pNew = Allocate(n_y, sz);
    u_int8_t* pNeww = pNew[0];
    u_int8_t* pOld = p_pBuff[0];

    int32_t j=0;

    for(int32_t i=0;i<n_size;i+=n_bpp,j+=3)
    {
        pNeww[j] = (pOld[i]+pOld[i+1]+pOld[i+2])/3;
        pNeww[j+1] = pNeww[j];
        pNeww[j+2] = pNeww[j];
    }
    Deallocate();
    p_pBuff = pNew;
    n_bpp   = 3;
    return (b_Ilum = true);
}

//-----------------------------------------------------------------------------------------
u_bool32 TexHandler::_GetPixel(u_int8_t* pOld,  int32_t x, int32_t y)
{
    x = (x+n_x)%n_x;
    y = (y+n_y)%n_y;
    return pOld[(n_x*y+x)*n_bpp+0];
}

//-----------------------------------------------------------------------------------------
u_bool32 TexHandler::MakeDot3(f32_t intens)
{
    int32_t  sz= n_x*n_y*4;
    u_int8_t** pNew  = Allocate(n_y, sz);
    u_int8_t*  pNeww = pNew[0];
    u_int8_t*  pOld  = p_pBuff[0];
    f32_t   dx,dy;

    for(int32_t x=0; x<n_x; x++)
    {
        for(int32_t y=0; y<n_y; y++)
        {
            dx  = _GetPixel( pOld, x-1, y-1);					//  1  0 -1
            dx += 2.f*(f32_t)_GetPixel( pOld, x-1, y);			//  2  0 -2
            dx += _GetPixel( pOld, x-1, y+1);					//  1  0 -1
            dx -= _GetPixel( pOld, x+1, y-1);
            dx -= 2.f*(f32_t)_GetPixel( pOld, x+1, y);
            dx -= _GetPixel( pOld, x+1, y+1);
            dx /= 255.f;


            dy  = _GetPixel( pOld, x-1, y-1);					//  1  2  1
            dy += 2.f*(f32_t)_GetPixel( pOld, x  , y-1);		//  0  0  0
            dy += _GetPixel( pOld, x+1, y-1);					// -1 -2 -1
            dy -= _GetPixel( pOld, x-1, y+1);
            dy -= 2.f*(f32_t)_GetPixel( pOld, x  , y+1);
            dy -= _GetPixel( pOld, x+1, y+1);
            dy /= 255.f;

            v3d_t   vclr(intens*dx, intens*dy, 1);
            vclr.norm();
            vclr = vclr*.05f + 0.5f;

            pNeww[(n_x*y+x)*n_bpp+0] = (u_int8_t)(255.f*vclr.x);
            pNeww[(n_x*y+x)*n_bpp+1] = (u_int8_t)(255.f*vclr.y);
            pNeww[(n_x*y+x)*n_bpp+2] = (u_int8_t)(255.f*vclr.z);
        }
    }
    Deallocate();
    p_pBuff = pNew;
    return 1;
}

//--------------[frees the row vector and whole array]--------------------------------------
void TexHandler::Deallocate()
{
    if(p_pBuff)
    {
        delete[] p_pBuff[0];
        p_pBuff[0] = 0;
        delete[] p_pBuff;
        p_pBuff = 0;
    }
}

//--------------[new's the row vector and rows]--------------------------------------
u_int8_t** TexHandler::Allocate(int32_t lines, int32_t rgbasz)
{
    b_Filter = false;
    b_MipMap = true;
    b_Ilum = false;
    u_int8_t** p;
    p    = new u_int8_t* [lines];
    u_int8_t* pall = new u_int8_t[rgbasz];
    for(int32_t i=0; i < lines; i++)
    {
        p[i] = &pall[i* n_x * n_bpp];
    }
    return p;
}

//--------------[:)]--------------------------------------------------------------
u_bool32 TexHandler::LoadBmpFile(const char *sFileName, u_int32_t flags)
{
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    int32_t				length;
    u_int8_t*			bits;
    u_int8_t*			ptr;
    u_int8_t			temp;
    int32_t				x,y;
    u_bool32			br = false;
    FILE* f=texture_fopen(sFileName,_T("rb"));
    if (!f)
    {
        char p[266];
        _getcwd(p,255);

        return false;
    }
    int32_t tst;
    if (!(tst=fread(&bmfh,1,sizeof(bmfh),f)))
      goto xERR;
    if (bmfh.bfType!=0x4D42)
      goto xERR;

    if (!(tst=fread(&bmih,1,sizeof(bmih),f)))
      goto xERR;

    if(bmih.biCompression!=BI_RGB)
      goto xERR;
/*
    if (!IsPowOf2(bmih.biWidth))
      goto ERR;
    if (!IsPowOf2(bmih.biHeight))
      goto ERR;
*/

    switch(bmih.biBitCount)
    {
        case 1:
            br =LoadBMP1bit(f, bmfh, bmih);
            fclose(f);
            return br;
        case 4:
            br =LoadBMP4bit(f, bmfh, bmih);
            fclose(f);
            return br;
        case 8:
            br =LoadBMP8bit(f, bmfh, bmih);
            fclose(f);
            return br;
        case 24:
        case 32:
            break;
        default:
            fclose(f);
            return false;
    }

    length = (bmih.biWidth * 3 + 3) & ~3;
    if(bmih.biWidth*3 != length)
        goto xERR;

    CreateBuffer(bmih.biWidth, bmih.biHeight, bmih.biBitCount);
    fseek(f, bmfh.bfOffBits, SEEK_SET);
    for ( y = 0; y < bmih.biHeight; y++)
    {
        fread(p_pBuff[y],1, n_bpp * n_x ,f);
    }
    fclose(f);
    length = (bmih.biWidth * 3 + 3) & ~3;
    bits   = Buffer();
    for (y = 0; y < bmih.biHeight; y ++)
    {
          for (ptr = bits + y * length, x = bmih.biWidth; x > 0; x--, ptr += 3)
          {
                temp   = ptr[0];
                ptr[0] = ptr[2];
                ptr[2] = temp;
          }
    }

    //Invert();
    return true;
xERR:
  fclose(f);
  Deallocate();
  return false;

}

u_bool32 TexHandler::LoadBMP1bit(FILE *f, BITMAPFILEHEADER &fileheader, BITMAPINFOHEADER &infoheader)
{
    RGBQUAD pal[2];

    if(fread( pal, sizeof(RGBQUAD), infoheader.biClrUsed, f)!=infoheader.biClrUsed)
    {
        return false;
    }

    u_int8_t* pData = 0;
    int32_t lineSz = infoheader.biWidth/8;
    if(infoheader.biWidth%8)
        lineSz++;
    if(lineSz%4)
        lineSz += 4 - lineSz % 4;
    pData = new u_int8_t[lineSz  * infoheader.biHeight];
    if(pData==0)
    {
        return false;
    }

    if(!fread( pData, lineSz*infoheader.biHeight, 1, f))
    {
        delete [] pData;
        return false;
    }

    CreateBuffer(infoheader.biWidth, infoheader.biHeight, 24);

    int32_t planes= 3;
    u_int8_t* pw = p_pBuff[0];
    for(int32_t y=0; y<infoheader.biHeight; y++)
    {
        for(int32_t x=0; x<infoheader.biWidth; x++)
        {
            u_int8_t index=pData[y*lineSz+x/8];
            index >>= (7-x%8);
            index &= 0x01;
            pw[(y*infoheader.biWidth+x)*planes+0] = pal[index].rgbRed;
            pw[(y*infoheader.biWidth+x)*planes+1] = pal[index].rgbGreen;
            pw[(y*infoheader.biWidth+x)*planes+2] = pal[index].rgbBlue;
        }
    }
    return true;
}


//----------------------------------------------------------------------------------------
u_bool32 TexHandler::LoadBMP4bit(FILE *f, BITMAPFILEHEADER &fileheader, BITMAPINFOHEADER &infoheader)
{
    RGBQUAD pal[16];

    if(fread( pal, sizeof(RGBQUAD), infoheader.biClrUsed, f)!=infoheader.biClrUsed)
    {
        return false;
    }

    u_int8_t*       pData = 0;
    int32_t         lineSz = infoheader.biWidth/2+infoheader.biWidth%2;
    if(lineSz%4)lineSz += 4 - lineSz%4;
    pData = new u_int8_t[lineSz*infoheader.biHeight];
    if(pData==NULL)
    {
        return false;
    }
    if(!fread( pData, lineSz*infoheader.biHeight, 1, f))
    {
        delete [] pData;
        return false;
    }


    CreateBuffer(infoheader.biWidth, infoheader.biHeight, 24);
    int32_t planes=3;
    u_int8_t* pw = p_pBuff[0];
    for(int32_t y=0; y<infoheader.biHeight; y++)
    {
        for(int32_t x=0; x<infoheader.biWidth; x++)
        {
            int32_t index;
            if(x%2)
                index = pData[y*lineSz+x/2]&0x0F;
            else
                index = (pData[y*lineSz+x/2]&0x0F0)>>4;

            pw[(y*infoheader.biWidth+x)*planes+0] = pal[index].rgbRed;
            pw[(y*infoheader.biWidth+x)*planes+1] = pal[index].rgbGreen;
            pw[(y*infoheader.biWidth+x)*planes+2] = pal[index].rgbBlue;
        }
    }




    return true;
}

u_bool32 TexHandler::LoadBMP8bit(FILE *f, BITMAPFILEHEADER &fileheader, BITMAPINFOHEADER &infoheader)
{
    RGBQUAD pal[256];


    if(infoheader.biClrUsed==0 && infoheader.biCompression==BI_RGB)
    {
        infoheader.biClrUsed=256;
    }
    int32_t wr = fread( pal, 1, sizeof(RGBQUAD)*infoheader.biClrUsed, f);
    if(wr != infoheader.biClrUsed*sizeof(RGBQUAD))
    {
        return false;
    }
    infoheader.biClrUsed=0;

    u_int8_t* pData = 0;
    int32_t lineSz = infoheader.biWidth;
    if(lineSz%4)lineSz += 4 - lineSz%4;
    pData = new u_int8_t[lineSz*infoheader.biHeight];
    if(pData==NULL)
    {
        return false;
    }
    int32_t bmpsz = lineSz*infoheader.biHeight;
    if(!(wr=fread( pData, 1, bmpsz, f)))
    {
        delete [] pData;
        return false;
    }
/*
    FILE* pfo = fopen("C:/_pal.bmp","wb");
    if(pfo)
    {
        int32_t wrb = sizeof(fileheader);
        wrb = fwrite(&fileheader,1,sizeof(fileheader),pfo);
        wrb = sizeof(infoheader);
        wrb = fwrite(&infoheader,1,sizeof(infoheader),pfo);

        int32_t j=0;
        for(int32_t i=0; i < 256*3; )
        {
            pal[j].rgbRed   = paltilc[i++];
            pal[j].rgbGreen = paltilc[i++];
            pal[j].rgbBlue  = paltilc[i++];
            pal[j].rgbReserved = 0;
            ++j;
        }
        wrb = sizeof(pal);
        wrb = fwrite(&pal,1,sizeof(pal),pfo);
        wrb = fwrite(pData,1,bmpsz,pfo);


        fclose(pfo);

    }
*/

    int32_t planes = 3;
    CreateBuffer(infoheader.biWidth, infoheader.biHeight, 24);
    u_int8_t* pw = p_pBuff[0];
    for(int32_t y=0; y<infoheader.biHeight; y++)
    {
        for(int32_t x=0; x<infoheader.biWidth; x++)
        {
            pw[(y*infoheader.biWidth+x)*planes+0] = pal[pData[y*lineSz+x]].rgbRed;
            pw[(y*infoheader.biWidth+x)*planes+1] = pal[pData[y*lineSz+x]].rgbGreen;
            pw[(y*infoheader.biWidth+x)*planes+2] = pal[pData[y*lineSz+x]].rgbBlue;
        }
    }
    delete[] pData;
    return true;
}



TexSys::TexSys(){
    _stx = 0;
    _rtx = 0;
    _cleaned = false;

}

TexSys::TexSys(STCB scb, RTCB rtc){
    _stx = scb;
    _rtx = rtc;
    _cleaned = false;

}

//--------------[]-----------------------------------------------------------------------
// Generates a texture from passed in RGB values
Htex& TexSys::GenTexture(const char* locn, int32_t x, int32_t y, int32_t bpp, u_int8_t* buff, u_int32_t flags)
{
    Texture          tex;
    TexSys::iterator fi = theApp->_TexSys.find(locn);

    if(fi == theApp->_TexSys.end()) //tex.refcount == 0)
    {
        tex.hTex = (_stx)(x, y, bpp, buff, flags);
        tex.cx  =   x;
        tex.cy  =   y;
        tex.bpp =   bpp;
        ::_ttcscpy(tex.filename, locn);
        theApp->_TexSys[locn] =  tex;

        fi = theApp->_TexSys.find(locn);
    }
    if(TexSys::_GNoRefs)
        fi->second.refcount = 1;
    else
        ++fi->second.refcount;
    return fi->second.hTex;
}


//--------------[adds a texture file and returns a copy of bitmap]-------------------
Htex&  TexSys::AddTextureFileGetImage(const char* szFileName,
                                      u_int8_t** pb, int32_t* dx,
                                      int32_t* dy, int32_t* bpp,u_int32_t flags)
{
    char    locn[255];

    if(!VALID_FILENAME(szFileName))
        return _dumptex;
    sprintf(locn, "%s?%x",szFileName,flags);

    TexSys::iterator fi = theApp->_TexSys.find(locn);

    if(fi == theApp->_TexSys.end()) //tex.refcount == 0)
    {
        Texture     tex;
        TexHandler	t;
        if(t.LoadFile(szFileName, flags))
        {

            tex.hTex = (_stx)(t.n_x, t.n_y, t.n_bpp, t.Buffer(), flags);
            ::_ttcscpy(tex.filename, szFileName);
            tex.hTex.glTarget = flags;

            *dx  = tex.cx  = t.n_x;
            *dy  = tex.cy  = t.n_y;
            *bpp = tex.bpp = t.n_bpp;
            tex.pBuff = new u_int8_t[t.n_x*t.n_y*t.n_bpp];
            ::memcpy(tex.pBuff, t.Buffer(), t.n_x*t.n_y*t.n_bpp);
            *pb    = tex.pBuff;
            theApp->_TexSys[locn] = tex;

            fi = theApp->_TexSys.find(locn);
        }
    }
    fi = theApp->_TexSys.find(locn);
    if(fi != theApp->_TexSys.end())
    {
        if(TexSys::_GNoRefs)
            fi->second.refcount = 1;
        else
            ++fi->second.refcount;
        return fi->second.hTex;
    }
    return _dumptex;;
}

//--------------[adds a texture file/ render wrapper]-------------------------------
Htex& TexSys::AddTextureFile(const char* szFileName, u_int32_t flags)
{
    char    locn[255];

    if(!VALID_FILENAME(szFileName))
        return _dumptex;
    sprintf(locn, "%s?%x",szFileName,flags);

    TexSys::iterator fi = theApp->_TexSys.find(locn);
    if(fi == theApp->_TexSys.end()) //tex.refcount == 0)
    {
        Texture     tex;
        TexHandler	t;
        if(t.LoadFile(szFileName, flags) && t.Buffer())
        {
            uint8_t* pb = t.Buffer();
            tex.hTex = (_stx)(t.n_x, t.n_y, t.n_bpp, t.Buffer(), flags);

            ::_ttcscpy(tex.filename, szFileName);

            tex.hTex.glTarget = flags;
            tex.cx      = t.n_x;
            tex.cy      = t.n_y;
            tex.bpp     = t.n_bpp;
            //tex.pBuff   = t.Buffer();//mco-mco

            theApp->_TexSys[locn] =  tex;
            fi = theApp->_TexSys.find(locn);
        }
    }
    if(fi != theApp->_TexSys.end())
    {
        if(TexSys::_GNoRefs)
            fi->second.refcount = 1;
        else
            ++fi->second.refcount;
        return fi->second.hTex;
    }
    return _dumptex;
}
/*
//--------------[remove a texture file]-------------------------------------------
void TexSys::RemoveTextureFile(const char* szFileName)
{
    Texture&    tex = theApp->_TexSys[szFileName];
    if(tex.refcount > 0)
        --tex.refcount;
    if(tex.refcount == 0)
    {
        if(_rtx)
        {
            assert(ThrID == GetCurrentThreadId());
            (_rtx)(&tex.hTex,1);
        }
        if(tex.pBuff)
            delete[] tex.pBuff;
        tex.pBuff=0;
        erase(szFileName);
    }
}
*/


Htex& TexSys::Assign(const char* psz, u_int32_t flags)
{
    return AddTextureFile(psz, flags);
}

//--------------[removes texture by id]---------------------------------------------
void TexSys::RemoveTexture(Htex& itex, u_bool32 rd)
{

    if(itex.hTex<=0) return;

    TexSys::iterator b = begin();
    TexSys::iterator e = end();
    for(;b!=e;b++)
    {
        Texture& ip = (*b).second;
        if(ip.hTex == itex)
        {
            --ip.refcount;
            if(ip.refcount==0)
            {
                if(rd)
                {
                    //assert(ThrID == GetCurrentThreadId());
                    // assert(0); //LOOK UP
                    if(_rtx) (_rtx)(&itex,1);
                }
                if(ip.pBuff)
                    delete[] ip.pBuff;
                ip.pBuff=0;
                erase(b);
                itex.hTex     = 0;
                break;
            }
        }
    }
}

//--------------|removes an array of textures|--------------------------------------
void TexSys::RemoveTextures(Htex* itextures, int32_t count)
{
    //assert(ThrID == GetCurrentThreadId());
    assert(0); //LLOK UP
    if(_rtx) (_rtx)(itextures,count);
    for(int32_t i=0;i<count;i++)
    {
        RemoveTexture(*(itextures+i),false);
        (itextures+i)->hTex = 0;
    }
}

//----------------------------------------------------------------------------------
// cleas all textures
void TexSys::Clean()
{
    if(size() == 0)
        return;
    TRACEX("TexSys::Clean\r\n");

    TexSys::iterator bt = begin();
    TexSys::iterator et = end();
    for(;bt!=et;bt++)
    {
        if((*bt).second.pBuff){
            delete[] (*bt).second.pBuff;
            (*bt).second.pBuff=0;
        }
        if(_rtx)
            (_rtx)(&(*bt).second.hTex,1);
    }
    clear();
}

//----------------------------------------------------------------------------------
// temp pointer not to be stored
Texture* TexSys::GetTempTexture(const char* ptName, u_int32_t flags)
{
    TexSys::iterator it = find(MKSTR("%s-%x",ptName,flags));
    if(it != end())
    {
        return &(*it).second;
    }
    return 0;
}

//----------------------------------------------------------------------------------
Texture* TexSys::GetTempTexture(Htex& itex)
{
    TexSys::iterator b = begin();
    TexSys::iterator e = end();

    for(;b!=e;b++)
    {
        Texture& ip = (*b).second;
        if(ip.hTex == itex)
        {
            return &(*b).second;
        }
    }
    return 0;
}

const char* TexSys::GetTexName(Htex& itex)
{
    TexSys::iterator b = begin();
    TexSys::iterator e = end();

    for(;b!=e;b++)
    {
        Texture& ip = (*b).second;
        if(ip.hTex == itex)
        {
            return (*b).first.c_str();
        }
    }
    return "";
}
void  TexHandler::Invert()
{
    u_int8_t* line = new u_int8_t[32768];

    for(int32_t i=0; i < n_y/2; i++ )
    {
        //flip vertical
        memcpy(&line[0], p_pBuff[i], n_bpp * n_x);
        memcpy(p_pBuff[i], p_pBuff[n_y-i-1], n_bpp * n_x);
        memcpy(p_pBuff[n_y-i-1], line, n_bpp * n_x);
    }

    delete[]	line;

}


void TexHandler::AlphaIt(u_int8_t alpha)
{
    if(n_bpp!=3)
        return;
    u_int8_t* pw  = p_pBuff[0];
    int32_t   sz  = n_x*n_y*4;

    u_int8_t** pd = Allocate(n_y, sz);
    u_int8_t* px  = pd[0];

    for(int32_t i=0,j=0;i<n_size;i+=n_bpp,j+=4)
    {
        px[j]   = pw[i];
        px[j+1] = pw[i+1];
        px[j+2] = pw[i+2];
        if(pw[i] < 16 && pw[1+i] < 16 && pw[i+2] < 16)
            px[j+3] = 0;
        else
            px[j+3] = 255;

    }
    n_size=sz;
    n_bpp=4;
    Deallocate();
    p_pBuff = pd;
}


//----------------------------------------------------------------------------------
u_int8_t* TexHandler::AlphaIt(u_int8_t* pTemp, int32_t x, int32_t y, int32_t& bpp)
{
    u_int8_t* pw  = pTemp;
    int32_t   sz  = x*y*4;

    u_int8_t* pd  = new u_int8_t[sz];
    u_int8_t* px  = pd;

    for(int32_t i=0,j=0;i<x*y*bpp;i+=bpp,j+=4)
    {
        px[j]   = pw[i];
        px[j+1] = pw[i+1];
        px[j+2] = pw[i+2];
        if(pw[i] < 32 && pw[1+i] < 32 && pw[i+2] < 32)
            px[j+3] = 0;
        else
            px[j+3] = 255;

    }
    bpp=4;
    return pd;
}

//----------------------------------------------------------------------------------
// places every color under RGB=16 (black) to 9, and everithing else to WHITE.
// Creates a black white mask
void TexHandler::MaskBW(u_int8_t* pb, int32_t sz, int32_t bpp)
{
    u_int8_t* pw = pb;

    for(int32_t i=0;i<sz;i+=bpp)
    {
        if(pw[i] < 16 && pw[1+i] < 16 && pw[i+2] < 16)
        {
            pw[i]   = 255;
            pw[i+1] = 255;
            pw[i+2] = 255;
        }
        else
        {
            pw[i]   = 0;
            pw[i+1] = 0;
            pw[i+2] = 0;
        }
    }
}

void TexHandler::SaveBMP(char *filename)
{
    BITMAPFILEHEADER fileheader;
    BITMAPINFOHEADER infoheader;

    FILE *sub;

    int32_t line_size = n_bpp*n_x;
    if(line_size%4)line_size += 4 - line_size%4;

    fileheader.bfType = 0x4D42; // Magic identifier   - "BM"	| identifikacia BMP suboru musi byt "BM"
    fileheader.bfSize = n_bpp*n_x*n_y+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);	// File size in bytes			| velkos suboru v byte
    fileheader.bfReserved1 = 0;
    fileheader.bfReserved2 = 0;
    fileheader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);	// Offset to image data, bytes	| posun na zaciatok dat

    infoheader.biSize = sizeof(BITMAPINFOHEADER);	// Header size in bytes			| velkost hlavicky BITMAPINFOHEADER
    infoheader.biWidth = n_x;	// Width of image			| sirka obrazka - sizeX
    infoheader.biHeight = n_y;	// Height of image			| vyska obrazka - sizeY
    infoheader.biPlanes = 1;		// Number of colour planes	| pocet farebnych rovin musi bit 1
    infoheader.biBitCount = 24;		// Bits per pixel			| bitov na pixel moze bit 1,4,8,24
    infoheader.biCompression = 0;	// Compression type			| typ compresie , 0 - bez kompresie
    infoheader.biSizeImage = line_size*infoheader.biHeight ;	// Image size in bytes		| velkost obrazka v byte
    infoheader.biXPelsPerMeter = 0;	// Pixels per meter X		| pixelov na meter v smere x
    infoheader.biYPelsPerMeter = 0;	// Pixels per meter Y		| pixelov na meter v smere y
    infoheader.biClrUsed = 0;		// Number of colours		| pocet  farieb v palete, ak 0 vsetky su pouzivane
    infoheader.biClrImportant = 0;	// Important colours		| dolezite farby v palete, ak 0 vsetky su dolezite

    sub = fopen(filename,"wb");
    if(sub==NULL)return;
    fwrite( &fileheader, sizeof(BITMAPFILEHEADER), 1, sub);
    fwrite( &infoheader, sizeof(BITMAPINFOHEADER), 1, sub);

    u_int8_t* pw = p_pBuff[0];
    u_int8_t* p  = new u_int8_t[line_size*n_x];

    for(int32_t y=0; y<n_y; y++)
    {
        for(int32_t x=0; x<n_x; x++)
        {
            /*
            if(BRG)
            {
                p[y*line_size+x*3+0] = pw[(y*n_x+x)*3+0];
                p[y*line_size+x*3+1] = pw[(y*n_x+x)*3+1];
                p[y*line_size+x*3+2] = pw[(y*n_x+x)*3+2];
            }
            else
            {
            */
                p[y*line_size+x*3+0] = pw[(y*n_x+x)*3+2];
                p[y*line_size+x*3+1] = pw[(y*n_x+x)*3+1];
                p[y*line_size+x*3+2] = pw[(y*n_x+x)*3+0];
            //}
        }
    }
    fwrite( p, line_size*n_y, 1, sub);
    delete[] p;
    fclose(sub);
}
