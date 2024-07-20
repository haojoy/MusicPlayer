#include <QScopedPointer>
#include <QScrollBar>
#include <QRandomGenerator>
#include <QFileDialog>
#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "taglibHelper.h"

MainWidget::MainWidget(QWidget *parent)
    : SwitchAnimation(parent)
    , ui(new Ui::MainWidget),
    changeBackground(new ChangeBackground(this)),
    settings(QApplication::applicationDirPath() + "/musics.ini", QSettings::Format::IniFormat),
    musicFileDir(QApplication::applicationDirPath() + "/musics"),
    downloadedMusicFileDir(QApplication::applicationDirPath() + "/downloaded"),
    player(new QMediaPlayer(this)),
    playlist(new QMediaPlaylist(this)),
    desktopLyric(new DesktopLyricWidget()),
    defaultImagePath(":/images/background.jpg"),
    paintingImagePath(defaultImagePath),
    homePageImagePath(defaultImagePath),
    lyricPageImagePath(defaultImagePath),
    paintingColor(QColor(97, 76, 64, 127))
{
    ui->setupUi(this);
    changeBackground->hide();
    playlist->setPlaybackMode(QMediaPlaylist::Loop); //循环模式
    ui->btn_mode->setToolTip("循环播放");
    ui->btn_mode->setIcon(QIcon(":/images/button/mode_loop.png"));

    player->setPlaylist(playlist);


    // tablewidget 歌曲右键菜单
    contextMenu = new QMenu(this);
    playNowAction = new QAction("立即播放", this);
    playNextAction = new QAction("下一首播放", this);
    addToPlayListAction = new QAction("添加到播放列表", this);
    favoriteAction = new QAction("我的喜欢", this);
    playListMenu = new QMenu("添加到歌单", this);
    contextMenu->addAction(playNowAction);
    contextMenu->addAction(playNextAction);
    contextMenu->addAction(addToPlayListAction);
    contextMenu->addAction(favoriteAction);
    contextMenu->addMenu(playListMenu);

    musicFileDir.mkpath(musicFileDir.absolutePath());

    bool showDeskTopLyric = settings.value("music/desktopLyric", false).toBool();

    if (showDeskTopLyric)
    {
        desktopLyric->show();
        ui->btn_desklrc->setIcon(QIcon(QPixmap(":/images/button/btn_lrc_show.png")));
    }
    else
    {
        desktopLyric->hide();
        ui->btn_desklrc->setIcon(QIcon(QPixmap(":/images/button/btn_lrc.png")));
    }

    volumeSliderWnd = new VolumeSlider(ui->btn_volume,this),
    volumeSliderWnd->hide();

    // 音量
    int volume = settings.value("music/volume", 20).toInt();
    bool mute = settings.value("music/mute", false).toBool();
    if (mute)
    {
        volume = 0;
        volumeSliderWnd->setSliderValue(0);
        ui->btn_volume->setIcon(QIcon(":/images/button/volume_close.png"));

    }
    player->setVolume(volume);


    ui->btn_volume->installEventFilter(this);
    this->installEventFilter(this);

    setbtnlikeIcon();

    ui->btn_songList->setIcon(QIcon(QPixmap(":/images/button/btn_unfold.png")));
    ui->listWidget_musiclist->show();

    restoreSongList("music/order", recentlySongs);
    restoreSongList("music/local", localSongs);
    restoreSongList("music/favorite", favoriteSongs);
    restorePlayList("playlist/list", MusicsList);
    setMusicsListTable(MusicsList, ui->listWidget_musiclist);

    // 清空音乐播放列表
    playlist->clear();
    // 初始化音乐播放列表
    for (int i = 0; i < recentlySongs.size(); i++) {
        addMusicToPlaylist(recentlySongs.at(i));
    }

    // UI相关初始化
    init_HandleUI();
    // 连接信号和槽
    init_HandleSignalsAndSlots();
}

// 界面初始化处理
void MainWidget::init_HandleUI(){
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    // 加载样式表
    loadStyleSheet(":/qss/qss/style.qss");

    // qss style QHeaderView::section
    ui->tableWidget_search->horizontalHeader()->setStyleSheet("background-color:transparent");
    ui->tableWidget_search->verticalHeader()->setStyleSheet("background-color:transparent");
    ui->tableWidget_favorite->horizontalHeader()->setStyleSheet("background-color:transparent");
    ui->tableWidget_favorite->verticalHeader()->setStyleSheet("background-color:transparent");
    ui->tableWidget_local->horizontalHeader()->setStyleSheet("background-color:transparent");
    ui->tableWidget_local->verticalHeader()->setStyleSheet("background-color:transparent");
    ui->tableWidget_recently->horizontalHeader()->setStyleSheet("background-color:transparent");
    ui->tableWidget_recently->verticalHeader()->setStyleSheet("background-color:transparent");
    ui->tableWidget_songlist->horizontalHeader()->setStyleSheet("background-color:transparent");
    ui->tableWidget_songlist->verticalHeader()->setStyleSheet("background-color:transparent");

    QString lineeditstyle = "QLineEdit {border: 1px solid lightgray; border-radius: 10px;padding-left: 10px;background-color: rgba(142,128,119,127)}";
    ui->edit_search->setStyleSheet(lineeditstyle);

    ui->scrollArea->setStyleSheet("QScrollArea {background:transparent; border: none}");
    ui->scrollarea_lrc->setStyleSheet("background:transparent");
    ui->scrollArea->setWidgetResizable(true);
}

void MainWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // 确定绘制区域，排除边框
    int borderThickness = 20;
    QRect rectajust = rect();
    if (!isMaximized()) {
        rectajust = rectajust.adjusted(borderThickness, borderThickness, -borderThickness, -borderThickness);
    }
    // qDebug() << "rect:" <<rect().height() << rect().width();
    // qDebug() << "---rect:" <<rectajust.height() << rectajust.width();
    // 绘制背景图像

    QPixmap pixmap(paintingImagePath);
    painter.drawPixmap(rectajust, pixmap);

    // 设置背景颜色透明度
    painter.fillRect(rectajust, paintingColor);

    QWidget::paintEvent(event);
}

void MainWidget::loadStyleSheet(const QString &styleSheetFile) {
    QFile file(styleSheetFile);
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "Failed to open the style sheet file.";
        QMessageBox::warning(this, "Error", "Failed to load the style sheet file.");
    }
}

// 显示歌词界面
void MainWidget::showLyricsPage()
{
    this->slideAnimation(this, ui->widget_lrc, AnimDirection::Up);
    QTimer::singleShot(animTime, this, [=] {
        ui->tabWidget->setCurrentWidget(ui->tab_lrc);
    });

    //ui->btn_albumpic->setStyleSheet(QString("QPushButton { border-image: url(%1); }").arg(btnImagePath));
    ui->btn_albumpic->setStyleSheet("QPushButton::hover{border-image:url(:/images/button/btn_down.png);}");

    if(player->state() == QMediaPlayer::PlayingState){
        setlyricPageBackImagePath(coverPath(playingSong));
    }
}

