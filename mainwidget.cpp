#include "mainwidget.h"
#include "ui_mainwidget.h"


MainWidget::MainWidget(QWidget *parent)
    : SwitchAnimation(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    // UI相关初始化
    init_HandleUI();

    // 连接信号和槽
    init_HandleSignalsAndSlots();
}
// 界面初始化处理
void MainWidget::init_HandleUI(){
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);
    // 加载样式表
    loadStyleSheet(":/qss/qss/style.qss");
    ui->centralWidget->setStyleSheet("background-image: url(:/images/background.jpg);");
    ui->tabWidget->setCurrentWidget(ui->tab_homePage);
    ui->widget_bottom->setStyleSheet("background: transparent");
}

void MainWidget::loadStyleSheet(const QString &styleSheetFile) {
    QFile file(styleSheetFile);
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "Failed to open the style sheet file.";
    }
}

// 显示歌词界面
void MainWidget::showLyricsPage()
{
    this->slideAnimation(this, ui->widget_lrc, AnimDirection::Up);
    QTimer::singleShot(animTime, [=] {
        ui->tabWidget->setCurrentWidget(ui->tab_lrc);
    });
    ui->btn_albumpic->setStyleSheet("QPushButton::hover{border-image:url(:/images/button/btn_down.png);}");
}
// 显示歌词界面
void MainWidget::showHomePage()
{
    this->slideAnimation(this, ui->widget_lrc, AnimDirection::Down);
    ui->tabWidget->setCurrentWidget(ui->tab_homePage);
    ui->btn_albumpic->setStyleSheet("QPushButton::hover{border-image:url(:/images/button/btn_up.png);}");
    ui->edit_search->clearFocus();
}

// 连接信号和槽
void MainWidget::init_HandleSignalsAndSlots(){

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
    //ui->edit_search->clearFocus();

}

void MainWidget::on_btn_albumpic_clicked()
{
    if(ui->tabWidget->currentWidget() == ui->tab_homePage){
        showLyricsPage();
        ui->btn_albumpic->setToolTip(QString("收起音乐详情"));
    }else if(ui->tabWidget->currentWidget() == ui->tab_lrc){
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
    ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_defalutSongList);
}


void MainWidget::on_btn_localsong_clicked()
{
    ui->tabWidget_switchcontent->setCurrentWidget(ui->tab_local);
}

/*
void MainWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPixmap background(":/images/images/background.jpg"); // 背景图片路径
    painter.drawPixmap(0, 0, width(), height(), background);


    // 将背景图片绘制到窗口的整个区域
   // painter.drawPixmap(0, 0, width(), height(), background.scaled(width(), height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    // 确保透明区域保持透明
    //painter.setCompositionMode(QPainter::CompositionMode_Source);
    //painter.fillRect(rect(), Qt::transparent);
    // 继续处理其他的绘制事件
    QWidget::paintEvent(event);

}
*/

void MainWidget::on_btn_minsize_clicked()
{
    this->showMinimized();
}


void MainWidget::on_btn_maxsize_clicked()
{
    if (this->isMaximized()) {
        this->showNormal();
        ui->btn_maxsize->setIcon(QIcon(QPixmap(":/images/button/btn_restore.png")));
    }
    else {
        this->showMaximized();
        ui->btn_maxsize->setIcon(QIcon(QPixmap(":/images/button/btn_maximize.png")));
    }
}

