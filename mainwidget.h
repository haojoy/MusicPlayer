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
#include <QDir>
#include <QTableWidgetItem>
#include <QMenu>
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
    enum{
        titleCol,
        artistCol,
        albumCol,
        durationCol
    };
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

    QString songPath(const Music &music) const;
    QString lyricPath(const Music &music) const;
    QString coverPath(const Music &music) const;
    bool isSongDownloaded(Music music);

    // 搜索音乐
    void musicSearch(const QString &keystring);
    void setSearchResultTable(SongList songs);


    void downloadFile(const QString &url, const Music &music);
    QString formatSongInfo(const Music &music);
    void handleDownloadFailure(const Music &music);
    // 下载音乐
    void playLocalSong(Music music);
    void addDownloadSong(Music music);
    void downloadNext();
    void downloadSong(Music music);
    //void downloadSongLyric(Music music);
    //void downloadSongCover(Music music);

    // 播放音乐
    void startPlaySong(Music music);
    //void playNext();
    void appendOrderSongs(SongList musics);
    //void removeOrderSongs(SongList musics);
    void appendNextSongs(SongList musics);
    void appendMusicToPlayList(SongList musics, int row);
    //void ClearPlayList();

    void addFavorite(SongList musics);
    void removeFavorite(SongList musics);


    QString msecondToString(int mseconds);


    // 保存配置
    void saveSongList(QString key, const SongList &songs);
    void savePlayList(QString key, const PlayListList &playlistlist);
    // 读取配置
    void restoreSongList(QString key, SongList &songs);
    void restorePlayList(QString key, PlayListList &playlistlist);


    void handleContextMenuRequest(QTableWidget* table, const QPoint& pos);
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

    void on_btn_logo_clicked();

    void on_tableWidget_search_customContextMenuRequested(const QPoint &pos);

    void on_tableWidget_search_itemDoubleClicked(QTableWidgetItem *item);

    void on_btn_play_clicked();

    void onPlayNowTriggered();

    void onPlayNextTriggered();

    void onAddToPlayListTriggered();

    void onFavoriteTriggered();

private:
    Ui::MainWidget *ui;
    //QNetworkAccessManager *networkManager;

    QMenu* contextMenu;
    QAction* playNowAction;
    QAction* playNextAction;
    QAction* addToPlayListAction;
    QAction* favoriteAction;
    QMenu* playListMenu;
    SongList menuMusics;
    Music menuCurrentSong;

    QSettings settings;
    QDir musicFileDir;   // 用于程序识别
    QDir downloadedMusicFileDir; // 已下载的歌曲资源
    const QString API_DOMAIN = "http://iwxyi.com:3000/";

    QMediaPlayer *player;            // 播放器
    QMediaPlaylist *playlist;        // 播放列表
    SongList songSearchResult;
    Music downloadingSong;
    Music playAfterDownloaded;
    Music playingSong;

    SongList orderSongs;             // 播放列表
    SongList favoriteSongs;          // 我的喜欢
    SongList localSongs;             // 本地歌曲
    SongList toDownLoadSongs;        // 即将下载
    PlayListList songplaylist; // 歌单
signals:
    void signalOrderSongPlayed(Music music);
    void signalSongDownloadFinished(Music music);
    void signalLyricDownloadFinished(Music music);
    void signalCoverDownloadFinished(Music music);

    void signalSongPlayStarted(Music music);
    void signalSongPlayFinished(Music music);

};
#endif // MAINWIDGET_H
