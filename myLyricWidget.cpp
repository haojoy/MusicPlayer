#include <QRegularExpression>
#include <QFontMetrics>
#include <QDateTime>
#include <QScrollArea>
#include <QWheelEvent>
#include <QDebug>
#include "myLyricWidget.h"

MyLyricWidget::MyLyricWidget(QWidget *parent)
    : QWidget(parent),
    settings(QApplication::applicationDirPath() + "/musics.ini", QSettings::Format::IniFormat),
    updateTimer(new QTimer(this)),
    currentRow(-1),
    switchRowTimestamp(0),
    lineSpacing(12),
    verticalMargin(5),
    pointSize(14),
    playingColor(QColor(248,216,129)),
    waitingColor(Qt::white),
    scrollValue(0)

{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showMenu()));

    // 从设置中加载颜色和字体大小配置，如果没有则使用默认值
    playingColor = qvariant_cast<QColor>(settings.value("lyric/playingColor", playingColor));
    waitingColor = qvariant_cast<QColor>(settings.value("lyric/waitingColor", waitingColor));
    pointSize = settings.value("lyric/pointSize", pointSize).toInt();

    // 连接定时器的超时信号到更新函数，并设置更新间隔
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    updateTimer->setInterval(16);
}

// 设置歌词文本
void MyLyricWidget::loadLyric(const QString &text)
{
    lyricStream.clear();  // 清空当前歌词列表

    // 歌词时间格式匹配正则表达式，支持 [mm:ss.ms] 或 [mm:ss:ms] 格式
    QRegularExpression re("\\[(\\d{2}):(\\d{2})[.:](\\d{2,3})\\](.*)");

    QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    QRegularExpressionMatch match;
    foreach (QString line, lines)
    {
        match = re.match(line);
        if (match.hasMatch())
        {
            LyricBean lyric;
            int minute = match.captured(1).toInt();
            int second = match.captured(2).toInt();
            int millisecond = match.captured(3).toInt();

            // 计算歌词开始时间（毫秒为单位）
            lyric.start = minute * 60000 + second * 1000 + millisecond;
            // 提取歌词文本
            lyric.text = match.captured(4).trimmed();
            // 添加到歌词流列表
            lyricStream.append(lyric);
        }
    }
    currentRow = 0;
    updateFixedHeight();    // 更新控件高度
    update();   // 更新界面显示
}
void MyLyricWidget::ensureCurrentRowVisible() {
    int currentTop = currentRow * lineSpacing;
    int visibleHeight = height();

    if (currentTop < scrollValue) {
        scrollValue = currentTop;
    } else if (currentTop + lineSpacing > scrollValue + visibleHeight) {
        scrollValue = currentTop + lineSpacing - visibleHeight;
    }
}

bool MyLyricWidget::setPosition(qint64 position)
{
    int prevRow = currentRow;
    if (lyricStream.isEmpty())
        return false;

    LyricBean lyric = lyricStream.at(currentRow);
    if (currentRow == lyricStream.size() - 1 && lyric.start <= position)
        return false;

    LyricBean nextLyric = currentRow == lyricStream.size() - 1 ? LyricBean() : lyricStream.at(currentRow + 1);
    if (lyric.start <= position && nextLyric.start >= position)
        return false;

    if (lyric.start > position)
        currentRow = 0;

    while (currentRow + 1 < lyricStream.size() && lyricStream.at(currentRow + 1).start < position)
    {
        currentRow++;
    }
    if (currentRow == prevRow)
        return false;

    switchRowTimestamp = QDateTime::currentMSecsSinceEpoch();
    updateTimer->start();
    ensureCurrentRowVisible();
    update();
    return true;
}

int MyLyricWidget::getCurrentTop() const
{
    return (currentRow + verticalMargin + 0.5) * lineSpacing;
}

void MyLyricWidget::setColors(const QColor &playing, const QColor &waiting)
{
    playingColor = playing;
    waitingColor = waiting;
    update();
}