// 显示主界面
void MainWidget::showHomePage()
{
    this->slideAnimation(this, ui->widget_lrc, AnimDirection::Down);
    ui->tabWidget->setCurrentWidget(ui->tab_homePage);

    //ui->btn_albumpic->setStyleSheet(QString("QPushButton { border-image: url(%1); }").arg(btnImagePath));
    ui->btn_albumpic->setStyleSheet("QPushButton::hover{border-image:url(:/images/button/btn_up.png);}");
    sethomePageBackImagePath(homePageImagePath);
    ui->edit_search->clearFocus();
}

// 连接信号和槽
void MainWidget::init_HandleSignalsAndSlots(){

    // 回车搜索 （也可转到槽）
    connect(ui->edit_search, &QLineEdit::returnPressed, this, [=] {
        ui->btn_search->click();
    });

    connect(playNowAction, &QAction::triggered, this, &MainWidget::onPlayNowTriggered);
    connect(playNextAction, &QAction::triggered, this, &MainWidget::onPlayNextTriggered);
    connect(addToPlayListAction, &QAction::triggered, this, &MainWidget::onAddToPlayListTriggered);
    connect(favoriteAction, &QAction::triggered, this, &MainWidget::onFavoriteTriggered);

    connect(player, &QMediaPlayer::positionChanged, this, [=](qint64 position){
        ui->slider_progress->setValue(position / 1000);
        slotPlayerPositionChanged();
    });

    connect(player, &QMediaPlayer::stateChanged, this, [=](QMediaPlayer::State state){
        if (state == QMediaPlayer::PlayingState)
        {
            ui->btn_play->setIcon(QIcon(":/images/button/btn_playing.png"));
        }
        else
        {
            ui->btn_play->setIcon(QIcon(":/images/button/btn_play.png"));
        }
    });

    connect(player,&QMediaPlayer::durationChanged,this, [=](qint64 duration){
        //实际可播放时长 歌曲总时长music.duration
        if (player->state() == QMediaPlayer::PlayingState) {
            ui->slider_progress->setMaximum(static_cast<int>(duration/1000));
        }

    });


    connect(ui->slider_progress, &QSlider::sliderMoved, this, [=](qint64 position){
        player->setPosition(position * 1000);
    });

    connectDesktopLyricSignals();

    connect(volumeSliderWnd->getSlider(), &QSlider::sliderMoved, this, &MainWidget::updateVolumeSliderValue);

    // 当前媒体状体改变
    connect(player, &QMediaPlayer::mediaStatusChanged, [=]()
    {
    });
    // 播放歌曲改变
    connect(player, &QMediaPlayer::currentMediaChanged, this, [=] {
        int index = playlist->currentIndex();
        if(0 > index) {
            return;
        }
    });
    connect(playlist, &QMediaPlaylist::currentMediaChanged, this, &MainWidget::onCurrentMediaChanged);

    connect(ui->tabWidget_switchcontent, &QTabWidget::currentChanged, this, &MainWidget::highlightCurrentTabButton);

    // 背景选择
    connect(changeBackground, &ChangeBackground::backgroundChanged, this, &MainWidget::onBackgroundChanged);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - dragStartPosition);
        event->accept();
        setCursor(Qt::ClosedHandCursor);
    }
}

void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = false;
        event->accept();
        setCursor(Qt::ArrowCursor);
    }
}

bool MainWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->btn_volume) {
        if (event->type() == QEvent::Enter) {
            showSliderWindow();
            return true;
        } else if (event->type() == QEvent::Leave) {
            hideSliderWindow();
            return true;
        }
    }

    if (watched == this && event->type() == QEvent::Resize)
    {
        if (changeBackground && !changeBackground->isHidden())
        {
            updateChangeBackgroundPosition();
        }
    }

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (changeBackground->isVisible() && !changeBackground->geometry().contains(mouseEvent->pos())) {
            if (rect().contains(mouseEvent->pos())) {
                changeBackground->hide();
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void MainWidget::on_btn_down_clicked()
{
    showHomePage();
}

void MainWidget::on_btn_albumpic_clicked()
{
    if (ui->tabWidget->currentWidget() == ui->tab_homePage) {
        showLyricsPage();
        ui->btn_albumpic->setToolTip(QString("收起音乐详情"));
    } else if (ui->tabWidget->currentWidget() == ui->tab_lrc) {
        showHomePage();
        ui->btn_albumpic->setToolTip(QString("展开音乐详情"));
    }
}

void MainWidget::setMusicsListTable(PlayListList playlist, QListWidget *list)
{
    list->clear();

    QFontMetrics fm(list->font()); // 使用列表的字体来计算文本宽度

    for (int index = 0; index < playlist.size(); index++)
    {
        const QString &playlistName = playlist.at(index).name;

        QListWidgetItem *item = new QListWidgetItem;
        item->setIcon(QIcon(":/images/playlist.png"));
        item->setText(playlistName);
        list->addItem(item);

        // 检查文字宽度是否超过列表项宽度
        if (fm.horizontalAdvance(playlistName) > list->width())
        {
            item->setToolTip(playlistName);
        }
    }
}

void MainWidget::setPlayListTable(QList<Music> songs, QTableWidget *table)
{
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table->clear();

    table->setColumnCount(4);
    QStringList headers{"标题", "作者", "专辑", "时长"};
    table->setHorizontalHeaderLabels(headers);
    QList<int> tableColumnWidths = { 300, 280, 260, 60 };
    if(this->isMaximized()){
        tableColumnWidths = { 480, 320, 420, 80 };
    }
    // 设置每列的原始宽度
    for (int col = 0; col < 4; ++col) {
        table->setColumnWidth(col, tableColumnWidths[col]);
    }

    // 设置列长度
    QFontMetrics fm(font());
    auto createItem = [&](QString text, int maxWidth) {
        QTableWidgetItem *item = new QTableWidgetItem();
        if (fm.horizontalAdvance(text) > maxWidth) {
            item->setToolTip(text); // 设置 tooltip 显示完整文本
            text = fm.elidedText(text, Qt::ElideRight, maxWidth); // 截断文本并显示省略号
        }
        item->setText(text);
        return item;
    };

    table->setRowCount(songs.size());
    for (int row = 0; row < songs.size(); row++)
    {
        Music song = songs.at(row);
        table->setItem(row, titleCol, createItem(song.name, tableColumnWidths[titleCol]));
        table->setItem(row, artistCol, createItem(song.artistNames, tableColumnWidths[artistCol]));
        table->setItem(row, albumCol, createItem(song.album.name, tableColumnWidths[albumCol]));
        table->setItem(row, durationCol, createItem(msecondToString(song.duration), tableColumnWidths[durationCol]));
    }
}

void MainWidget::setSearchResultTable(SongList songs)
{
    QTableWidget *table = ui->tableWidget_search;
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->setColumnCount(4);
    QStringList headers{"标题", "作者", "专辑", "时长"};
    table->setHorizontalHeaderLabels(headers);
    table->setRowCount(songs.size());
    auto createItem = [=](const QString &text) {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(text);
        return item;
    };
    for (int row = 0; row < songs.size(); row++)
    {
        Music song = songs.at(row);
        table->setItem(row, titleCol, createItem(song.name));
        table->setItem(row, artistCol, createItem(song.artistNames));
        table->setItem(row, albumCol, createItem(song.album.name));
        table->setItem(row, durationCol, createItem(msecondToString(song.duration)));
    }

    QTimer::singleShot(0, this, [=]{
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    });

}

void MainWidget::on_btn_favorite_clicked()
{
    setPlayListTable(favoriteSongs, ui->tableWidget_favorite);
    ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_favorite);
    highlightCurrentTabButton(1);
}

void MainWidget::on_btn_recently_clicked()
{
    setPlayListTable(recentlySongs, ui->tableWidget_recently);
    ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_recentlyPlayed);
    highlightCurrentTabButton(2);
}

