#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H

#include <QTableWidget>
#include <QMouseEvent>

class MyTableWidget : public QTableWidget {
    Q_OBJECT

public:
    explicit MyTableWidget(QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    // void wheelEvent(QWheelEvent *event) override;
private:
    void handleHoverEvent(const QPoint &pos);

private:
    int currentHoveredRow = -1;
};


#endif // MYTABLEWIDGET_H