void MyLyricWidget::paintEvent(QPaintEvent *event){

    QPainter painter(this);
    QFont font = painter.font();
    font.setPointSize(pointSize);
    font.setFamily("Arial");
    painter.setFont(font);
    QFontMetrics fm(font);
    /*
    lineSpacing = fm.height() + (fm.lineSpacing() - fm.height()) * 2 + 26;
    int endY = lyricStream.size() * lineSpacing;
    int visibleHeight = height();
    int maxScroll = qMax(0, endY - visibleHeight);
    if (scrollValue > maxScroll)
        scrollValue = maxScroll;

    painter.translate(0, -scrollValue);
    */
    // 计算基础行间距
    int baseLineSpacing = fm.height() + (fm.lineSpacing() - fm.height()) * 2;
    lineSpacing = baseLineSpacing * 2;  // 使用基础行间距，可以根据需要调整

    int totalHeight = lyricStream.size() * lineSpacing;
    int visibleHeight = height();

    // 计算当前播放行应该居中的位置
    int centerRowTop = currentRow * lineSpacing - (visibleHeight - lineSpacing) / 2;

    // 限制滚动范围，确保不超出可见区域
    int maxScroll = qMax(0, totalHeight - visibleHeight);
    scrollValue = qBound(0, centerRowTop, maxScroll);

    painter.translate(0, -scrollValue);

    for (int i = 0; i < lyricStream.size(); ++i) {
        int top = i * lineSpacing;
        QRect textRect(0, top, width(), lineSpacing);

        if (i == currentRow) {
            QFont currentFont = font;
            currentFont.setPointSize(pointSize + 4);
            painter.setFont(currentFont);
            painter.setPen(playingColor);
        } else {
            painter.setFont(font);
            painter.setPen(waitingColor);
        }

        painter.drawText(textRect, Qt::AlignCenter, lyricStream.at(i).text);
    }
}



void MyLyricWidget::updateFixedHeight()
{
    QFont font = this->font();
    font.setPointSize(pointSize);
    QFontMetrics fm(font);
    lineSpacing = fm.height() + 2; // 加 2 是为了适当增加行间距
    setFixedHeight((lyricStream.size() + 1) * lineSpacing);
}

void MyLyricWidget::showMenu()
{
    QScopedPointer<QMenu> menu(new QMenu(this));

    // 设置右键菜单的样式表
    menu->setStyleSheet(
        "QMenu {"
        "    background-color: #A99F99;" // 设置背景颜色
        "    color: white;"            // 设置文字颜色
        "    border: none;" // 设置边框颜色
        "}"
        "QMenu::item {"
        "    background-color: transparent;" // 设置菜单项背景透明
        "}"
        "QMenu::item:selected {"
        "    background-color: #555555;" // 设置选中菜单项背景颜色
        "}"
        );

    QAction *playingC = menu->addAction("选择已播放颜色", [=]() {
        QColor c = QColorDialog::getColor(playingColor, this, "选择已播放颜色", QColorDialog::ShowAlphaChannel);
        if (c.isValid() && c != playingColor)
        {
            settings.setValue("lyric/playingColor", playingColor = c);
            update();
        }
    });

    QAction *waitingC = menu->addAction("选择未播放颜色", [=]() {
        QColor c = QColorDialog::getColor(waitingColor, this, "选择未播放颜色", QColorDialog::ShowAlphaChannel);
        if (c.isValid() && c != waitingColor)
        {
            settings.setValue("lyric/waitingColor", waitingColor = c);
            update();
        }
    });

    QMenu *fontMenu = menu->addMenu("字体大小");
    for (int i = 8; i < 30; ++i)
    {
        QAction *fm = fontMenu->addAction(QString::number(i), [=]() {
            pointSize = i;
            settings.setValue("lyric/pointSize", pointSize);
            updateFixedHeight();
            update();
        });
        fm->setCheckable(true);
        if (pointSize == i)
            fm->setChecked(true);
    }

    menu->exec(QCursor::pos());
}
