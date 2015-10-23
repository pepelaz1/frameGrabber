#include "moreuuids.h"
#include "overlayfilter.h"
#include <QString>
#include <QDebug>
#include <Gdiplus.h>
#include <Dvdmedia.h>

#pragma comment (lib,"Gdiplus.lib")
using namespace Gdiplus;

OverlayFilter::OverlayFilter(LPWSTR name, LPUNKNOWN unk, HRESULT *hr) :
  //  CTransInPlaceFilter(name, unk, CLSID_TextOverlay, hr)
    CTransformFilter(name,unk, CLSID_TextOverlay )

{
    m_videoWidth = 0;
    m_videoHeight = 0;
    m_bmpData = 0;
    m_anyItems = false;

    recreateTextBitmap();
}

OverlayFilter::~OverlayFilter()
{
    if (m_bmpData)
    {
        delete m_bmpData;
        m_bmpData = 0;
    }
    //qDebug() << "OverlayFilter::~OverlayFilter";
}

HRESULT OverlayFilter::CheckInputType(const CMediaType* mtIn)
{
    CAutoLock lock(m_pLock);


    if (mtIn->majortype != MEDIATYPE_Video
            /*|| mtIn->formattype != FORMAT_VideoInfo2*/)
    {
            qDebug() << "OverlayFilter::CheckInputType FAIL 1";
        return E_FAIL;
    }

    if( mtIn->subtype != MEDIASUBTYPE_RGB32 &&
        mtIn->subtype != MEDIASUBTYPE_RGB24 &&
        mtIn->subtype != MEDIASUBTYPE_YV12   &&
        mtIn->subtype != MEDIASUBTYPE_YUY2   )
    {
        qDebug() << "OverlayFilter::CheckInputType FAIL 2";

        return E_FAIL;
    }

    m_subtype = mtIn->subtype;
    if ( mtIn->formattype == FORMAT_VideoInfo)
    {
        VIDEOINFO *pvi = (VIDEOINFO *)mtIn->pbFormat;
        m_videoWidth = pvi->bmiHeader.biWidth;
        m_videoHeight = pvi->bmiHeader.biHeight;
        recreateTextBitmap();
    }
    else if ( mtIn->formattype == FORMAT_VideoInfo2)
    {
        VIDEOINFOHEADER2 *pvi = (VIDEOINFOHEADER2 *)mtIn->pbFormat;
        m_videoWidth = pvi->bmiHeader.biWidth;
        m_videoHeight = pvi->bmiHeader.biHeight;
        recreateTextBitmap();
    }
    return S_OK;
}

//bool SaveImage(char* szPathName, void* lpBits, int w, int h)
//{
//    //Create a new file for writing
//    FILE *pFile = fopen(szPathName, "wb");
//    if(pFile == NULL)
//    {
//    return false;
//    }
//    BITMAPINFOHEADER BMIH;
//    BMIH.biSize = sizeof(BITMAPINFOHEADER);
//    BMIH.biSizeImage = w * h * 3;
//    // Create the bitmap for this OpenGL context
//    BMIH.biSize = sizeof(BITMAPINFOHEADER);
//    BMIH.biWidth = w;
//    BMIH.biHeight = h;
//    BMIH.biPlanes = 1;
//    BMIH.biBitCount = 24;
//    BMIH.biCompression = BI_RGB;
//    BMIH.biSizeImage = w * h* 3;
//    BITMAPFILEHEADER bmfh;
//    int nBitsOffset = sizeof(BITMAPFILEHEADER) + BMIH.biSize;
//    LONG lImageSize = BMIH.biSizeImage;
//    LONG lFileSize = nBitsOffset + lImageSize;
//    bmfh.bfType = 'B'+('M'<<8);
//    bmfh.bfOffBits = nBitsOffset;
//    bmfh.bfSize = lFileSize;
//    bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
//    //Write the bitmap file header
//    UINT nWrittenFileHeaderSize = fwrite(&bmfh, 1,
//    sizeof(BITMAPFILEHEADER), pFile);
//    //And then the bitmap info header
//    UINT nWrittenInfoHeaderSize = fwrite(&BMIH,
//    1, sizeof(BITMAPINFOHEADER), pFile);
//    //Finally, write the image data itself
//    //-- the data represents our drawing
//    UINT nWrittenDIBDataSize =
//    fwrite(lpBits, 1, lImageSize, pFile);
//    fclose(pFile);
//    return true;
//}


