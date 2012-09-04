#ifndef FIMAGE_H
#define FIMAGE_H

#include <QVector>
#include <QImage>

class FImage
{
public:
    FImage();
    explicit FImage(const QImage &i);

    float pixel(int x, int y) const;
    float &pixel(int x, int y);

    QImage render(float min, float max) const;

    QVector<float> data;
    int width;
    int height;

    static void subtract(const FImage &a, const FImage &b, float f, FImage &c); // c = a - b*f
    static void subtract(const FImage &a, float f, const FImage &b, FImage &c); // c = a*f - b
    static void sgfilter(const FImage &in, FImage &out);
};

#endif // FIMAGE_H
