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
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    // 加载样式表
    loadStyleSheet(":/qss/qss/style.qss");
    ui->centralWidget->setStyleSheet("background-image: url(:/images/background.jpg);");
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



void MainWidget::on_btn_close_2_clicked()
{
    this->close();
}

void MainWidget::on_btn_down_clicked()
{
    this->slideAnimation(this, ui->widget_lrc, AnimDirection::Down);
    ui->tabWidget->setCurrentWidget(ui->tab_homePage);
    ui->edit_search->clearFocus();

}

void MainWidget::on_btn_albumpic_clicked()
{
    showLyricsPage();
}




