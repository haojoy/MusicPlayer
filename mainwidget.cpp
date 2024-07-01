#include <QScopedPointer>
#include <QScrollBar>
#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : SwitchAnimation(parent)
    , ui(new Ui::MainWidget),
    settings(QApplication::applicationDirPath() + "/musics.ini", QSettings::Format::IniFormat),
    musicFileDir(QApplication::applicationDirPath() + "/musics"),
    downloadedMusicFileDir(QApplication::applicationDirPath() + "/downloaded"),
    player(new QMediaPlayer(this))
{
    ui->setupUi(this);

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

    QString lineeditstyle = "QLineEdit {border: 1px solid lightgray; border-radius: 10px;padding-left: 10px;background-color: rgba(142,128,119,127)}";
    ui->lineEdit_songlistname->setStyleSheet(lineeditstyle);
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
    QPixmap pixmap(":/images/background.jpg");
    painter.drawPixmap(rectajust, pixmap);

    // 设置背景颜色透明度
    painter.fillRect(rectajust, QColor(97, 76, 64, 127)); // 白色，透明度为 100

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
    ui->btn_albumpic->setStyleSheet("QPushButton::hover{border-image:url(:/images/button/btn_down.png);}");
}

// 显示主界面
void MainWidget::showHomePage()
{
    this->slideAnimation(this, ui->widget_lrc, AnimDirection::Down);
    ui->tabWidget->setCurrentWidget(ui->tab_homePage);
    ui->btn_albumpic->setStyleSheet("QPushButton::hover{border-image:url(:/images/button/btn_up.png);}");
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

void MainWidget::on_btn_favorite_clicked()
{
    ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_favorite);
}

void MainWidget::on_btn_recently_clicked()
{
    ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_recentlyPlayed);
}

void MainWidget::on_btn_songList_clicked()
{
    ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_defaultSongList);
}

void MainWidget::on_btn_localsong_clicked()
{
    ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_local);
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
    QString requrl  = "http://iwxyi.com:3000/search?keywords=";
    //QString requrl = "http://mobilecdn.kugou.com/api/v3/search/song?format=json&keyword=";
    requrl = requrl + keystring.toUtf8().toPercentEncoding();
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

