#include "vumeter.h"
#include <QPainter>
#include <math.h>

VUMeter::VUMeter(QObject *parent) :
    QObject(parent)
{
    m_sg = 0;
    //m_logger = Logger::GetInstance();
    resetValues();
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(on_timer()));
    m_timer->start(100);
    m_prev = 0;
    m_cnt = 0;
}

VUMeter::~VUMeter()
{
    if (m_timer)
     delete m_timer;
}

void VUMeter::resetValues()
{
    m_avgR = 0;
    m_avgL = 0;
    m_peakR = 1;
    m_peakL = 1;
}


void VUMeter::setGrabber(ISampleGrabber *sg)
{
    m_sg = sg;
    m_sg->SetCallback(this, 1);
}

void VUMeter::onPause()
{
    resetValues();
}

void VUMeter::onStop()
{
    resetValues();
}


ULONG STDMETHODCALLTYPE VUMeter::AddRef()
{
    return 1;
}

ULONG STDMETHODCALLTYPE VUMeter::Release()
{
    return 1;
}

HRESULT STDMETHODCALLTYPE   VUMeter::QueryInterface(REFIID riid, void **ppvObject)
{
    if (NULL == ppvObject) return E_POINTER;
    if (riid == __uuidof(IUnknown))
    {
        *ppvObject = static_cast<IUnknown*>(this);
         return S_OK;
    }
    if (riid == IID_ISampleGrabberCB)
    {
        *ppvObject = static_cast<ISampleGrabberCB*>(this);
         return S_OK;
    }
    return E_NOTIMPL;
}


STDMETHODIMP  VUMeter::SampleCB( double sampleTime, IMediaSample *sample)
{
   // m_logger->AddLogMessage(INFO, "VUMeter::SampleCB(), sampleTime=" +  QString::number(sampleTime));
    return S_OK;
}

STDMETHODIMP  VUMeter::BufferCB( double sampleTime, BYTE *buffer, long len)
{

   // m_logger->AddLogMessage(INFO, "VUMeter::BufferCB(), sampleTime=" +  QString::number(sampleTime));
    m_cnt++;
    try {
       unsigned short *e = (unsigned short *)buffer;
       int size = len / 4;
       int leftS = 0;
       int rightS = 0;
       int avgR = 0;
       int avgL = 0;
       int peakR = 0;
       int peakL = 0;

              for(int i = 0; i < size; i += 2)
              {
                leftS = abs(e[i]) ;
                avgL += leftS;
                if(leftS > peakL)
                {
                   peakL = leftS;
                }
                rightS = abs(e[i + 1]);
                avgR += rightS;
                if(rightS > peakR)
                {
                   peakR = rightS;
                }
              }

              m_avgR = size == 0 ? avgR : avgR / size;
              m_avgL = size == 0 ? avgL : avgL / size;
              m_peakR = peakR;
              m_peakL = peakL;


//       double mult = 6.0;
//       double offset = 12.0;
//       m_avgR = log10((double)m_avgR) * mult - offset;
//       m_avgL = log10((double)m_avgL) * mult - offset;
//       m_peakL = log10((double)m_peakL) * mult - offset;
//       m_peakR = log10((double)m_peakR) * mult - offset;

//       m_avgR = (int)((Math.Log10(volumeAvgR)*
//                           DbMultiplier)- DbOffset);
//               avgL = (int)((Math.Log10(volumeAvgL)*
//                           DbMultiplier)- DbOffset);
//               peakR = (int)((Math.Log10(volumePeakR)*
//                           DbMultiplier)- DbOffset);
//               peakL = (int)((Math.Log10(volumePeakL)*
//                           DbMultiplier)- DbOffset);
    }
    catch(...)
    {
        //m_logger->AddLogMessage(INFO, "VUMeter::BufferCB(), exception thrown");
    }

    return S_OK;
}

void VUMeter::getValues(int channel, int *avg, int *peak)
{
    if (!channel)
    {
      *avg = m_avgL;
      *peak = m_peakL;
    }
    else
    {
      *avg = m_avgR;
      *peak = m_peakR;
    }
}

void VUMeter::on_timer()
{
  //  m_logger->AddLogMessage(INFO, "VUMeter::on_timer()");
    if (m_prev == m_cnt)
         resetValues();
    m_prev = m_cnt;
}