void MainWidget::on_btn_songList_clicked()
{
    if(ui->listWidget_musiclist->isHidden())
    {
        setMusicsListTable(MusicsList, ui->listWidget_musiclist);
        ui->btn_songList->setIcon(QIcon(QPixmap(":/images/button/btn_unfold.png")));
        ui->listWidget_musiclist->show();
    }else{
        ui->btn_songList->setIcon(QIcon(QPixmap(":/images/button/btn_fold.png")));
        ui->listWidget_musiclist->hide();
    }
}

void MainWidget::on_btn_localsong_clicked()
{
    setPlayListTable(localSongs, ui->tableWidget_local);
    ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_local);
    highlightCurrentTabButton(4);
}

void MainWidget::on_btn_minsize_clicked()
{
    this->showMinimized();
}

void MainWidget::on_btn_maxsize_clicked()
{
    if (this->isMaximized()) {
        this->showNormal();
        ui->btn_maxsize->setIcon(QIcon(QPixmap(":/images/button/btn_maximize.png")));
    } else {
        this->showMaximized();
        ui->btn_maxsize->setIcon(QIcon(QPixmap(":/images/button/btn_restore.png")));
    }
}

void MainWidget::on_btn_minsize2_clicked()
{
    this->showMinimized();
}

void MainWidget::on_btn_maxsize2_clicked()
{
    if (this->isMaximized()) {
        this->showNormal();
        ui->btn_maxsize2->setIcon(QIcon(QPixmap(":/images/button/btn_maximize.png")));
    } else {
        this->showMaximized();
        ui->btn_maxsize2->setIcon(QIcon(QPixmap(":/images/button/btn_restore.png")));
    }
}

void MainWidget::on_btn_close_clicked()
{
    this->close();
}

void MainWidget::on_btn_close2_clicked()
{
    this->close();
}

void MainWidget::on_btn_search_clicked()
{
    ui->tableWidget_search->clear();
    ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_searchResult);
    QString songname = ui->edit_search->text();
    if (songname.trimmed().isEmpty()) {
        QMessageBox::information(this, "提示", "请输入想要搜索的歌曲");
        setPlayListTable(songSearchResult,ui->tableWidget_search);
    } else {
        musicSearch(songname);
    }
}

void MainWidget::handleNetworkResponseData(QNetworkReply *reply) {
    if (!reply) {
        qDebug() << __FILE__ << __LINE__ <<"Invalid reply pointer";
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(response, &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug() << __FILE__ << __LINE__ << error.errorString();
            return;
        }

        QJsonObject json = document.object();
        if (json.value("code").toInt() != 200) {
            qDebug() << __FILE__ << __LINE__ << json.value("message").toString();
            return;
        }

        QJsonArray musics = json.value("result").toObject().value("songs").toArray();
        // 清除上次搜索结果
        songSearchResult.clear();
        foreach (QJsonValue val, musics) {
            songSearchResult << Music::fromJson(val.toObject());
        }

        setSearchResultTable(songSearchResult);
    } else {
        qDebug() << "Network error:" << reply->errorString();
        QMessageBox::warning(this, "Network Error", reply->errorString());
    }
    reply->deleteLater();
}

QNetworkRequest MainWidget::getNetworkRequest(const QString &requrl)
{
    QNetworkRequest request(requrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36");
    return request;
}

void MainWidget::musicSearch(const QString &keystring) {
    QString requrl = API_DOMAIN +"search?keywords=" + keystring.toUtf8().toPercentEncoding();
    //QString requrl = "http://mobilecdn.kugou.com/api/v3/search/song?format=json&keyword=";
    QNetworkAccessManager *manager = new QNetworkAccessManager;
    QNetworkRequest request = getNetworkRequest(requrl);

    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply* reply) {
        if (!reply) {
            qDebug() << __FILE__ << __LINE__ << "Invalid reply pointer";
            return;
        }

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonParseError error;
            QJsonDocument document = QJsonDocument::fromJson(response, &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug() << __FILE__ << __LINE__ << error.errorString();
                return;
            }

            QJsonObject json = document.object();
            if (json.value("code").toInt() != 200) {
                qDebug() << __FILE__ << __LINE__ << json.value("message").toString();
                return;
            }

            QJsonArray musics = json.value("result").toObject().value("songs").toArray();
            songSearchResult.clear();
            foreach (QJsonValue val, musics) {
                songSearchResult << Music::fromJson(val.toObject());
            }

            setSearchResultTable(songSearchResult);
        } else {
            qDebug() << "Network error:" << reply->errorString();
            QMessageBox::warning(this, "Network Error", reply->errorString());
        }
        reply->deleteLater();
        manager->deleteLater();
    });

    manager->get(request);
}

