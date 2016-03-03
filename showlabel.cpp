#include "showlabel.h"

showLabel::showLabel(QWidget* parent):
    QLabel(parent) {
}

void showLabel::mouseMoveEvent(QMouseEvent *ev)
{
    int x=ev->x();
    int y=ev->y();
    emit mouseMove(x, y);
}

void showLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    int x=ev->x();
    int y=ev->y();
    emit mouseClick(x, y);
}

void showLabel::mouseDoubleClickEvent(QMouseEvent *ev)
{
    int x=ev->x();
    int y=ev->y();
    emit mouseDClick(x, y);
}
