#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QPainter>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT
private:
    bool dragging;
    QPoint dragStartPosition;

private:
    //void paintEvent(QPaintEvent *event) override;
    void loadStyleSheet(const QString &styleSheetFile);
protected:
    //实现窗口可拖动
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private slots:
    void on_btn_close_clicked();

    void on_btn_close_2_clicked();

private:
    Ui::MainWidget *ui;
};
#endif // MAINWIDGET_H
