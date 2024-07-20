#include "taglibHelper.h"

TaglibHelper::TaglibHelper()
{

}

TaglibHelper::~TaglibHelper()
{

}

Music TaglibHelper::GetMusicTagInfo(const QString& filepath) {
    Music music;
    FileRef file(filepath.toStdWString().c_str());
    Tag *tag = file.tag();
    AudioProperties *properties = file.audioProperties();

    if (tag != nullptr) {
        music.name = QString::fromStdString(tag->title().to8Bit(true));
        music.artists.push_back({0, QString::fromStdString(tag->artist().to8Bit(true)), ""});
        music.album.name = QString::fromStdString(tag->album().to8Bit(true));
    }

    if (properties != nullptr) {
        music.duration = properties->lengthInMilliseconds();
    }

    return music;
}

// 保存专辑封面
void TaglibHelper::saveAlbumCover(ID3v2::Tag *tag, const QString &path) {
    ID3v2::FrameList frames = tag->frameListMap()["APIC"];
    for (auto it = frames.begin(); it != frames.end(); ++it) {
        ID3v2::AttachedPictureFrame *frame = dynamic_cast<ID3v2::AttachedPictureFrame *>(*it);
        if (frame) {
            QFile file(path);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(frame->picture().data(), frame->picture().size());
                file.close();
                qDebug() << "Album cover saved to" << path;
            } else {
                qWarning() << "Failed to save album cover to" << path;
            }
            return;
        }
    }
    qDebug() << "No album cover found.";
}

// 保存歌词
void TaglibHelper::saveLyrics(ID3v2::Tag *tag, const QString &path) {
    ID3v2::FrameList frames = tag->frameListMap()["USLT"];
    if (!frames.isEmpty()) {
        ID3v2::UnsynchronizedLyricsFrame *lyricsFrame = dynamic_cast<ID3v2::UnsynchronizedLyricsFrame *>(frames.front());
        if (lyricsFrame) {
            QString lyrics = QString::fromStdString(lyricsFrame->text().to8Bit(true));
            QFile file(path);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file.write(lyrics.toUtf8());
                file.close();
                qDebug() << "Lyrics saved to" << path;
            } else {
                qWarning() << "Failed to save lyrics to" << path;
            }
            return;
        }
    }
    qDebug() << "No lyrics found.";
}

void TaglibHelper::GetAlbumCoverAndLyrics(const QString& songpath, QString &coverpath, QString &lyricsPath) {
    MPEG::File file(songpath.toStdString().c_str());
    if (!file.isValid()) {
        qCritical() << "File is not valid or could not be opened!";
        return;
    }

    ID3v2::Tag *tag = file.ID3v2Tag();
    if (tag) {
        // 获取并保存专辑封面
        saveAlbumCover(tag, coverpath);
        // 获取并保存内嵌歌词
        saveLyrics(tag, lyricsPath);
    } else {
        qWarning() << "No ID3v2 tag found in the file.";
    }
}

