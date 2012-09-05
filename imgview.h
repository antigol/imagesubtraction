#ifndef IMGVIEW_H
#define IMGVIEW_H

#include <QLabel>

class IMGView : public QLabel
{
    Q_OBJECT
public:
    explicit IMGView(QWidget *parent = 0);

signals:
    void clickOnPixmap(int x, int y);

protected:
    virtual void mousePressEvent(QMouseEvent *ev);
};

#endif // IMGVIEW_H
