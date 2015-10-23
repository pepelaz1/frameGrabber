#include "textoverlay.h"
#include <QDebug>
#include <Gdiplus.h>
#include "moreuuids.h"

#pragma comment (lib,"Gdiplus.lib")
using namespace Gdiplus;

TextOverlay::TextOverlay(QObject *parent) :
    QObject(parent)
{
   // qDebug() << "TextOverlay::TextOverlay";
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
    m_ovl = 0;
}

TextOverlay::~TextOverlay()
{
    clear();
    if (m_ovl)
    {
      //  delete m_ovl;
        m_ovl = 0;
    }
    m_vmr.Release();
    m_mb9.Release();
    m_bv.Release();
    GdiplusShutdown(m_gdiplusToken);
   // qDebug() << "TextOverlay::~TextOverlay";
}

HRESULT TextOverlay::setGraphBuilder(CComPtr<IGraphBuilder> gb)
{
    m_gb = gb;
    return S_OK;
}

HRESULT TextOverlay::setRenderer(CComPtr<IBaseFilter> vmr)
{
    m_vmr = vmr;
    if (m_vmr != 0)
    {
        m_vmr.QueryInterface(&m_mb9);
        m_vmr.QueryInterface(&m_bv);
    }
    return S_OK;
}

HRESULT TextOverlay::setEncoder(CComPtr<IBaseFilter> vc)
{
    m_vc = vc;
    return S_OK;
}

HRESULT TextOverlay::addFilter()
{
    HRESULT hr = S_OK;
    CComPtr<IUnknown>unk;
    m_ovl = new OverlayFilter(L"Overlay filter", unk, &hr);
    if (FAILED(hr))
        return hr;



    IBaseFilter *ovl = (IBaseFilter *)m_ovl;
    hr = m_gb->AddFilter(ovl, L"Overlay filter");
    if (FAILED(hr))
        return hr;

    return hr;
}


//HRESULT TextOverlay::configure(CComPtr<IGraphBuilder> gb, CComPtr<IBaseFilter>vmr, CComPtr<IBaseFilter>vc)
//{
//    HRESULT hr = S_OK;
//    m_gb = gb;
//    m_vc = vc;
//    m_vmr = vmr;
//    if (m_vmr != 0)
//    {
//        m_vmr.QueryInterface(&m_mb9);
//        m_vmr.QueryInterface(&m_bv);
//    }
//   // return insertOverlayFilter();

//    CComPtr<IUnknown>unk;
//    m_ovl = new OverlayFilter(L"Overlay filter", unk, &hr);
//    if (FAILED(hr))
//        return hr;



//    IBaseFilter *ovl = (IBaseFilter *)m_ovl;
//    hr = m_gb->AddFilter(ovl, L"Overlay filter");
//    if (FAILED(hr))
//        return hr;

//    return hr;
//}

IBaseFilter *TextOverlay::getFilter()
{
    return (IBaseFilter *)m_ovl;
}

HRESULT TextOverlay::insertOverlayFilter()
{
    if (m_vc == 0)
        return S_FALSE;

   // qDebug() << "m_vc = " << m_vc;
    // Find video compressor's input pin and video decompressor's output pin

    HRESULT hr = S_OK;
    CComPtr<IEnumPins>enm;
    m_vc->EnumPins(&enm);

    CComPtr<IPin>ipin;
    CComPtr<IPin>opin;

    ULONG ftch = 0;
    while(enm->Next(1,&ipin,&ftch) == S_OK)
    {
        PIN_DIRECTION pd;
        ipin->QueryDirection(&pd);
        if (pd == PINDIR_INPUT)
        {
           hr = ipin->ConnectedTo(&opin);
         //  qDebug() << "hr = " << hr;
           break;
        }
       ipin.Release();
    }

    // Disconnect decoder and encoder
    hr = ipin->Disconnect();
    //qDebug() << "hr = " << hr;
    if (FAILED(hr))
        return hr;

    hr = opin->Disconnect();
    //qDebug() << "hr = " << hr;
    if (FAILED(hr))
        return hr;


    CComPtr<IUnknown>unk;
    m_ovl = new OverlayFilter(L"Overlay filter", unk, &hr);
    //qDebug() << "hr = " << hr;
    if (FAILED(hr))
        return hr;



    IBaseFilter *ovl = (IBaseFilter *)m_ovl;
    hr = m_gb->AddFilter(ovl, L"Overlay filter");
   // qDebug() << "hr = " << hr;
    if (FAILED(hr))
        return hr;


    CComPtr<IPin>iopin;
    CComPtr<IPin>oopin;
    CComPtr<IPin>pin;

    ovl->EnumPins(&enm);
    //qDebug() << "enm = " << enm;
    ftch = 0;
    while(enm->Next(1,&pin,&ftch) == S_OK)
    {
        PIN_DIRECTION pd;
        pin->QueryDirection(&pd);
        if (pd == PINDIR_INPUT)
           iopin = pin;
        else
           oopin = pin;

       pin.Release();
    }

    hr = m_gb->Connect(opin, iopin);
   // qDebug() << "hr = " << hr;
    if (FAILED(hr))
        return hr;

    hr = m_gb->Connect(oopin, ipin);
    //qDebug() << "hr = " << hr;
    if (FAILED(hr))
        return hr;
   // qDebug() << "hr = " << hr;

    //hr = m_gb->Connect(opin, ipin);
    //qDebug() << "hr = " << hr;

    ipin.Release();
    opin.Release();
    return S_OK;
}

