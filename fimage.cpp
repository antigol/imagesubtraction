#include "fimage.h"

FImage::FImage()
    : width(0), height(0)
{
}

FImage::FImage(const QImage &i)
{
    data.reserve(i.width() * i.height());
    for (int y = 0; y < i.height(); ++y)
        for (int x = 0; x < i.width(); ++x)
            data << qGray(i.pixel(x, y)) / 255.0; // [0,1]
    width = i.width();
    height = i.height();
}

float FImage::pixel(int x, int y) const
{
    return data[y * width + x];
}

float &FImage::pixel(int x, int y)
{
    return data[y * width + x];
}

QImage FImage::render(float min, float max) const
{
    QImage out(width, height, QImage::Format_RGB32);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float w = pixel(x, y);
            w = (w - min) / (max - min);
            // noir -> jaune -> blanc
            int r, g, b;
            if (w < 0.0) {
                r = g = b = 0;
            } else if (w < 0.5) {
                r = g = w * 510;
                b = 0;
            } else if (w < 1.0) {
                r = g = 255;
                b = w * 510 - 255;
            } else {
                r = g = b = 255;
            }
            out.setPixel(x, y, qRgb(r, g, b));
        }
    }
    return out;
}

void FImage::subtract(const FImage &a, const FImage &b, float f, FImage &c)
{
    Q_ASSERT(a.data.size() == b.data.size());

    c.width = a.width;
    c.height = a.height;
    c.data.resize(a.data.size());
    for (int i = 0; i < a.data.size(); ++i) {
        c.data[i] = a.data[i] - b.data[i] * f;
    }
}

void FImage::subtract(const FImage &a, float f, const FImage &b, FImage &c)
{
    Q_ASSERT(a.data.size() == b.data.size());

    c.width = a.width;
    c.height = a.height;
    c.data.resize(a.data.size());
    for (int i = 0; i < a.data.size(); ++i) {
        c.data[i] = a.data[i] * f - b.data[i];
    }
}

float sgfilter_(const FImage &in, int x, int y, int dx, int dy)
{
    float tm3 = in.pixel(qMax(x - 3 * dx, 0), qMax(y - 3 * dy, 0));
    float tm2 = in.pixel(qMax(x - 2 * dx, 0), qMax(y - 2 * dy, 0));
    float tm1 = in.pixel(qMax(x - 1 * dx, 0), qMax(y - 1 * dy, 0));
    float t0 = in.pixel(x, y);
    float tp1 = in.pixel(qMin(x + 1 * dx, in.width - 1), qMin(y + 1 * dy, in.height - 1));
    float tp2 = in.pixel(qMin(x + 2 * dx, in.width - 1), qMin(y + 2 * dy, in.height - 1));
    float tp3 = in.pixel(qMin(x + 3 * dx, in.width - 1), qMin(y + 3 * dy, in.height - 1));

    return (-2.0 * tm3 + 3.0 * tm2 + 6.0 * tm1 + 7.0 * t0 + 6.0 * tp1 + 3.0 * tp2 - 2.0 * tp3) / 21.0;
}

void FImage::sgfilter(const FImage &in, FImage &out)
{
    out.width = in.width;
    out.height = in.height;
    out.data.resize(in.data.size());
    for (int y = 0; y < in.height; ++y) {
        for (int x = 0; x < in.width; ++x) {
            float sgx = sgfilter_(in, x, y, 1, 0);
            float sgy = sgfilter_(in, x, y, 0, 1);
            out.pixel(x, y) = (sgx + sgy) / 2.0;
        }
    }
}
