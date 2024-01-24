#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H
#include <QGraphicsView>

class myGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit myGraphicsView(QWidget* parent = nullptr);
    void wheelEvent(QWheelEvent* event);  // 重写鼠标缩放
    void expand();                                        // 放大
    void shrink();                                          // 缩小
    void refresh();                                         // 刷新

private:
    void zoom(double);                                 // 尺寸变化

signals:

};

#endif // MYGRAPHICSVIEW_H
