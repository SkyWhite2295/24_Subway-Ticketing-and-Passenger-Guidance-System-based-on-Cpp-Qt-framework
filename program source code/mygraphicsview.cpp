#include "mygraphicsview.h"
#include <cmath>
#include <QDebug>
#include <qmath.h>
#include <QWheelEvent>
#include <qscrollbar.h>

myGraphicsView::myGraphicsView(QWidget* parent) : QGraphicsView(parent){}

//处理滚轮事件
void myGraphicsView::wheelEvent(QWheelEvent* event)
{
    auto step = event->angleDelta().y();
    //QPointF cursorPoint = event->position();
    QPointF cursorPoint(event->x(), event->y());
    QPointF scenePos = this->mapToScene(cursorPoint.x(), cursorPoint.y());
    qreal factor = 1 + ((qreal)step / (qreal)viewport()->height()); scale(factor, factor);

    QPointF viewPoint = this->transform().map(scenePos);
    horizontalScrollBar()->setValue(viewPoint.x() - cursorPoint.x());
    verticalScrollBar()->setValue(viewPoint.y() - cursorPoint.y());

}

void myGraphicsView::shrink()
{
    zoom(1.0/1.2);
}

void myGraphicsView::expand()
{
    zoom(1.2);
}

void myGraphicsView::zoom(double scaleFactor)
{
    //缩放函数
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.1 || factor > 50)
        return;
    scale(scaleFactor, scaleFactor);
}

void myGraphicsView::refresh()
{
    this->viewport()->update();
}
