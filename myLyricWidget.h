#ifndef MYLYRICWIDGET_H
#define MYLYRICWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QSettings>
#include <QTimer>
#include <QMenu>
#include <QColorDialog>
#include <QApplication>

const int switchRowDuration = 200;

struct LyricBean {
    qint64 start = 0;
    qint64 end = 0;
    QString text;
};

class MyLyricWidget : public QWidget {
    Q_OBJECT
public:
    explicit MyLyricWidget(QWidget *parent = nullptr);
    void loadLyric(const QString &text);
    bool setPosition(qint64 position);
    void setColors(const QColor &playing, const QColor &waiting);
    int getCurrentTop() const;

protected:
    void paintEvent(QPaintEvent *event) override;
signals:
    void switchCoverBlur();
private slots:
    void showMenu();

private:
    void updateFixedHeight();
    void ensureCurrentRowVisible();
private:
    QSettings settings;
    QVector<LyricBean> lyricStream;
    int currentRow;
    qint64 switchRowTimestamp;
    QTimer *updateTimer;
    int lineSpacing;
    int verticalMargin;
    int pointSize;
    QColor playingColor;
    QColor waitingColor;
    int scrollValue;
};


#endif // MYLYRICWIDGET_H
