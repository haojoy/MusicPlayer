#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QPainter>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "music.h"
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
    void paintEvent(QPaintEvent *event) override;
    void loadStyleSheet(const QString &styleSheetFile);
    // 界面初始化处理
    void init_HandleUI();
    // 连接信号和槽
    void init_HandleSignalsAndSlots();

    // 显示主界面
    void showHomePage();
    // 显示歌词页面
    void showLyricsPage();

    // 搜索音乐
    void musicSearch(const QString &keystring);
    void setSearchResultTable(SongList songs);


    QString msecondToString(int mseconds);


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

    void on_btn_close2_clicked();

    void on_btn_albumpic_clicked();

    void on_btn_down_clicked();

    void on_btn_favorite_clicked();

    void on_btn_recently_clicked();

    void on_btn_songList_clicked();

    void on_btn_localsong_clicked();

    void on_btn_minsize_clicked();

    void on_btn_maxsize_clicked();

    // 搜索歌曲
    void on_btn_search_clicked();

    void handleNetworkResponseData(QNetworkReply *reply);

    void on_btn_setskin_clicked();

private:
    Ui::MainWidget *ui;
    QNetworkAccessManager *networkManager;


    QMediaPlayer *player;            // 播放器
    QMediaPlaylist *playlist;        // 播放列表
    SongList songSearchResult;
    PlayListList PlayListSearchResult;

};
#endif // MAINWIDGET_H
