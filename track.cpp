#include "track.h"

#include <QAudioDecoder>

Track::Track(QWidget *parent)
    : QCustomPlot(parent)
    , decoder(new QAudioDecoder(this))
{
    wavePlot = addGraph();
    setMinimumHeight(50);
    setInteraction(QCP::iRangeDrag, true);
    setInteraction(QCP::Interaction::iRangeZoom, true);
    connect(decoder, SIGNAL(bufferReady()), this, SLOT(setBuffer()));
    connect(decoder, SIGNAL(finished()), this, SLOT(plot()));
}

Track::~Track()
{
    delete decoder;
    // wavePlot delete auto ?
}

void Track::setSource(const QString &fileName)
{
    samples.clear();
    clearItems();
    clearGraphs();
    buffer = *new QAudioBuffer();
    wavePlot = addGraph();
    decoder->setSourceFilename(fileName);
    decoder->start();
}

void Track::setBuffer()
{
    buffer = decoder->read();
    qreal peak = getPeakValue(buffer.format());
    const qint16 *data = buffer.constData<qint16>();
    int count = buffer.sampleCount() / 1;
    for (int i=0; i<count; i++){
        double val = data[i]/peak;
        samples.append(val);
    }
}

void Track::plot()
{
    QVector<double> x(samples.size());
    for (int i=0; i<x.size(); i++)
        x[i] = i;
    wavePlot->addData(x, samples);
    yAxis->setRange(QCPRange(-1, 1));
    xAxis->setRange(QCPRange(0, samples.size()));
    replot();
}

/**
 * https://stackoverflow.com/questions/46947668/draw-waveform-from-raw-data-using-qaudioprobe
 * @brief Track::getPeakValue
 * @param format
 * @return The peak value
 */
qreal Track::getPeakValue(const QAudioFormat &format)
{
    qreal ret(0);
    if (format.isValid()){
        switch (format.sampleType()) {
            case QAudioFormat::Unknown:
            break;
            case QAudioFormat::Float:
                if (format.sampleSize() != 32) // other sample formats are not supported
                    ret = 0;
                else
                    ret = 32767;//1.00003;
            break;
            case QAudioFormat::SignedInt:
                if (format.sampleSize() == 32)
#ifdef Q_OS_WIN
                    ret = INT_MAX;
#endif
#ifdef Q_OS_UNIX
                    ret = SHRT_MAX;
#endif
                else if (format.sampleSize() == 16)
                    ret = SHRT_MAX;
                else if (format.sampleSize() == 8)
                    ret = CHAR_MAX;
                break;
            case QAudioFormat::UnSignedInt:
                if (format.sampleSize() == 32)
                    ret = UINT_MAX;
                else if (format.sampleSize() == 16)
                    ret = USHRT_MAX;
                else if (format.sampleSize() == 8)
                    ret = UCHAR_MAX;
            break;
        default:
            break;
        }
    }
    return ret;
}