QString MainWidget::msecondToString(int mseconds) {
    int seconds = mseconds / 1000;
    int minutes = seconds / 60;
    seconds %= 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

void MainWidget::saveSongList(QString key, const SongList &songs)
{
    QJsonArray array;
    foreach(Music music, songs)
        array.append(music.toJson());
    settings.setValue(key,array);
}

void MainWidget::savePlayList(QString key, const PlayListList &playlistlist)
{
    QJsonArray array;
    foreach(PlayList pl, playlistlist)
    {
        array.append(pl.toJson());
    }
    settings.setValue(key, array);
}

void MainWidget::restoreSongList(QString key, SongList &songs)
{
    QJsonArray array = settings.value(key).toJsonArray();
    foreach(QJsonValue val, array){
        Music music = Music::fromJson(val.toObject());
        // QString songFilePath = songPath(music);
        // if (!QFile::exists(songFilePath)) {
        //     settings.remove(key);
        // }else{
        //     songs.append(music);
        // }
        songs.append(music);
    }
}

void MainWidget::restorePlayList(QString key, PlayListList &playlistlist)
{
    QJsonArray array = settings.value(key).toJsonArray();
    foreach(QJsonValue val, array)
        playlistlist.append(PlayList::fromJson(val.toObject()));
}

void MainWidget::connectDesktopLyricSignals()
{
    connect(desktopLyric, &DesktopLyricWidget::signalhide, this, [=]{
        ui->btn_desklrc->setIcon(QIcon(QPixmap(":/images/button/btn_lrc.png")));
        settings.setValue("music/desktopLyric", false);
    });
    connect(desktopLyric, &DesktopLyricWidget::signalSWitchTrans, this, [=]{
        desktopLyric->close();
        desktopLyric->deleteLater();
        desktopLyric = new DesktopLyricWidget(nullptr);
        connectDesktopLyricSignals();
        desktopLyric->show();

        if (playingSong.isValid())
        {
            Music song = playingSong;
            if (QFileInfo(lyricPath(song)).exists())
            {
                QFile file(lyricPath(song));
                file.open(QIODevice::ReadOnly | QIODevice::Text);
                QTextStream stream(&file);
                QString lyric;
                QString line;
                while (!stream.atEnd())
                {
                    line = stream.readLine();
                    lyric.append(line+"\n");
                }
                file.close();

                desktopLyric->setLyric(lyric);
                desktopLyric->setPosition(player->position());
            }
        }
    });

}


QString MainWidget::songPath(const Music &music) const
{
    if(music.id != 0){
        return musicFileDir.absoluteFilePath(snum(music.id) + ".mp3");
    }else{
        return musicFileDir.absoluteFilePath(music.name + ".mp3");
    }

}
QString MainWidget::lyricPath(const Music &music) const
{
    if(music.id != 0){
        return musicFileDir.absoluteFilePath(snum(music.id) + ".lrc");
    }else{
        return musicFileDir.absoluteFilePath(music.name + ".lrc");
    }

}
QString MainWidget::coverPath(const Music &music) const
{
    if(music.id != 0){
        return musicFileDir.absoluteFilePath(snum(music.id) + ".jpg");
    }else{
        return musicFileDir.absoluteFilePath(music.name + ".jpg");
    }

}

bool MainWidget::isSongDownloaded(Music music)
{
    return QFileInfo(songPath(music)).exists();
}

void MainWidget::playLocalSong(Music music)
{
    qDebug() << "开始播放" << music.simpleString();
    //mySystemTray->setToolTip(music.simpleString());
    if (music.id != 0 && !isSongDownloaded(music) )
    {
        qDebug() << "error:未下载歌曲：" << music.simpleString() << "开始下载";
        playAfterDownloaded = music;
        downloadSong(music);
        return ;
    }

    setLableSongInfo(music);
    //ui->slider_progress->setMaximum(music.duration /1000);


    if (QFileInfo(coverPath(music)).exists())
    {
        QIcon buttonIcon(coverPath(music));
        ui->btn_albumpic->setIcon(buttonIcon);
    }
    else
    {
        downloadSongCover(music);
    }


    if (QFileInfo(lyricPath(music)).exists())
    {
        QFile file(lyricPath(music));
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream stream(&file);
        QString lyric;
        QString line;
        while (!stream.atEnd())
        {
            line = stream.readLine();
            lyric.append(line + "\n");
        }

        file.close();

        setCurrentLyric(lyric);
    }
    else
    {
        setCurrentLyric("");
        downloadSongLyric(music);
    }

    // 开始播放
    playingSong = music;
    if (recentlySongs.contains(music))
        recentlySongs.removeAll(music);
    recentlySongs.insert(0,music);

    QString songFilePath = songPath(music);
    // 检查播放列表中是否已经存在相同的歌曲路径
    bool alreadyExists = false;
    for (int i = 0; i < playlist->mediaCount(); ++i) {
        QMediaContent mediaContent = playlist->media(i);
        if (mediaContent.request().url().toLocalFile() == songFilePath) {
            playlist->setCurrentIndex(i);
            alreadyExists = true;
            break;
        }
    }
    if (!alreadyExists){
        addMusicToPlaylist(music);
        playlist->setCurrentIndex(playlist->mediaCount() - 1);
    }


    // player->setMedia(QUrl::fromLocalFile(songPath(music)));
    // player->setPosition(0);
    player->play();
    setbtnlikeIcon();
    emit signalSongPlayStarted(music);
    setWindowTitle(music.name);

    // 添加到本地歌曲
    if (localSongs.contains(music))
    {
        qDebug() << "本地歌曲中已存在：" << music.simpleString();
        return ;
    }
    else
    {
        localSongs.append(music);
        saveSongList("music/local", localSongs);
    }

    // 保存当前歌曲
    settings.setValue("music/currentSong", music.toJson());

}

/**
 * 放入下载队列，准备下载（并不立即下载）
 */
void MainWidget::addDownloadSong(Music music)
{
    if (isSongDownloaded(music) || toDownLoadSongs.contains(music) || downloadingSong == music)
        return ;
    toDownLoadSongs.append(music);
}

/**
 * 放入下载队列、或一首歌下载结束后，下载下一个
 */
void MainWidget::downloadNext()
{
    // 正在下载的歌已经下载了/即将下载的队列是空的 bool 0 false 1 true
    if (downloadingSong.isValid() || !toDownLoadSongs.size())
        return ;
    Music music = toDownLoadSongs.takeFirst();
    if (!music.isValid())
        return downloadNext();
    downloadSong(music);
}

void MainWidget::downloadFile(const QString &url, const Music &music) {
    QNetworkAccessManager *fileManager = new QNetworkAccessManager(this);
    QNetworkReply *reply = fileManager->get(QNetworkRequest(QUrl(url)));

    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Download failed:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QByteArray fileData = reply->readAll();
        QFile file(songPath(music));
        if (file.open(QIODevice::WriteOnly)) {
            file.write(fileData);
            file.close();

            emit signalSongDownloadFinished(music);

            if (playAfterDownloaded == music)
                playLocalSong(music);

            downloadingSong = Music();
            downloadNext();
        } else {
            qDebug() << "Failed to open file for writing:" << file.errorString();
        }

        reply->deleteLater();
        fileManager->deleteLater();
    });
}

void MainWidget::setLableSongInfo(const Music &music) {
    ui->lable_songinfo->clear();
    //ui->lable_songinfo->setWordWrap(true);
    ui->lable_songinfo->setToolTip(music.name + "\n" + music.artistNames);
    ui->label_lrchead->setText(music.name + "\n" + music.artistNames);
    auto max11 = [](const QString &s) {
        return s.length() >11 ? s.left(10) + "..." : s;
    };
    ui->lable_songinfo->setText(max11(music.name) + "\n" + max11(music.artistNames));
}

void MainWidget::handleDownloadFailure(const Music &music) {
    if (playAfterDownloaded == music) {
        if (recentlySongs.contains(music)) {
            recentlySongs.removeAll(music);
            settings.setValue("music/currentSong", "");
            setLableSongInfo(music);
            player->stop();
        }
        saveSongList("music/order", recentlySongs);
    }
    downloadingSong = Music();
    downloadNext();
}



void MainWidget::downloadSong(Music music)
{
    if (isSongDownloaded(music))
        return;
    downloadingSong = music;
    QString url = API_DOMAIN +"/song/url?id=" + snum(music.id);
    //QString url = API_DOMAIN +"/song/url/v1?id=" + snum(music.id) + "&level=jymaster";
    qDebug()<< "获取歌曲信息：" << music.simpleString();
    QNetworkAccessManager *manager = new QNetworkAccessManager;
    QNetworkRequest request = getNetworkRequest(url);

    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply* reply) {
        QByteArray baData = reply->readAll();
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(baData, &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug() << error.errorString();
            return;
        }
        QJsonObject json = document.object();
        if (json.value("code").toInt() != 200) {
            qDebug() << "返回结果不为200：" << json.value("message").toString();
            return;
        }
        QJsonArray array = json.value("data").toArray();
        if (array.isEmpty()) {
            qDebug() << "未找到歌曲：" << music.simpleString();
            downloadingSong = Music();
            downloadNext();
            return;
        }

        json = array.first().toObject();
        QString songUrl = json.value("url").toString();
        QString url = JVAL_STR(url);
        int br = JVAL_INT(br); // 比率320000
        int size = JVAL_INT(size);
        QString type = JVAL_STR(type); // mp3
        QString encodeType = JVAL_STR(encodeType); // mp3
        qDebug() << "  信息：" << br << size << type << encodeType << url;
        if (size == 0) {
            qDebug() << "下载失败，可能没有版权" << music.simpleString();
            handleDownloadFailure(music);
            return;
        }

        downloadFile(songUrl, music);
        reply->deleteLater();
        manager->deleteLater();
    });

    manager->get(request);

    downloadSongLyric(music);
    downloadSongCover(music);
}

