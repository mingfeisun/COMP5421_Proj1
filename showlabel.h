#ifndef SHOWLABEL_H
#define SHOWLABEL_H
#include <QLabel>
#include <QMouseEvent>

class showLabel : public QLabel
{
    Q_OBJECT
public:
    showLabel(QWidget* parent =0);

    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseDoubleClickEvent(QMouseEvent *ev);

signals:
    void mouseClick(int, int);
    void mouseMove(int, int);
    void mouseDClick(int, int);
};

#endif // SHOWLABEL_H
