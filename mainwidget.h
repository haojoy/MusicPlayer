#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QPainter>
#include <QDebug>
#include <QTimer>

#include "switchanimation.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public SwitchAnimation
{
    Q_OBJECT
private:
    bool dragging;
    QPoint dragStartPosition;

private:
    //void paintEvent(QPaintEvent *event) override;
    void loadStyleSheet(const QString &styleSheetFile);
    // 界面初始化处理
    void init_HandleUI();
    // 连接信号和槽
    void init_HandleSignalsAndSlots();

    // 显示歌词页面
    void showLyricsPage();

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

    void on_btn_albumpic_clicked();

    void on_btn_down_clicked();

private:
    Ui::MainWidget *ui;
};
#endif // MAINWIDGET_H
