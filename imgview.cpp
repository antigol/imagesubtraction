#include "imgview.h"
#include <QMouseEvent>

IMGView::IMGView(QWidget *parent) :
    QLabel(parent)
{
}

void IMGView::mousePressEvent(QMouseEvent *ev)
{
    QLabel::mousePressEvent(ev);

    if (pixmap()) {
        QPoint pos = ev->pos();
        int x = 0;
        int y = 0;

        if (alignment() & Qt::AlignLeft)
            x = pos.x();
        if (alignment() & Qt::AlignRight)
            x = pos.x() - width() + pixmap()->width();
        if (alignment() & Qt::AlignHCenter)
            x = pos.x() - (width() - pixmap()->width()) / 2.0;

        if (alignment() & Qt::AlignTop)
            y = pos.y();
        if (alignment() & Qt::AlignBottom)
            y = pos.y() - height() + pixmap()->height();
        if (alignment() & Qt::AlignVCenter)
            y = pos.y() - (height() - pixmap()->height()) / 2.0;

        if (x >= 0 && x < pixmap()->width()
                && y >= 0 && y < pixmap()->height())
            emit clickOnPixmap(x, y);
    }
}
