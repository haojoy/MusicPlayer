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
#include "changebackground.h"
#include "music.h"
#include "switchanimation.h"
#include "desktopLyricWidget.h"
#include "volumeSlider.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE


class MainWidget : public SwitchAnimation
{
    Q_OBJECT
    Q_PROPERTY(int lyricScroll READ getLyricScroll WRITE setLyricScroll)

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
    void setMusicsListTable(PlayListList playlist, QListWidget *list);
    void setPlayListTable(SongList songs, QTableWidget *table);
    // 搜索音乐
    void musicSearch(const QString &keystring);
    void setSearchResultTable(SongList songs);


    void downloadFile(const QString &url, const Music &music);
    void setLableSongInfo(const Music &music);
    void handleDownloadFailure(const Music &music);
    // 下载音乐
    void playLocalSong(Music music);
    void addDownloadSong(Music music);
    void downloadNext();
    void downloadSong(Music music);
    void downloadSongLyric(Music music);
    void downloadSongCover(Music music);

    // 播放音乐
    void startPlaySong(Music music);
    //void playNext();
    void appendOrderSongs(SongList musics);
    //void removeOrderSongs(SongList musics);
    void appendNextSongs(SongList musics);
    void addMusicToMusicList(SongList musics, int row);
    //void ClearPlayList();
    void setCurrentLyric(QString lyric);

    // 设置背景
    void sethomePageBackImagePath(QString imagePath);
    void setlyricPageBackImagePath(QString imagePath);

    void addFavorite(SongList musics);
    void removeFavorite(SongList musics);


    QString msecondToString(int mseconds);


    // 保存配置
    void saveSongList(QString key, const SongList &songs);
    void savePlayList(QString key, const PlayListList &playlistlist);
    // 读取配置
    void restoreSongList(QString key, SongList &songs);
    void restorePlayList(QString key, PlayListList &playlistlist);

    void connectDesktopLyricSignals();
    void handleContextMenuRequest(const QPoint& pos, QTableWidget* table, SongList typemusics);
    void handleTableDoubleClick(QTableWidgetItem *item, QTableWidget* table, SongList typemusics);
    QNetworkRequest getNetworkRequest(const QString &requrl);

    void setLyricScroll(int x);
    int getLyricScroll() const;

    void setbtnlikeIcon();

    void addMusicToPlaylist(const Music &music);
    Music getCurrentMusic() const;
protected:
    //实现窗口可拖动
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    bool eventFilter(QObject *watched, QEvent *event) override;
public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

signals:
    void signalOrderSongPlayed(Music music);
    void signalSongDownloadFinished(Music music);
    void signalLyricDownloadFinished(Music music);
    void signalCoverDownloadFinished(Music music);

    void signalSongPlayStarted(Music music);
    void signalSongPlayFinished(Music music);

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

    void on_btn_minsize2_clicked();

    void on_btn_maxsize2_clicked();

    // 搜索歌曲
    void on_btn_search_clicked();

    void handleNetworkResponseData(QNetworkReply *reply);

    void on_btn_setskin_clicked();

    void on_btn_logo_clicked();

    void on_tableWidget_search_customContextMenuRequested(const QPoint &pos);

    void on_tableWidget_search_itemDoubleClicked(QTableWidgetItem *item);

    void on_tableWidget_favorite_customContextMenuRequested(const QPoint &pos);

    void on_tableWidget_favorite_itemDoubleClicked(QTableWidgetItem *item);

    void on_tableWidget_recently_customContextMenuRequested(const QPoint &pos);

    void on_tableWidget_recently_itemDoubleClicked(QTableWidgetItem *item);

    void on_tableWidget_local_customContextMenuRequested(const QPoint &pos);

    void on_tableWidget_local_itemDoubleClicked(QTableWidgetItem *item);

    void on_tableWidget_songlist_customContextMenuRequested(const QPoint &pos);

    void on_tableWidget_songlist_itemDoubleClicked(QTableWidgetItem *item);

    void on_btn_play_clicked();

    void onPlayNowTriggered();

    void onPlayNextTriggered();

    void onAddToPlayListTriggered();

    void onFavoriteTriggered();

    void slotPlayerPositionChanged();


    void on_btn_desklrc_clicked();

    void on_btn_volume_clicked();

    void showSliderWindow();

    void hideSliderWindow();

    void updateVolumeSliderValue(int value);

    void on_btn_like_clicked();

    void on_btn_pre_clicked();

    void on_btn_next_clicked();

    void on_btn_mode_clicked();

    void onCurrentMediaChanged(const QMediaContent &content);
    void on_btn_createsonglist_clicked();

    void playMusiclist(QList<QListWidgetItem *> items);
    void on_listWidget_musiclist_customContextMenuRequested(const QPoint &pos);

    void on_listWidget_musiclist_itemClicked(QListWidgetItem *item);

    void on_listWidget_musiclist_itemDoubleClicked(QListWidgetItem *item);

    void highlightCurrentTabButton(int index);

    void updateChangeBackgroundPosition();

    void onBackgroundChanged(const QColor &color, const QString &imagePath);

    void on_btn_selectlocal_clicked();

private:
    Ui::MainWidget *ui;

    /*换肤的界面*/
    ChangeBackground * changeBackground = nullptr;
    QMenu* contextMenu;
    QAction* playNowAction;
    QAction* playNextAction;
    QAction* addToPlayListAction;
    QAction* favoriteAction;
    QMenu* playListMenu;
    SongList menuSelectedMusics;
    Music menuCurrentSong;

    int lyricScroll;


    QSettings settings;
    QDir musicFileDir;   // 用于程序识别
    QDir downloadedMusicFileDir; // 已下载的歌曲资源
    const QString API_DOMAIN = "http://iwxyi.com:3000/";

    QMediaPlayer *player;            // 播放器
    QMediaPlaylist *playlist;        // 播放列表
    QMap<QString, Music> mediaToMusicMap; // 用于映射 QMediaContent 到 Music 对象

    SongList songSearchResult;
    Music downloadingSong;
    Music playAfterDownloaded;
    Music playingSong;

    SongList recentlySongs;             // 最近播放
    SongList favoriteSongs;          // 我的喜欢
    SongList localSongs;             // 本地歌曲
    SongList toDownLoadSongs;        // 即将下载
    SongList currentMusicList;     // 当前歌单的音乐列表
    PlayListList MusicsList; // 歌单列表

    DesktopLyricWidget* desktopLyric;
    VolumeSlider *volumeSliderWnd;


    QString defaultImagePath;
    QString paintingImagePath;
    QString homePageImagePath;
    QString lyricPageImagePath;

    QColor paintingColor;
};
#endif // MAINWIDGET_H
