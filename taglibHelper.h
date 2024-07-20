#ifndef TAGLIBHELPER_H
#define TAGLIBHELPER_H

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/textidentificationframe.h>
#include <taglib/unsynchronizedlyricsframe.h>
#include <QImage>
#include <QFile>
#include <QByteArray>
#include <QBuffer>
#include "music.h"

using namespace TagLib;

class TaglibHelper{
public:
    TaglibHelper();
    ~TaglibHelper();

private:
    static void saveAlbumCover(TagLib::ID3v2::Tag *tag, const QString &path);
    static void saveLyrics(TagLib::ID3v2::Tag *tag, const QString &path);
public:
    static Music GetMusicTagInfo(const QString& filepath);
    static void GetAlbumCoverAndLyrics(const QString& songpath, QString &coverpath, QString &lyricsPath);
};

#endif // TAGLIBHELPER_H
