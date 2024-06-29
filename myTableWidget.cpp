#include <QHeaderView>
#include <QDebug>
#include "myTableWidget.h"

MyTableWidget::MyTableWidget(QWidget *parent):QTableWidget(parent){

    setMouseTracking(true); // 启用鼠标跟踪

}

void MyTableWidget::handleHoverEvent(const QPoint &pos){
    QModelIndex index = indexAt(pos);
    if (index.isValid()) {
        clearSelection(); // 清除之前的选择
        selectRow(index.row()); // 选中当前悬停的行
    }

}

void MyTableWidget::mouseMoveEvent(QMouseEvent *event){
    handleHoverEvent(event->pos());
    QTableWidget::mouseMoveEvent(event);
}

// void MyTableWidget::wheelEvent(QWheelEvent *event){
//     handleHoverEvent(event->pos());
//     QTableWidget::wheelEvent(event);
// }