void MainWidget::downloadSongLyric(Music music)
{
    if (music.id == 0 || QFileInfo(lyricPath(music)).exists())
        return ;

    downloadingSong = music;
    QString url = API_DOMAIN + "/lyric?id=" + snum(music.id);
    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest request = getNetworkRequest(url);

    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply* reply){
        QByteArray baData = reply->readAll();
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(baData, &error);
        if (error.error != QJsonParseError::NoError)
        {
            qDebug() << error.errorString();
            return ;
        }
        QJsonObject json = document.object();
        if (json.value("code").toInt() != 200)
        {
            qDebug() << ("返回结果不为200：") << json.value("message").toString();
            return ;
        }

        QString lrc = json.value("lrc").toObject().value("lyric").toString();
        if (!lrc.isEmpty())
        {
            QFile file(lyricPath(music));
            file.open(QIODevice::WriteOnly);
            QTextStream stream(&file);
            stream << lrc;
            file.flush();
            file.close();

            qDebug() << "下载歌词完成：" << music.simpleString();
            if (playAfterDownloaded == music || playingSong == music)
            {
                setCurrentLyric(lrc);
            }

            emit signalLyricDownloadFinished(music);
        }
        else
        {
            qDebug() << "warning: 下载的歌词是空的" << music.simpleString();
        }
        reply->deleteLater();
        manager->deleteLater();
    });
    manager->get(request);
}

void MainWidget::downloadSongCover(Music music)
{
    if (music.id == 0 || QFileInfo(coverPath(music)).exists())
        return ;
    downloadingSong = music;
    QString url = API_DOMAIN + "/song/detail?ids=" + snum(music.id);
    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest request = getNetworkRequest(url);

    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply* reply){
        QByteArray baData = reply->readAll();
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(baData, &error);
        if (error.error != QJsonParseError::NoError)
        {
            qDebug() << error.errorString();
            return ;
        }
        QJsonObject json = document.object();
        if (json.value("code").toInt() != 200)
        {
            qDebug() << ("返回结果不为200：") << json.value("message").toString();
            return ;
        }

        QJsonArray array = json.value("songs").toArray();
        if (!array.size())
        {
            qDebug() << "未找到歌曲：" << music.simpleString();
            downloadingSong = Music();
            downloadNext();
            return ;
        }

        json = array.first().toObject();
        QString url = json.value("al").toObject().value("picUrl").toString();
        qDebug() << "封面地址：" << url;

        // 开始下载歌曲本身
        QNetworkAccessManager manager;
        QEventLoop loop;
        QNetworkReply *reply1 = manager.get(QNetworkRequest(QUrl(url)));
        //请求结束并下载完成后，退出子事件循环
        connect(reply1, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        //开启子事件循环
        loop.exec();
        QByteArray baData1 = reply1->readAll();
        QPixmap pixmap;
        pixmap.loadFromData(baData1);
        if (!pixmap.isNull())
        {
            QFile file(coverPath(music));
            file.open(QIODevice::WriteOnly);
            file.write(baData1);
            file.flush();
            file.close();

            emit signalCoverDownloadFinished(music);

            // 正是当前要播放的歌曲
            if (playAfterDownloaded == music || playingSong == music)
            {
                QIcon buttonIcon(coverPath(music));
                ui->btn_albumpic->setIcon(buttonIcon);
                //setCurrentCover(music);
            }
        }
        else
        {
            qDebug() << "warning: 下载的封面是空的" << music.simpleString();
        }
    });
    manager->get(request);
}

void MainWidget::startPlaySong(Music music)
{
    // 暂时用music id 0表示本地上传的歌曲
    if (isSongDownloaded(music) || music.id == 0)
    {
        playLocalSong(music);
    }
    else
    {
        playAfterDownloaded = music;
        downloadSong(music);
    }
    if (recentlySongs.contains(music))
        recentlySongs.removeAll(music);
    recentlySongs.insert(0,music);
    saveSongList("music/order", recentlySongs);
}

void MainWidget::appendOrderSongs(SongList musics)
{

}

void MainWidget::appendNextSongs(SongList musics)
{
    // foreach (Music music, musics)
    // {
    //     // 获取当前播放的位置
    //     int currentIndex = playlist->currentIndex();
    //     // 在当前位置的后面插入新的曲目
    //     playlist->insertMedia(currentIndex + 1, QUrl::fromLocalFile(songPath(music)));
    //     // 设置下一曲播放
    //     playlist->setCurrentIndex(currentIndex + 1);
    // }

}

void MainWidget::addMusicToMusicList(SongList musics, int row)
{
    foreach (Music music, musics)
    {
        if (MusicsList.at(row).containsMusic.contains(music))
            continue ;
        MusicsList[row].containsMusic.append(music);
    }

    savePlayList("playlist/list", MusicsList);
}


void MainWidget::setCurrentLyric(QString lyric)
{
    desktopLyric->setLyric(lyric);
    ui->scrollarea_lrc->loadLyric(lyric);
}

void MainWidget::sethomePageBackImagePath(QString imagePath)
{
    if(lyricPageImagePath != homePageImagePath){
        paintingImagePath = homePageImagePath;
        update();
    }
    if(imagePath != homePageImagePath){
        homePageImagePath = imagePath;
        paintingImagePath = imagePath;
        update();
    }
}

void MainWidget::setlyricPageBackImagePath(QString imagePath)
{
    if(!QFileInfo(imagePath).exists()){
        return;
    }
    if(imagePath != lyricPageImagePath || lyricPageImagePath != homePageImagePath){
        lyricPageImagePath = imagePath;
        paintingImagePath = imagePath;
        update();
    }
}

void MainWidget::addFavorite(SongList musics)
{
    foreach (Music music, musics)
    {
        if (favoriteSongs.contains(music))
        {
            qDebug() << "歌曲已存在：" << music.simpleString();
            continue;
        }
        favoriteSongs.append(music);
        qDebug() <<"添加收藏：" << music.simpleString();
    }
    saveSongList("music/favorite", favoriteSongs);
    setPlayListTable(favoriteSongs, ui->tableWidget_favorite);
}

void MainWidget::removeFavorite(SongList musics)
{
    foreach (Music music, musics)
    {
        if (favoriteSongs.removeOne(music))
        {
            qDebug() << "取消收藏：" << music.simpleString();
        }
    }
    saveSongList("music/favorite", favoriteSongs);
    setPlayListTable(favoriteSongs, ui->tableWidget_favorite);
}


