#include <QHeaderView>
#include <QStyledItemDelegate>
#include "myTableWidget.h"


 MyTableWidget::MyTableWidget(QWidget *parent)
    : QTableWidget(parent), hoverDelegate(new HoverDelegate(this))
{
    setMouseTracking(true);
    setItemDelegate(hoverDelegate);
}

void MyTableWidget::handleHoverEvent(int row){
    if (row != hoverDelegate->hoverRow)
    {
        hoverDelegate->setHoverRow(row);
        viewport()->update();
    }

}

void MyTableWidget::mouseMoveEvent(QMouseEvent* event){
    int row = rowAt(event->pos().y());
    handleHoverEvent(row);
    QTableWidget::mouseMoveEvent(event);

}


void MyTableWidget::wheelEvent(QWheelEvent *event){
    int row = rowAt(event->pos().y());
    handleHoverEvent(row);
    QTableWidget::wheelEvent(event);
}
