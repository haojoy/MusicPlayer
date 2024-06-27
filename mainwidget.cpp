#include "mainwidget.h"
#include "ui_mainwidget.h"

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
MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    // 加载样式表
    loadStyleSheet(":/qss/qss/style.qss");
    //ui->tabWidget->tabBar()->hide();
    ui->centralWidget->setStyleSheet("background-image: url(:/images/images/background.jpg);");
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