void MainWidget::onPlayNowTriggered() {
    startPlaySong(menuCurrentSong);
}

void MainWidget::onPlayNextTriggered() {
    appendNextSongs(menuSelectedMusics);
}

void MainWidget::onAddToPlayListTriggered() {
    appendOrderSongs(menuSelectedMusics);
}

void MainWidget::onFavoriteTriggered() {
    if (!favoriteSongs.contains(menuCurrentSong))
        addFavorite(menuSelectedMusics);
    else
        removeFavorite(menuSelectedMusics);
    setbtnlikeIcon();
    menuSelectedMusics.clear();
}

void MainWidget::slotPlayerPositionChanged()
{
    qint64 position = player->position();
    if (desktopLyric && !desktopLyric->isHidden())
        desktopLyric->setPosition(position);
    if (ui->scrollarea_lrc->setPosition(position))
    {
        QPropertyAnimation* ani = new QPropertyAnimation(this, "lyricScroll");
        ani->setStartValue(ui->scrollArea->verticalScrollBar()->sliderPosition());
        ani->setEndValue(qMax(0, ui->scrollarea_lrc->getCurrentTop() - this->height() / 2));
        ani->setDuration(200);
        connect(ani, &QPropertyAnimation::valueChanged, this, [=]{
            ui->scrollArea->verticalScrollBar()->setSliderPosition(lyricScroll);
        });
        connect(ani, SIGNAL(finished()), ani, SLOT(deleteLater()));
        ani->start();
    }
    update();
}

void MainWidget::setLyricScroll(int x)
{
    this->lyricScroll = x;
}

int MainWidget::getLyricScroll() const
{
    return this->lyricScroll;
}

void MainWidget::on_btn_setskin_clicked()
{
    updateChangeBackgroundPosition();
    if(changeBackground->isHidden())
    {
        changeBackground->show();
    }
    else {
        changeBackground->hide();
    }

}


void MainWidget::on_btn_logo_clicked()
{
    QMessageBox::information(this, "关于", "name: MusicPlayer \ninfo: a qt project for learning \nauthor: haojoy");
}

//右键菜单请求
void MainWidget::handleContextMenuRequest(const QPoint& pos, QTableWidget* table, SongList typemusics) {
    if(typemusics.isEmpty())
        return;
    menuSelectedMusics.clear();
    auto items = table->selectedItems();
    foreach (auto item, items) {
        int row = table->row(item);
        int col = table->column(item);
        if (col == 0)
            menuSelectedMusics.append(typemusics.at(row));
    }

    int row = table->currentRow();
    if (row > -1)
        menuCurrentSong = typemusics.at(row);

    playListMenu->clear();
    playListMenu->setEnabled(!MusicsList.isEmpty());
    favoriteAction->setText(favoriteSongs.contains(menuCurrentSong) ? "从我的喜欢中移除" : "添加到我的喜欢");
    // 添加歌单菜单项并连接信号
    for (int index = 0; index < MusicsList.size(); ++index) {
        QAction* playlistAction = new QAction(MusicsList.at(index).name, this);
        playListMenu->addAction(playlistAction);

        connect(playlistAction, &QAction::triggered, this, [=]() {
            addMusicToMusicList(menuSelectedMusics, index);
        });
    }
    contextMenu->exec(table->viewport()->mapToGlobal(pos));
}

void MainWidget::handleTableDoubleClick(QTableWidgetItem *item, QTableWidget *table, SongList typemusics)
{
    int row = table->row(item);
    Music currentsong;
    if (row > -1 )
        currentsong = typemusics.at(row);

    if (recentlySongs.contains(currentsong))
        recentlySongs.removeAll(currentsong);
    recentlySongs.insert(0, currentsong);
    startPlaySong(currentsong);
}



void MainWidget::on_tableWidget_search_customContextMenuRequested(const QPoint &pos)
{
    handleContextMenuRequest(pos, ui->tableWidget_search, songSearchResult);
}


void MainWidget::on_tableWidget_search_itemDoubleClicked(QTableWidgetItem *item)
{
    handleTableDoubleClick(item, ui->tableWidget_search, songSearchResult);
}

void MainWidget::on_tableWidget_favorite_customContextMenuRequested(const QPoint &pos)
{
    handleContextMenuRequest(pos, ui->tableWidget_favorite, favoriteSongs);
}

void MainWidget::on_tableWidget_favorite_itemDoubleClicked(QTableWidgetItem *item)
{
    handleTableDoubleClick(item, ui->tableWidget_favorite, favoriteSongs);
}

void MainWidget::on_tableWidget_recently_customContextMenuRequested(const QPoint &pos)
{
    handleContextMenuRequest(pos, ui->tableWidget_recently, recentlySongs);
}


void MainWidget::on_tableWidget_recently_itemDoubleClicked(QTableWidgetItem *item)
{
    handleTableDoubleClick(item, ui->tableWidget_recently, recentlySongs);
    setPlayListTable(recentlySongs, ui->tableWidget_recently);
}


void MainWidget::on_tableWidget_local_customContextMenuRequested(const QPoint &pos)
{
    handleContextMenuRequest(pos, ui->tableWidget_local, localSongs);
}


void MainWidget::on_tableWidget_local_itemDoubleClicked(QTableWidgetItem *item)
{
     handleTableDoubleClick(item, ui->tableWidget_local, localSongs);
}

void MainWidget::on_tableWidget_songlist_customContextMenuRequested(const QPoint &pos)
{
    handleContextMenuRequest(pos, ui->tableWidget_songlist, currentMusicList);
}


void MainWidget::on_tableWidget_songlist_itemDoubleClicked(QTableWidgetItem *item)
{
    handleTableDoubleClick(item, ui->tableWidget_songlist, currentMusicList);
}

void MainWidget::on_btn_play_clicked()
{
    if (!recentlySongs.size())
        return ;
    if (player->state() == QMediaPlayer::PlayingState)
    {
        player->pause();
        ui->btn_play->setToolTip(QString("播放"));
    }
    else
    {
        if (!playingSong.isValid())
        {
            /*
            if(playlist->mediaCount() > 0){
                int randomIndex = QRandomGenerator::global()->bounded(playlist->mediaCount());
                playlist->setCurrentIndex(randomIndex);
                player->play();
            }
            */
            //playNext();
            return ;
        }
        player->play();
        ui->btn_play->setToolTip(QString("暂停"));
    }
}


void MainWidget::on_btn_desklrc_clicked()
{

    bool showDesktopLyric = !settings.value("music/desktopLyric", false).toBool();
    settings.setValue("music/desktopLyric", showDesktopLyric);
    if (showDesktopLyric)
    {
        desktopLyric->show();
        ui->btn_desklrc->setIcon(QIcon(QPixmap(":/images/button/btn_lrc_show.png")));
    }
    else
    {
        desktopLyric->hide();
        ui->btn_desklrc->setIcon(QIcon(QPixmap(":/images/button/btn_lrc.png")));
    }

}