void OverlayFilter::recreateTextBitmap()
{
  //  if (m_videoWidth == 0 || m_videoHeight == 0)
    if (m_videoWidth <= 320 )
    {
        m_bmpWidth = 256;
        m_bmpHeight = 256;
        m_textSize = 18;

    }
    else
    {
        m_bmpWidth = m_videoWidth;
        m_bmpHeight = m_bmpWidth;
        m_textSize = 36;
    }

    if (m_bmpData)
    {
        delete m_bmpData;
        m_bmpData = 0;
    }
    m_bmpData = new unsigned char[m_bmpWidth * m_bmpHeight * 3];
    memset(m_bmpData,1,m_bmpWidth * m_bmpHeight * 3);
}

HRESULT OverlayFilter::draw(QList<OverlayItem *> &items)
{
    if (items.length() !=0)
        m_anyItems = true;

    HWND w = GetDesktopWindow();
    HDC hdc = GetDC(w);
    HDC hdcBmp = CreateCompatibleDC(hdc);
    HFONT font = CreateFont(45,0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                            CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_MODERN, L"Arial");
    SelectObject(hdcBmp, font);
    // Create a new bitmap that is compatible with the current window
    HBITMAP hbm = CreateCompatibleBitmap(hdc, m_bmpWidth, m_bmpHeight);
    ReleaseDC(w, hdc);
    BITMAP bm;
    HBITMAP hbmOld;
    GetObject(hbm, sizeof(bm), &bm);
    hbmOld = (HBITMAP)SelectObject(hdcBmp, hbm);

    RECT rcText;
    SetRect(&rcText, 0, 0, m_bmpWidth, m_bmpHeight);

    // Draw text using GDI+
    Graphics *graphics = new Graphics(hdcBmp);
    graphics->SetSmoothingMode(SmoothingModeAntiAlias);
    graphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
    SolidBrush colorkey(Color(1,1,1));
    graphics->FillRectangle(&colorkey, rcText.left, rcText.top, rcText.right - rcText.left, rcText.bottom - rcText.top );
    FontFamily fontFamily(L"Arial");
    StringFormat strformat;
    GraphicsPath path;

    //qDebug() << "234";
    for (int i = 0; i < items.length(); i++)
    {
        OverlayItem *item = items[i];
        wchar_t* w = new wchar_t[item->getText().size() + 1];
        int n = item->getText().toWCharArray(w);
        w[n] = 0;
        path.AddString(w, wcslen(w), &fontFamily,  FontStyleRegular,
                       m_textSize, Gdiplus::Point(item->GetX(),item->GetY()), &strformat );
    }

    Pen pen(Color(0,0,0), 2.5);
    graphics->DrawPath(&pen, &path);
    SolidBrush brush(Color(255,255,255));
    graphics->FillPath(&brush, &path);
    delete graphics;

    BITMAPINFO bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = m_bmpWidth;
    bmpInfo.bmiHeader.biHeight = -m_bmpHeight;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 24;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    bmpInfo.bmiHeader.biSizeImage = 0;

   // qDebug() << "OverlayFilter::draw";
    m_cs.Lock();
    // unsigned char *buff = new unsigned char [m_bmpHeight * m_bmpWidth * 3];
     //memset(buff, 255,m_bmpHeight * m_bmpWidth*3 );
    //int result =
    GetDIBits(hdcBmp, hbm, 0, m_bmpHeight, m_bmpData, &bmpInfo, DIB_RGB_COLORS);


   // for ( int )
   // for ( int i = 0; i < m_bmpHeight * m_bmpWidth; i++)
  //      m_bmpData[i] = buff[i * 3];
//    for ( int i = 0; i < m_bmpWidth; i++)
//    {
//        for (int j = 0; j < m_bmpHeight; j++)
//        {
//               m_bmpData[j * m_bmpWidth + i] = buff [i * m_bmpHeight*3 + j  * 3];
//        }
//    }

    //SaveImage("c:\\test\\1.bmp", buff,  m_bmpWidth,  m_bmpHeight);
    // delete buff;
    //qDebug() << "GetDIBits result = " << result;
    m_cs.Unlock();


    DeleteObject(hbm);
    DeleteObject(font);
    DeleteDC(hdcBmp);
    return S_OK;
}



