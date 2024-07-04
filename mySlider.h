#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QSlider>
#include <QPainter>
#include <QMouseEvent>
#include <QStyle>

class MySlider : public QSlider
{
public:
    MySlider(QWidget *parent = nullptr) : QSlider(Qt::Horizontal, parent) {
        // 设置样式表
        setStyleSheet(R"(
            QSlider::groove:horizontal {
                border: 1px solid #bbb;
                background: #ddd;
                height: 2px;
                border-radius: 4px;
            }

            QSlider::handle:horizontal {
                background: rgb(254,125,125); /* 使滑块背景透明，以便绘制自定义文本 */
                border-radius: 6px;
                width: 100px; /* 根据文本的宽度调整滑块的宽度 */
                height: 20px;
                margin: -7px 0;
            }

            QSlider::sub-page:horizontal {
                background: rgb(254,125,125);
            }

            QSlider::add-page:horizontal {
                background: #C0C0C0;
            }
        )");
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QSlider::paintEvent(event);

        QPainter painter(this);

        // 设置字体颜色
        QPen pen(Qt::white); // 设置为白色
        painter.setPen(pen);

        // 获取当前时间和总时间
        int currentValue = value();
        int totalValue = maximum();
        if(totalValue == 0)
            return;

        // 格式化时间
        QString currentTime = formatTime(currentValue);
        QString totalTime = formatTime(totalValue);

        // 计算滑块位置
        int handleX = (currentValue * (width() - 100)) / totalValue; // 100是滑块的宽度

        // 绘制滑块文本
        QRect handleRect(handleX, 0, 100, height());
        QString timeText = currentTime + "/" + totalTime;
        painter.drawText(handleRect, Qt::AlignCenter, timeText);
    }

    // 鼠标单击到指定位置播放
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            int sliderPosition = QStyle::sliderValueFromPosition(
                minimum(),
                maximum(),
                event->x(),
                width()
                );
            setValue(sliderPosition);
            emit sliderMoved(sliderPosition);
            emit valueChanged(sliderPosition);
        }
        QSlider::mousePressEvent(event);

    }

private:
    QString formatTime(int seconds)
    {
        int minutes = seconds / 60;
        int secs = seconds % 60;
        return QString::asprintf("%02d:%02d", minutes, secs);
    }
};

#endif // MYSLIDER_H