void MainWidget::on_btn_volume_clicked()
{
    int volume = volumeSliderWnd->getSliderValue();
    if (volume == 0)
    {
        volume  = settings.value("music/volume", 20).toInt();
        if(volume == 0)
            volume = 20;
        ui->btn_volume->setIcon(QIcon(":/images/button/volume.png"));
        volumeSliderWnd->setSliderValue(volume);
        settings.setValue("music/mute", false);
        settings.setValue("music/volume", volume);
    }
    else
    {
        settings.setValue("music/volume", volume);
        volume = 0;
        ui->btn_volume->setIcon(QIcon(":/images/button/volume_close.png"));
        volumeSliderWnd->setSliderValue(0);
        settings.setValue("music/mute", true);
    }

    player->setVolume(volume);

}

void MainWidget::showSliderWindow()
{
    QPoint pos = ui->btn_volume->mapToGlobal(QPoint(0, -volumeSliderWnd->height()));
    volumeSliderWnd->move(pos.x() + ui->btn_volume->width() / 2 - volumeSliderWnd->width() / 2, pos.y());
    volumeSliderWnd->show();
}

void MainWidget::hideSliderWindow()
{
    QRect volumeSliderRect = volumeSliderWnd->geometry();
    QRect volumeButtonRect = ui->btn_volume->geometry();
    QRect combinedRect = volumeSliderRect.united(volumeButtonRect);
    if (!combinedRect.contains(QCursor::pos())) {
        // 使用 QTimer 实现延迟隐藏
        QTimer::singleShot(1000, this, [=]() {
            volumeSliderWnd->hide();
        });
    }
}

void MainWidget::updateVolumeSliderValue(int value)
{

    if(value == 0){
        ui->btn_volume->setIcon(QIcon(":/images/button/volume_close.png"));
        settings.setValue("music/mute", true);
    }else{
        ui->btn_volume->setIcon(QIcon(":/images/button/volume.png"));
        settings.setValue("music/mute", false);
    }

    settings.setValue("music/volume", value);
    player->setVolume(value);

}


void MainWidget::on_btn_like_clicked()
{
    if(!playingSong.isValid()){
        return;
    }
    SongList tempsong;
    tempsong.append(playingSong);
    if(favoriteSongs.contains(playingSong)){
        ui->btn_like->setIcon(QIcon(":/images/button/btn_like.png"));
        removeFavorite(tempsong);
    }else{
        ui->btn_like->setIcon(QIcon(":/images/button/btn_like2.png"));
        addFavorite(tempsong);
    }
}

void MainWidget::on_btn_pre_clicked()
{
    playlist->previous();
}

void MainWidget::on_btn_next_clicked()
{
    playlist->next();

}

void MainWidget::setbtnlikeIcon(){
    if(favoriteSongs.contains(playingSong)){
        ui->btn_like->setIcon(QIcon(":/images/button/btn_like2.png"));

    }else{
        ui->btn_like->setIcon(QIcon(":/images/button/btn_like.png"));
    }
}

void MainWidget::on_btn_mode_clicked()
{
    if(playlist->playbackMode()==QMediaPlaylist::Loop){
        ui->btn_mode->setToolTip("随机播放");
        ui->btn_mode->setIcon(QIcon(":/images/button/mode_random.png"));
        playlist->setPlaybackMode(QMediaPlaylist::Random);
    }
    else if(playlist->playbackMode()==QMediaPlaylist::Random){
        ui->btn_mode->setToolTip("单曲循环");
        ui->btn_mode->setIcon(QIcon(":/images/button/mode_single.png"));
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }
    else if(playlist->playbackMode()==QMediaPlaylist::CurrentItemInLoop){
        ui->btn_mode->setToolTip("顺序播放");
        ui->btn_mode->setIcon(QIcon(":/images/button/mode_seq.png"));
        playlist->setPlaybackMode(QMediaPlaylist::Sequential);
    }
    else if(playlist->playbackMode()==QMediaPlaylist::Sequential){
        ui->btn_mode->setToolTip("循环播放");
        ui->btn_mode->setIcon(QIcon(":/images/button/mode_loop.png"));
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
    }
}

void MainWidget::onCurrentMediaChanged(const QMediaContent &content)
{
    QString mediaUrl = content.request().url().toString();
    if (mediaToMusicMap.contains(mediaUrl)) {
        Music currentMusic = mediaToMusicMap.value(mediaUrl);
        playingSong = currentMusic;
        if(ui->tabWidget->currentWidget() == ui->tab_lrc){
            setlyricPageBackImagePath(coverPath(currentMusic));
        }
        setLableSongInfo(currentMusic);
        if (QFileInfo(lyricPath(currentMusic)).exists())
        {
            QFile file(lyricPath(currentMusic));
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream stream(&file);
            QString lyric;
            QString line;
            while (!stream.atEnd())
            {
                line = stream.readLine();
                lyric.append(line + "\n");
            }

            file.close();

            setCurrentLyric(lyric);
        }
        else
        {
            setCurrentLyric("");
            downloadSongLyric(currentMusic);
        }

        if (QFileInfo(coverPath(currentMusic)).exists())
        {
            QIcon buttonIcon(coverPath(currentMusic));
            ui->btn_albumpic->setIcon(buttonIcon);
        }
        else
        {
            downloadSongCover(currentMusic);
        }
    }
}

void MainWidget::highlightCurrentTabButton(int index)
{
    QString stylenormal = R"(
        QPushButton{
            background: transparent;
            color: #FFFFFF; /* 设置字体颜色 */
            border-radius: 8px;
            font-weight: bold;
        }
        QPushButton::hover{
            background-color: rgba(142,128,119,127);
        }
    )";
    // 重置所有按钮的样式
    ui->btn_favorite->setStyleSheet(stylenormal);
    ui->btn_recently->setStyleSheet(stylenormal);
    ui->btn_localsong->setStyleSheet(stylenormal);

    // 根据当前索引设置按钮的高亮样式
    QString stylehighlight = R"(
        QPushButton{
            background: transparent;
            color: #FFFFFF; /* 设置字体颜色 */
            border-radius: 8px;
            font-weight: bold;
            background-color: rgba(142,128,119,255);
        }
    )";
    // index为个tab页在UI中的index
    switch(index) {
    case 1:
        ui->btn_favorite->setStyleSheet(stylehighlight);
        break;
    case 2:
        ui->btn_recently->setStyleSheet(stylehighlight);
        break;
    case 4:
       ui->btn_localsong->setStyleSheet(stylehighlight);
        break;
    default:
        break;
    }
}

void MainWidget::addMusicToPlaylist(const Music &music) {
    QString songFilePath = songPath(music);
    if (QFile::exists(songFilePath)) {
        QUrl mediaUrl = QUrl::fromLocalFile(songFilePath);
        QMediaContent mediaContent(mediaUrl);
        playlist->addMedia(mediaContent);
        mediaToMusicMap.insert(mediaUrl.toString(), music);
    } else {
        qDebug() << "Song file does not exist: " << songFilePath;
    }
}

Music MainWidget::getCurrentMusic() const {
    QMediaContent currentMedia = playlist->currentMedia();
    QString mediaUrl = currentMedia.request().url().toString();

    if (mediaToMusicMap.contains(mediaUrl)) {
        return mediaToMusicMap.value(mediaUrl);
    }else{
        return Music();
    }
}