HRESULT TextOverlay::draw()
{
    LONG cx, cy;
    HRESULT hr;

    m_bv->get_VideoWidth(&cx);
    m_bv->get_VideoHeight(&cy);

    HWND w = GetDesktopWindow();
    HDC hdc = GetDC(w);
    HDC hdcBmp = CreateCompatibleDC(hdc);

    HFONT font = CreateFont(45,0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                            CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_MODERN, __T("Arial"));
    SelectObject(hdcBmp, font);

    //int nLength, bmpWidth, bmpHeight;
    //SIZE sz={0};
   // nLength = (int) _tcslen(text);
   // GetTextExtentPoint32(hdcBmp, text, nLength, &sz);
    //bmpHeight = sz.cy + 300;
    //bmpWidth  = sz.cx + 300;

    int bmpHeight = 500;
    int bmpWidth = 500;

    // Create a new bitmap that is compatible with the current window
    HBITMAP hbm = CreateCompatibleBitmap(hdc, bmpWidth, bmpHeight);
    ReleaseDC(w, hdc);
    BITMAP bm;
    HBITMAP hbmOld;
    GetObject(hbm, sizeof(bm), &bm);
    hbmOld = (HBITMAP)SelectObject(hdcBmp, hbm);

    RECT rcText;
    SetRect(&rcText, 0, 0, bmpWidth, bmpHeight);

    // GDI way (currenly GDI+ is used )
    //SetBkColor(hdcBmp, RGB(1, 1, 1)); // Pure white background
    //SetTextColor(hdcBmp, RGB(255,255,255));      // Write text with requested color
    //TextOut(hdcBmp, 0, 0, text, nLength);   // Draw the requested text string onto the bitmap

    Graphics *graphics = new Graphics(hdcBmp);
    graphics->SetSmoothingMode(SmoothingModeAntiAlias);
    graphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);

    SolidBrush colorkey(Color(1,1,1));
    graphics->FillRectangle(&colorkey, rcText.left, rcText.top, rcText.right - rcText.left, rcText.bottom - rcText.top );

    FontFamily fontFamily(L"Arial");
    StringFormat strformat;
    GraphicsPath path;

    for (int i = 0; i < m_items.length(); i++)
    {
        OverlayItem *item = m_items[i];
        wchar_t* w = new wchar_t[item->getText().size() + 1];
        int n = item->getText().toWCharArray(w);
        w[n] = 0;
        path.AddString(w, wcslen(w), &fontFamily,  FontStyleRegular,
                       35, Gdiplus::Point(item->GetX(),item->GetY()), &strformat );
    }

    Pen pen(Color(0,0,0), 2.5);
    graphics->DrawPath(&pen, &path);
    SolidBrush brush(Color(255,255,255));
    graphics->FillPath(&brush, &path);
    delete graphics;

    // Configure the VMR's bitmap structure
    VMR9AlphaBitmap bmpInfo;
    ZeroMemory(&bmpInfo, sizeof(bmpInfo) );
    bmpInfo.dwFlags = VMRBITMAP_HDC | VMR9AlphaBitmap_SrcColorKey;
    bmpInfo.hdc = hdcBmp;  // DC which has selected our bitmap

    int x = 5;
    int y = 5;
    double xf = (double)x/(double)cx;
    double yf = (double)y/(double)cy;

    double kx = cx < 720 ? 0.5 : (double)cx / 1280.0;
    double ky = cy < 480 ? 0.5 : (double)cy / 720.0;

    bmpInfo.rDest.left = xf;
    bmpInfo.rDest.top = yf;
    bmpInfo.rDest.right = xf + (double)bm.bmWidth/(double)cx * kx;
    bmpInfo.rDest.bottom = yf + (double)bm.bmHeight/(double)cy * ky;
    bmpInfo.rSrc = rcText;

    // Transparency value 1.0 is opaque, 0.0 is transparent.
    bmpInfo.fAlpha = 1.0f;
    bmpInfo.clrSrcKey = RGB(1,1,1);

   // Give the bitmap to the VMR for display
    hr = m_mb9->SetAlphaBitmap(&bmpInfo);
   // Clean up resources
    DeleteObject(hbm);
    DeleteObject(font);
    DeleteDC(hdcBmp);

    return hr;
}

void TextOverlay::add(int x, int y, QString text)
{
    //qDebug() << "TextOverlay::add";
    OverlayItem *item = new OverlayItem(x, y, text);
    m_items.push_back(item);
}

HRESULT TextOverlay::apply()
{
    //qDebug() << "TextOverlay::apply";
    if (m_ovl)
        m_ovl->draw(m_items);

    if (m_vmr == 0)
        return S_OK;

    return draw();
}

void TextOverlay::clear()
{
    //qDebug() << "TextOverlay::clear";

    for(int i = 0; i < m_items.count(); i++)
        delete m_items[i];
    m_items.clear();
}
