#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H

#include <QTableWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QDebug>

// TableWidget 悬停时高亮整行
struct HoverDelegate : public QStyledItemDelegate {
    HoverDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent), hoverRow(-1) {}

    void setHoverRow(int row) {
        hoverRow = row;
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        if (index.row() == hoverRow)
        {
            painter->save();
            painter->fillRect(option.rect, QColor(255, 255, 255, 127));
            painter->restore();
        }
        QStyledItemDelegate::paint(painter, option, index);
    }
    int hoverRow;
};

class MyTableWidget : public QTableWidget {
    Q_OBJECT

public:
    explicit MyTableWidget(QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    void handleHoverEvent(int row);

private:
    HoverDelegate* hoverDelegate;
};


#endif // MYTABLEWIDGET_H