void MainWidget::on_btn_createsonglist_clicked()
{
    bool ok;
    QString text =QInputDialog::getText(this, "新建歌单", "输入新歌单名字", QLineEdit::Normal, "", &ok);

    if (ok && !text.isEmpty())
    {
        for (int i = 0; i < MusicsList.size(); i++)
        {
            if (MusicsList.at(i).name == text)
            {
                QMessageBox::warning(this, "警告:歌单已存在", "歌单已存在");
                return ;
            }
        }
        PlayList templist;
        templist.name = text;
        MusicsList.push_back(templist);
    }

    savePlayList("playlist/list", MusicsList);
    setMusicsListTable(MusicsList, ui->listWidget_musiclist);
}

void MainWidget::playMusiclist(QList<QListWidgetItem *> items)
{
    player->stop();
    playlist->clear();
    // 选择歌单开始播放
    SongList tempmusiclist;
    Music tempMusic;
    foreach (auto item, items)
    {
        // 选中的歌单
        int row1 = ui->listWidget_musiclist->row(item);
        tempmusiclist = MusicsList.at(row1).containsMusic;

        for (int i = 0; i < tempmusiclist.size(); ++i) {
            tempMusic = tempmusiclist[i];
            if(recentlySongs.contains(tempMusic)){
                recentlySongs.removeAll(tempMusic);
            }
            recentlySongs.insert(i, tempMusic);
            addMusicToPlaylist(tempMusic);
        }
    }
    playlist->setCurrentIndex(0);
    player->play();
    saveSongList("music/order", recentlySongs);
    setPlayListTable(recentlySongs, ui->tableWidget_recently);
}

void MainWidget::on_listWidget_musiclist_customContextMenuRequested(const QPoint &pos)
{
    auto items = ui->listWidget_musiclist->selectedItems();
    int row = ui->listWidget_musiclist->currentRow();
    PlayList currentplaylist;
    if (row < -1)
        currentplaylist= MusicsList.at(row);

    QMenu *menu = new QMenu(this);

    QAction *playNow = new QAction("播放歌单", this);
    QAction *rename = new QAction("重命名", this);
    QAction *deletePlayList = new QAction("删除歌单", this);

    menu->addAction(playNow);
    menu->addAction(rename);
    menu->addAction(deletePlayList);

    connect(playNow, &QAction::triggered, [=]{
        playMusiclist(items);
    });

    connect(rename, &QAction::triggered, [=]{
        bool ok;
        QString text = QInputDialog::getText(this, "重命名歌单", "请输入歌单新名字", QLineEdit::Normal, "", &ok);
        if (ok && !text.isEmpty())
        {
            for (int i = 0; i < MusicsList.size(); i++)
            {
                if (MusicsList.at(i).name == text)
                {
                    QMessageBox::warning(this, "警告:歌单名已存在", "请重新取名");
                    return ;
                }
            }
            MusicsList[row].name = text;
        }

        savePlayList("playlist/list", MusicsList);
        setMusicsListTable(MusicsList, ui->listWidget_musiclist);
    });

    connect(deletePlayList, &QAction::triggered, [=]{
        if (!items.isEmpty()) {
            QListWidgetItem *item = items.first();
            int rowToRemove = ui->listWidget_musiclist->row(item);
            ui->listWidget_musiclist->takeItem(rowToRemove);
            MusicsList.removeAt(rowToRemove);

            // 保存歌单列表和更新显示
            savePlayList("playlist/list", MusicsList);
            setMusicsListTable(MusicsList, ui->listWidget_musiclist);

            //player->stop();
            playlist->clear();

            // 删除后自动选中下一个歌单
            int nextRow = qMin(rowToRemove, MusicsList.size() - 1); // 下一个歌单的索引
            if (nextRow >= 0) {
                ui->listWidget_musiclist->setCurrentRow(nextRow); // 选中下一个歌单项
                QListWidgetItem *item1 = ui->listWidget_musiclist->currentItem();
                setPlayListTable(MusicsList.at(nextRow).containsMusic, ui->tableWidget_songlist);
                ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_defaultSongList);
                if(player->state() == QMediaPlayer::PlayingState){
                    QList<QListWidgetItem *> items;
                    items.append(item1);
                    playMusiclist(items);
                }
            }else{
                QList<Music> temp;
                setPlayListTable(temp, ui->tableWidget_songlist);
                ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_defaultSongList);
            }

        }
    });

    // 显示菜单
    menu->exec(cursor().pos());
    delete menu;
}


void MainWidget::on_listWidget_musiclist_itemClicked(QListWidgetItem *item)
{
    currentMusicList.clear();
    int index = ui->listWidget_musiclist->row(item);
    currentMusicList = MusicsList.at(index).containsMusic;
    setPlayListTable(currentMusicList, ui->tableWidget_songlist);
    ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_defaultSongList);
}


void MainWidget::on_listWidget_musiclist_itemDoubleClicked(QListWidgetItem *item)
{
    currentMusicList.clear();
    QList<QListWidgetItem *> items;
    items.append(item);
    int index = ui->listWidget_musiclist->row(item);
    currentMusicList = MusicsList.at(index).containsMusic;
    setPlayListTable(currentMusicList, ui->tableWidget_songlist);

    ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_defaultSongList);
    playMusiclist(items);
}

void MainWidget::updateChangeBackgroundPosition()
{
    // 获取按钮在父窗口中的位置
    QPoint btnPos = ui->btn_setskin->mapToParent(QPoint(0, 0));
    QSize btnSize = ui->btn_setskin->size();

    // 计算 changeBackground 窗口的显示位置
    int offsetX = btnPos.x() + btnSize.width() - changeBackground->width() / 2;
    int offsetY = btnPos.y() + btnSize.height() + 30;

    changeBackground->setGeometry(offsetX, offsetY, changeBackground->width(), changeBackground->height());
    changeBackground->raise();
}

void MainWidget::onBackgroundChanged(const QColor &color, const QString &imagePath)
{
    if (color.isValid()) {
        paintingColor = color;
    }
    if (!imagePath.isEmpty()) {
        paintingImagePath = imagePath;
    }
    update();  // 触发重绘
}

void MainWidget::on_btn_selectlocal_clicked()
{
    QString curPath=QCoreApplication::applicationDirPath();;
    QString dlgTitle="选择音频文件"; //对话框标题
    QString filter="音频文件(*.mp3 *.wav *.wma);;mp3文件(*.mp3);;wav文件(*.wav);;wma文件(*.wma);;所有文件(*.*)"; //文件过滤器
    QStringList fileList=QFileDialog::getOpenFileNames(this,dlgTitle,curPath,filter);

    if (fileList.count()<1)
        return;

    for (int i=0; i<fileList.count();i++)
    {
        QString songpath=fileList.at(i);
        Music localMusic = TaglibHelper::GetMusicTagInfo(songpath);

        QFileInfo fileInfo(songpath);
        localMusic.name = fileInfo.baseName();
        if (!localSongs.contains(localMusic))
        {
            localSongs.append(localMusic);
        }
        QString coverpath = coverPath(localMusic);
        QString lyricpath = lyricPath(localMusic);
        TaglibHelper::GetAlbumCoverAndLyrics(songpath,coverpath,lyricpath);
    }
    saveSongList("music/local", localSongs);
    setPlayListTable(localSongs, ui->tableWidget_local);
}

