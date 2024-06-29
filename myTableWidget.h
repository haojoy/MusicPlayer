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

};


#endif // MYTABLEWIDGET_H
