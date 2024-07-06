#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include "changebackground.h"
#include "ui_changebackground.h"


ChangeBackground::ChangeBackground(QWidget *parent)
    : QWidget(parent,Qt::FramelessWindowHint)
    , ui(new Ui::ChangeBackground)
{
    ui->setupUi(this);
    //this->setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet(R"(
        QWidget{
            border:1px;
            background:#A79D98;
            border-radius:8px;
        }
        QPushButton{
            background:rgba(97, 76, 64,127);
            border-radius: 8px;
        }
        QPushButton#btn_color1{
            background:rgb(97, 76, 64);
            border:0px;
        }
        QPushButton#btn_color2{
            background:rgb(73,56,52);
            border:0px;
        }
        QPushButton#btn_color3{
            background:rgb(54,66,81);
            border:0px;
        }
        QPushButton#btn_color4{
            background:rgb(54,60,143);
            border:0px;
        }
        QPushButton#btn_color5{
            background:rgb(52,80,97);
            border:0px;
        }
        QPushButton#btn_color6{
            background:rgb(167,58,88);
            border:0px;
        }
        QPushButton#btn_image1{
            background-image: url(:/images/background.jpg);
            background-position: left center;
            background-repeat: no-repeat;
        }
        QPushButton#btn_image2{
            background-image: url(:/images/backimages/back_image2_min.jpg);
            background-position: left center;
            background-repeat: no-repeat;
        }
        QPushButton#btn_image3{
            background-image: url(:/images/backimages/back_image3_min.jpg);
            background-position: left center;
            background-repeat: no-repeat;
        }
        QPushButton#btn_image4{
            background-image: url(:/images/backimages/back_image4_min.jpg);
            background-position: left center;
            background-repeat: no-repeat;
        }
        QPushButton#btn_image5{
            background-image: url(:/images/backimages/back_image5_min.jpg);
            background-position: left center;
            background-repeat: no-repeat;
        }
        QPushButton#btn_image6{
            background-image: url(:/images/backimages/back_image6_min.jpg);
            background-position: left center;
            background-repeat: no-repeat;
        }
    )");
}

ChangeBackground::~ChangeBackground()
{
    delete ui;
}

// void ChangeBackground::leaveEvent(QEvent *event)
// {
//     Q_UNUSED(event);
//     this->close();
// }

void ChangeBackground::on_btn_colorswitch_clicked()
{
    ui->tabWidget_back->setCurrentWidget(ui->tab_color);
}


void ChangeBackground::on_btn_imageswitch_clicked()
{
    ui->tabWidget_back->setCurrentWidget(ui->tab_image);
}

void ChangeBackground::on_btn_color1_clicked()
{
    currentSelectColor = QColor(97, 76, 64,127);
    emit backgroundChanged(currentSelectColor, "");
}


void ChangeBackground::on_btn_color2_clicked()
{
    currentSelectColor = QColor(73,56,52,127);
    emit backgroundChanged(currentSelectColor, "");
}


void ChangeBackground::on_btn_color3_clicked()
{
    currentSelectColor = QColor(54,66,81,127);
    emit backgroundChanged(currentSelectColor, "");
}


void ChangeBackground::on_btn_color4_clicked()
{
    currentSelectColor = QColor(54,60,143,127);
    emit backgroundChanged(currentSelectColor, "");
}


void ChangeBackground::on_btn_color5_clicked()
{
    currentSelectColor = QColor(52,80,97,127);
    emit backgroundChanged(currentSelectColor, "");
}


void ChangeBackground::on_btn_color6_clicked()
{
    currentSelectColor = QColor(167,58,88,127);
    emit backgroundChanged(currentSelectColor, "");
}

void ChangeBackground::on_btn_image1_clicked()
{
    currentSelectImage = ":/images/background.jpg";
    emit backgroundChanged(QColor(), currentSelectImage);
}


void ChangeBackground::on_btn_image2_clicked()
{
    currentSelectImage = ":/images/backimages/back_image2.jpg";
    emit backgroundChanged(QColor(), currentSelectImage);
}


void ChangeBackground::on_btn_image3_clicked()
{
    currentSelectImage = ":/images/backimages/back_image3.jpg";
    emit backgroundChanged(QColor(), currentSelectImage);
}


void ChangeBackground::on_btn_image4_clicked()
{
    currentSelectImage = ":/images/backimages/back_image4.jpg";
    emit backgroundChanged(QColor(), currentSelectImage);
}


void ChangeBackground::on_btn_image5_clicked()
{
    currentSelectImage = ":/images/backimages/back_image5.jpg";
    emit backgroundChanged(QColor(), currentSelectImage);
}


void ChangeBackground::on_btn_image6_clicked()
{
    currentSelectImage = ":/images/backimages/back_image6.jpg";
    emit backgroundChanged(QColor(), currentSelectImage);
}

void ChangeBackground::on_btn_choose_clicked()
{
    // 获取当前应用程序路径
    QString defaultPath = QDir::currentPath();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), defaultPath, tr("Image Files (*.png *.jpg *.bmp)"));
    if (!fileName.isEmpty()) {
        currentSelectImage = fileName;
        emit backgroundChanged(QColor(), currentSelectImage);
    }
}


