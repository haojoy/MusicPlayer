#ifndef CHANGEBACKGROUND_H
#define CHANGEBACKGROUND_H

#include <QWidget>

namespace Ui {
class ChangeBackground;
}

class ChangeBackground : public QWidget
{
    Q_OBJECT

public:
    explicit ChangeBackground(QWidget *parent = nullptr);
    ~ChangeBackground();

protected:
    //void leaveEvent(QEvent *event) override;

public:
    int getBackColor() const;
    int getBackImage() const;

signals:
    void backgroundChanged(const QColor &color, const QString &imagePath);
private slots:

    void on_btn_colorswitch_clicked();

    void on_btn_imageswitch_clicked();

    // 背景颜色选择
    void on_btn_color1_clicked();

    void on_btn_color2_clicked();

    void on_btn_color3_clicked();

    void on_btn_color4_clicked();

    void on_btn_color5_clicked();

    void on_btn_color6_clicked();

    //背景图片选择
    void on_btn_image1_clicked();

    void on_btn_image2_clicked();

    void on_btn_image3_clicked();

    void on_btn_image4_clicked();

    void on_btn_image5_clicked();

    void on_btn_image6_clicked();

    void on_btn_choose_clicked();

private:
    Ui::ChangeBackground *ui;

    QColor currentSelectColor;
    QString currentSelectImage;

};

#endif // CHANGEBACKGROUND_H
