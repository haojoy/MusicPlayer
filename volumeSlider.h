#ifndef VOLUMESLIDER_H
#define VOLUMESLIDER_H
#include <QWidget>
#include <QVBoxLayout>
#include <QSlider>
#include <QEvent>
#include <QTimer>
#include <QPushButton>
#include <QLabel>

// class MainWidget;
class VolumeSlider : public QWidget
{
    Q_OBJECT

public:
    explicit VolumeSlider(QPushButton *volumeButton,QWidget *parent = nullptr)
        : QWidget(parent, Qt::Window | Qt::FramelessWindowHint),
        m_volumeButton(volumeButton)
    {
        setFixedSize(60, 200);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        // 创建水平布局用于水平居中
        QHBoxLayout *hLayout = new QHBoxLayout();

        volumeSlider = new QSlider(Qt::Vertical, this);
        volumeSlider->setRange(0, 100);
        volumeSlider->setValue(20);

        // 设置样式
        volumeSlider->setStyleSheet(R"(
            QSlider::groove:vertical {
                border: 1px solid #bbb;
                background: #ddd;
                width: 2px;
                border-radius: 4px;
            }
            QSlider::handle:vertical {
                background: rgb(254,125,125);
                border-style: solid;
                border-width: 1px;
                border-color: rgb(207,207,207);
                height: 12px;
                margin: 0 -5px;
                border-radius: 7px;
            }
            QSlider::sub-page:vertical {
                background: #C0C0C0;
            }

            QSlider::add-page:vertical {
                background: rgb(254,125,125);
            }
        )");


        volumeValue = new QLabel(this);
        volumeValue->setAlignment(Qt::AlignCenter);  // 设置文本居中对齐

        // 创建垂直布局用于垂直居中
        QVBoxLayout *vLayout = new QVBoxLayout();
        vLayout->addWidget(volumeSlider);
        vLayout->addWidget(volumeValue);

        // 将垂直布局添加到水平布局中
        hLayout->addStretch();
        hLayout->addLayout(vLayout);
        hLayout->addStretch();

        // 将水平布局添加到主布局中
        mainLayout->addLayout(hLayout);
        volumeValue->setStyleSheet(R"(
            QLabel {
                color: white;
            }
        )");
        volumeValue->setText(QString::number(volumeSlider->value())+"%");

        // 连接滑块的 valueChanged 信号到更新标签的槽
        connect(volumeSlider, &QSlider::valueChanged, this, &VolumeSlider::updateVolumeLabel);

        // 安装事件过滤器
        installEventFilter(this);

        setLayout(mainLayout);

        setStyleSheet(R"(
            VolumeSlider {
                background-color: #7f726b;  /* 设置背景颜色 */
            }
        )");
    }

    void setSliderValue(int value)
    {
        volumeSlider->setValue(value);
    }

    int getSliderValue() const
    {
        return volumeSlider->value();
    }



    QSlider* getSlider() const
    {
        return volumeSlider;
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == this && event->type() == QEvent::Leave) {

            QRect volumeSliderRect = volumeSlider->geometry();
            QRect volumeButtonRect = m_volumeButton->geometry();
            QRect combinedRect = volumeSliderRect.united(volumeButtonRect);
            if (!combinedRect.contains(QCursor::pos())) {
                // 使用 QTimer 实现延迟隐藏
                QTimer::singleShot(1000, this, [=]() {
                    hide();
                });
                return true;
            }

        }
        return QWidget::eventFilter(watched, event);
    }

    // friend class MainWidget;
private slots:
    void updateVolumeLabel(int value)
    {
        volumeValue->setText(QString::number(value)+"%");
    }
private:
    QSlider *volumeSlider;
    QPushButton *m_volumeButton;
    QLabel *volumeValue;
};


#endif // VOLUMESLIDER_H