HRESULT OverlayFilter::Transform(IMediaSample *sourceSample, IMediaSample *destSample)
{
    unsigned char *inBuff, *outBuff;

    sourceSample->GetPointer(&inBuff);
    destSample->GetPointer(&outBuff);

    long inSize = sourceSample->GetSize();
    long outSize = destSample->GetSize();


    // Here we need to convert YUY2 -> YV12

   // memcpy(outBuff, inBuff, outSize);
    unsigned char *src = inBuff;
    unsigned char *dst = outBuff;

    if (m_subtype == MEDIASUBTYPE_YUY2)
    {
        //YUY2 -> YV12
        int width = m_videoWidth * 2;
        int height = m_videoHeight;

        // Y
        for (int i = 0; i < inSize/2; i++)
        {
            *dst = *src;
            dst++;
            src+=2;
        }

        // V
        src = inBuff;
        for ( int i = 0; i < height; i+=2)
        {
            for(int j = 0; j < width; j += 4)
            {
                int v = (int)src[j+3] + (int)src[j+width+3];
                *dst = v / 2;
                dst++;
            }
            src += width*2;
        }

        // U
        src = inBuff;
        for ( int i = 0; i < height; i+=2)
        {
            for(int j = 0; j < width; j += 4)
            {
                int u = (int)src[j+1] + (int)src[j+width+1];
                *dst = u / 2;
                dst++;

            }
            src += width*2;
        }
    }
    else if ( m_subtype == MEDIASUBTYPE_RGB32)
    {
        // TODO
    }
    else if (m_subtype == MEDIASUBTYPE_RGB24)
    {
        // TODO
    }
    else
    {
        memcpy(outBuff, inBuff, outSize);
    }

    // Set the size of the destination image.
    //ASSERT(destSample->GetSize() >= cbByte);

    destSample->SetActualDataLength(outSize);

    // sample times
    REFERENCE_TIME startTime, endTime;
    sourceSample->GetTime(&startTime, &endTime);
    destSample->SetTime(&startTime, &endTime);

    if (!m_anyItems)
        return S_OK;

    unsigned char *p = outBuff;
   // output->GetPointer(&p);

    m_cs.Lock();
//    if (m_subtype == MEDIASUBTYPE_YV12)
//    {
        for ( int i = 0; i < m_bmpWidth; i++)
        {
            unsigned char *p1 = p;
            for (int j = 0; j < m_bmpHeight; j++)
            {
              // unsigned char v = m_bmpData[j * m_bmpWidth*3 + i*3];
                unsigned char v = m_bmpData [i * m_bmpHeight*3 + j  * 3];
                if (v != 1)
                 *p1 = v;
               p1++;
            }
            p += m_videoWidth;
        }
//    }
//    else if ( m_subtype == MEDIASUBTYPE_RGB32)
//    {
//        unsigned char *src = m_bmpData;
//        unsigned char *dst = p + (m_videoWidth * 4) * (m_videoHeight - 1);
//        for ( int i = 0; i < m_bmpWidth; i++)
//        {
//            for ( int j = 0; j < m_bmpWidth*3; j++)
//            {
//                if (src[j] != 1)
//                    dst[j] = src[j];
//            }
//            dst -= m_videoWidth*4;
//            src += m_bmpWidth*3;
//        }
//    }
    m_cs.Unlock();

    return S_OK;
}


