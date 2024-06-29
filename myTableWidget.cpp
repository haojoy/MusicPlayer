#include "myTableWidget.h"

MyTableWidget::MyTableWidget(QWidget *parent):QTableWidget(parent){

    setMouseTracking(true); // 启用鼠标跟踪

}


void MyTableWidget::mouseMoveEvent(QMouseEvent *event){
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        clearSelection(); // 清除之前的选择
        selectRow(index.row()); // 选中当前悬停的行
    }
    QTableWidget::mouseMoveEvent(event);
}