void MainWidget::setSearchResultTable(SongList songs)
{
    QTableWidget *table = ui->tableWidget_search;
    // table->setAlternatingRowColors(true);
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

QString MainWidget::songPath(const Music &music) const
{
    return musicFileDir.absoluteFilePath(snum(music.id) + ".mp3");
}
QString MainWidget::lyricPath(const Music &music) const
{
    return musicFileDir.absoluteFilePath(snum(music.id) + ".lrc");
}
QString MainWidget::coverPath(const Music &music) const
{
    return musicFileDir.absoluteFilePath(snum(music.id) + ".jpg");
}

bool MainWidget::isSongDownloaded(Music music)
{
    return QFileInfo(songPath(music)).exists();
}

/**
 * 立即开始播放音乐
 */
void MainWidget::playLocalSong(Music music)
{
    qDebug() << "开始播放" << music.simpleString();
    //mySystemTray->setToolTip(music.simpleString());
    if (!isSongDownloaded(music))
    {
        qDebug() << "error:未下载歌曲：" << music.simpleString() << "开始下载";
        playAfterDownloaded = music;
        downloadSong(music);
        return ;
    }

    setLableSongInfo(music);
    //ui->slider_progress->setMaximum(music.duration /1000);


    //  设置封面
    if (QFileInfo(coverPath(music)).exists())
    {
        QPixmap pixmap(coverPath(music), "1");
        if (pixmap.isNull())
            qDebug() << "warning: 本地封面是空的" << music.simpleString() << coverPath(music);
        // 自适应高度
        pixmap = pixmap.scaledToHeight(ui->lable_songinfo->height());
        ui->lable_songinfo->setPixmap(pixmap);
        setCurrentCover(pixmap);
    }
    else
    {
        downloadSongCover(music);
    }

    // 设置
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
    player->setMedia(QUrl::fromLocalFile(songPath(music)));
    player->setPosition(0);
    player->play();
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
        if (orderSongs.contains(music)) {
            orderSongs.removeOne(music);
            settings.setValue("music/currentSong", "");
            setLableSongInfo(music);
            player->stop();
        }
        saveSongList("music/order", orderSongs);
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
    if (QFileInfo(lyricPath(music)).exists())
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
    if (QFileInfo(coverPath(music)).exists())
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
                pixmap = pixmap.scaledToHeight(ui->btn_albumpic->height());
                ui->btn_albumpic->setIcon(pixmap);
                setCurrentCover(pixmap);
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

    if (isSongDownloaded(music))
    {
        playLocalSong(music);
    }
    else
    {
        playAfterDownloaded = music;
        downloadSong(music);
    }
    if (!orderSongs.contains(music))
        orderSongs.append(music);
    saveSongList("music/order", orderSongs);
    //setPlayListTable(orderSongs, ui->MusicTable);

}

void MainWidget::appendOrderSongs(SongList musics)
{

}

void MainWidget::appendNextSongs(SongList musics)
{

}

void MainWidget::appendMusicToPlayList(SongList musics, int row)
{

}

void MainWidget::setCurrentCover(const QPixmap &pixmap)
{

}

void MainWidget::setCurrentLyric(QString lyric)
{
    //desktopLyric->setLyric(lyric);
    ui->scrollarea_lrc->loadLyric(lyric);
}

void MainWidget::addFavorite(SongList musics)
{

}

void MainWidget::removeFavorite(SongList musics)
{

}

void MainWidget::onPlayNowTriggered() {
    startPlaySong(menuCurrentSong);
}

void MainWidget::onPlayNextTriggered() {
    appendNextSongs(menuMusics);
}

void MainWidget::onAddToPlayListTriggered() {
    appendOrderSongs(menuMusics);
}

void MainWidget::onFavoriteTriggered() {
    if (!favoriteSongs.contains(menuCurrentSong))
        addFavorite(menuMusics);
    else
        removeFavorite(menuMusics);
}

void MainWidget::slotPlayerPositionChanged()
{
    qint64 position = player->position();

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
    qDebug() << "todo";
}


void MainWidget::on_btn_logo_clicked()
{
    QMessageBox::information(this, "关于", "name: MusicPlayer \n info: a qt project for learning \n author: haojoy");
}

//右键菜单请求
void MainWidget::handleContextMenuRequest(QTableWidget* table, const QPoint& pos) {
    auto items = table->selectedItems();
    foreach (auto item, items) {
        int row = table->row(item);
        int col = table->column(item);
        if (col == 0)
            menuMusics.append(songSearchResult.at(row));
    }

    int row = table->currentRow();
    if (row > -1)
        menuCurrentSong = songSearchResult.at(row);

    playListMenu->setEnabled(!songplaylist.isEmpty());
    favoriteAction->setText(favoriteSongs.contains(menuCurrentSong) ? "从我的喜欢中移除" : "添加到我的喜欢");
    // 添加歌单菜单项并连接信号
    for (int index = 0; index < songplaylist.size(); ++index) {
        QAction* playlistAction = new QAction(songplaylist.at(index).name, this);
        playListMenu->addAction(playlistAction);

        connect(playlistAction, &QAction::triggered, this, [=]() {
            appendMusicToPlayList(menuMusics, index);
        });
    }
    contextMenu->exec(table->viewport()->mapToGlobal(pos));
}

void MainWidget::on_tableWidget_search_customContextMenuRequested(const QPoint &pos)
{
    if (songSearchResult.isEmpty()) {
        return;
    }
    handleContextMenuRequest(ui->tableWidget_search, pos);
}


void MainWidget::on_tableWidget_search_itemDoubleClicked(QTableWidgetItem *item)
{
    int row = ui->tableWidget_search->row(item);
    Music currentsong;
    if (row > -1 )
        currentsong = songSearchResult.at(row);
    if (orderSongs.contains(currentsong))
    {
        orderSongs.removeOne(currentsong);
       // setPlayListTable(orderSongs, ui->MusicTable);
    }
    else
        orderSongs.insert(0, currentsong);
    startPlaySong(currentsong);
}


void MainWidget::on_btn_play_clicked()
{
    if (!orderSongs.size())
        return ;
    if (player->state() == QMediaPlayer::PlayingState)
    {
        player->pause();
    }
    else
    {
        if (!playingSong.isValid())
        {
            //playNext();
            return ;
        }
        player->play();
    }
}