HRESULT OverlayFilter::GetMediaType(int iPosition, CMediaType *mt)
{
  //qDebug() << "OverlayFilter::GetMediaType";

   ASSERT(m_pInput->IsConnected());

   if (iPosition < 0)
   {
       return E_INVALIDARG;
   }
   else if (iPosition == 0)
   {
       HRESULT hr = m_pInput->ConnectionMediaType(mt);
       if( *mt->FormatType() == FORMAT_VideoInfo)
       {
           VIDEOINFO vi;
           memcpy(&vi, mt->Format(), sizeof(VIDEOINFO));

           VIDEOINFOHEADER2 *vi2 = (VIDEOINFOHEADER2 *)mt->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER2));
           memcpy(&vi2->bmiHeader, &vi.bmiHeader, sizeof(BITMAPINFOHEADER));
           memcpy(&vi2->rcSource, &vi.rcSource, sizeof(RECT));
           memcpy(&vi2->rcTarget, &vi.rcTarget, sizeof(RECT));

           vi2->rcSource.right = vi2->bmiHeader.biWidth;
           vi2->rcSource.bottom = abs(vi2->bmiHeader.biHeight);

           vi2->rcTarget.right = vi2->bmiHeader.biWidth;
           vi2->rcTarget.bottom = abs(vi2->bmiHeader.biHeight);

           vi2->AvgTimePerFrame = vi.AvgTimePerFrame;
           vi2->dwBitRate = 0;
           vi2->dwBitErrorRate = 0;
           vi2->dwCopyProtectFlags = 0;
           vi2->dwPictAspectRatioX = 4;
           vi2->dwPictAspectRatioY = 3;
           vi2->dwInterlaceFlags = 0x81;
           vi2->dwControlFlags = 0;


           mt->SetFormatType(&FORMAT_VideoInfo2);

           if (*mt->Subtype() == MEDIASUBTYPE_YUY2)
           {
                mt->SetSubtype(&MEDIASUBTYPE_YV12);
                mt->SetSampleSize( mt->GetSampleSize() / 1.5);
                vi2->dwControlFlags = 0;
                vi2->bmiHeader.biCompression = MAKEFOURCC('Y','V', '1', '2');
                vi2->bmiHeader.biBitCount = 12;
                vi2->bmiHeader.biSizeImage = mt->GetSampleSize();
           }
           else if (*mt->Subtype() == MEDIASUBTYPE_RGB24)
           {
                mt->SetSubtype(&MEDIASUBTYPE_YV12);
                mt->SetSampleSize( mt->GetSampleSize() /2);
                vi2->dwControlFlags = 0;
                vi2->bmiHeader.biCompression = MAKEFOURCC('Y','V', '1', '2');
                vi2->bmiHeader.biBitCount = 12;
                vi2->bmiHeader.biSizeImage = mt->GetSampleSize();
           }
           else if (*mt->Subtype() == MEDIASUBTYPE_RGB32)
           {
                mt->SetSubtype(&MEDIASUBTYPE_YV12);
                mt->SetSampleSize( mt->GetSampleSize() /3);
                vi2->dwControlFlags = 0;
                vi2->bmiHeader.biCompression = MAKEFOURCC('Y','V', '1', '2');
                vi2->bmiHeader.biBitCount = 12;
                vi2->bmiHeader.biSizeImage = mt->GetSampleSize();
           }
       }
       else if (*mt->FormatType() == FORMAT_VideoInfo2 )
       {
           if (*mt->Subtype() == MEDIASUBTYPE_YUY2)
           {
                mt->SetSubtype(&MEDIASUBTYPE_YV12);
                mt->SetSampleSize( mt->GetSampleSize() / 1.5);
                VIDEOINFOHEADER2 *vi2 = (VIDEOINFOHEADER2 *)mt->Format();
                vi2->dwControlFlags = 0;
                vi2->bmiHeader.biCompression = MAKEFOURCC('Y','V', '1', '2');
                vi2->bmiHeader.biBitCount = 12;
                vi2->dwPictAspectRatioX = 4;
                vi2->dwPictAspectRatioY = 3;
                vi2->bmiHeader.biSizeImage = mt->GetSampleSize();

           }
       }
       return hr;
   }
   return VFW_S_NO_MORE_ITEMS;
}

HRESULT OverlayFilter::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
    return S_OK;
}

HRESULT OverlayFilter::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProp)
{
    //qDebug() << "OverlayFilter::DecideBufferSize";

    // Make sure the input pin connected.
    if (!m_pInput->IsConnected())
    {
        return E_UNEXPECTED;
    }
    // Our strategy here is to use the upstream allocator as the guideline, but
    // also defer to the downstream filter's request when it's compatible with us.

    // First, find the upstream allocator...
    ALLOCATOR_PROPERTIES InputProps;

    IMemAllocator *pAllocInput = 0;
    HRESULT hr = m_pInput->GetAllocator(&pAllocInput);

    if (FAILED(hr))
    {
        return hr;
    }
    // ... now get the properters
    hr = pAllocInput->GetProperties(&InputProps);
    pAllocInput->Release();
    if (FAILED(hr))
    {
        return hr;
    }
    // Buffer alignment should be non-zero [zero alignment makes no sense!]
    if (pProp->cbAlign == 0)
    {
        pProp->cbAlign = 1;
    }
    // Number of buffers must be non-zero
    if (pProp->cbBuffer == 0)
    {
        pProp->cBuffers = 1;
    }
    // For buffer size, find the maximum of the upstream size and
    // the downstream filter's request.
    pProp->cbBuffer = max(InputProps.cbBuffer, pProp->cbBuffer);

    // Now set the properties on the allocator that was given to us,
    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pProp, &Actual);
    if (FAILED(hr))
    {
         return hr;
    }

    // Even if SetProperties succeeds, the actual properties might be
    // different than what we asked for. We check the result, but we only
    // look at the properties that we care about. The downstream filter
    // will look at them when NotifyAllocator is called.
    if (InputProps.cbBuffer > Actual.cbBuffer)
    {
        return E_FAIL;
    }
    return S_OK;
}
