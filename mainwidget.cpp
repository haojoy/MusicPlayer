#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : SwitchAnimation(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    networkManager = new QNetworkAccessManager(this);
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
    //ui->centralWidget->setStyleSheet("background-image: url(:/images/background.jpg); background-color:rgba(97, 76, 64, 127);");
    // qss style QHeaderView::section
    ui->tableWidget_search->horizontalHeader()->setStyleSheet("background-color:transparent");
    ui->tableWidget_search->verticalHeader()->setStyleSheet("background-color:transparent");


}

void MainWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // // 绘制背景图像
    // QPixmap pixmap(":/images/background.jpg");
    // painter.drawPixmap(rect(), pixmap);

    // // 设置背景颜色透明度
    // painter.fillRect(rect(), QColor(97, 76, 64, 127)); // 白色，透明度为 100

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

    // 回车搜索
    connect(ui->edit_search, &QLineEdit::returnPressed, this, [=] {
        ui->btn_search->click();
    });
    // 搜索请求响应
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWidget::handleNetworkResponseData);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::on_btn_close_clicked()
{
    this->close();
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

void MainWidget::on_btn_close2_clicked()
{
    this->close();
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
        ui->btn_maxsize->setIcon(QIcon(QPixmap(":/images/button/btn_restore.png")));
    } else {
        this->showMaximized();
        ui->btn_maxsize->setIcon(QIcon(QPixmap(":/images/button/btn_maximize.png")));
    }
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


void MainWidget::musicSearch(const QString &keystring) {
    QString requrl  = "http://iwxyi.com:3000/search?keywords=";
    //QString requrl = "http://mobilecdn.kugou.com/api/v3/search/song?format=json&keyword=";
    requrl = requrl + keystring.toUtf8().toPercentEncoding();
    QNetworkRequest request = QNetworkRequest(QUrl(requrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36");
    networkManager->get(request);
}

void MainWidget::setSearchResultTable(SongList songs)
{
    enum{
        titleCol,
        artistCol,
        albumCol,
        durationCol
    };

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
}

QString MainWidget::msecondToString(int mseconds) {
    int seconds = mseconds / 1000;
    int minutes = seconds / 60;
    seconds %= 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

void MainWidget::on_btn_setskin_clicked()
{
    qDebug() << "todo";
}

