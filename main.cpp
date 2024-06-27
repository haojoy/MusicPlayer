#include "mainwidget.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "MusicPlayer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWidget w;
    // 外层窗口透明
    w.setAttribute(Qt::WA_TranslucentBackground);
    // w.setAutoFillBackground(false);
    w.show();
    return a.exec();
}
