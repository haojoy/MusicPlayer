#ifndef MUSIC_H
#define MUSIC_H

#include <QUrl>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QDateTime>
#include <QSettings>

#define JSON_VAL_LONG(json, x) static_cast<qint64>(json.value(#x).toDouble())
#define JVAL_LONG(x) static_cast<qint64>(json.value(#x).toDouble())
#define JVAL_INT(x) json.value(#x).toInt()
#define JVAL_STR(x) json.value(#x).toString()
#define snum(x) QString::number(x)

struct Artist
{
    qint64 id = 0;
    QString name;
    QString faceUrl;

    static Artist fromJson(const QJsonObject& json)
    {
        Artist artist;
        artist.id = JVAL_LONG(id);
        artist.name = JVAL_STR(name);
        artist.faceUrl = JVAL_STR(img1v1Url);
        return artist;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json.insert("id", id);
        json.insert("name", name);
        json.insert("faceUrl", faceUrl);
        return json;
    }
    bool operator==(const Artist &other) const {
        return name == other.name;
    }
};

struct Album
{
    qint64 id = 0;
    QString name;
    int size = 0;
    int mark = 0;

    static Album fromJson(const QJsonObject& json)
    {
        Album album;
        album.id = JVAL_LONG(id);
        album.name = JVAL_STR(name);
        album.size = JVAL_INT(size);
        return album;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json.insert("id", id);
        json.insert("name", name);
        json.insert("size", size);
        return json;
    }
    bool operator==(const Album &other) const {
        return name == other.name;
    }
};

struct Music
{
    qint64 id = 0;
    QString name;
    int duration = 0;
    QList<Artist> artists;
    Album album;
    QString artistNames;

    static Music fromJson(const QJsonObject& json)
    {
        Music music;
        music.id = JVAL_LONG(id);
        music.name = JVAL_STR(name);
        QJsonArray array = json.value("artists").toArray();
        QStringList artistNameList;
        foreach (QJsonValue val, array)
        {
            Artist artist = Artist::fromJson(val.toObject());
            music.artists.append(artist);
            artistNameList.append(artist.name);
        }

        music.artistNames = artistNameList.join("/");
        music.album = Album::fromJson(json.value("album").toObject());
        music.duration = JVAL_INT(duration);
        return music;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json.insert("id", id);
        json.insert("name", name);
        json.insert("duration", duration);
        QJsonArray array;
        foreach (const Artist& artist, artists)
            array.append(artist.toJson());
        json.insert("artists", array);
        json.insert("album", album.toJson());
        return json;
    }

    bool isValid() const
    {
        // return id != 0;
        return !name.isEmpty() && duration != 0;
    }

    bool operator==(const Music &other) const
    {
        return id == other.id && name == other.name && artists == other.artists &&
            album.name == other.album.name  && duration == other.duration;
    }

    QString simpleString() const
    {
        return name + "-" + artistNames;
    }
};

struct PlayListCreator
{
    QString nickname;
    QString signature;
    QString avatarUrl;

    static PlayListCreator fromJson(const QJsonObject& json)
    {
        PlayListCreator creator;
        creator.nickname = JVAL_STR(nickname);
        creator.signature = JVAL_STR(signature);
        creator.avatarUrl = JVAL_STR(avatarUrl);
        return creator;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json.insert("nickname", nickname);
        json.insert("signature", signature);
        json.insert("avatarUrl", avatarUrl);
        return json;
    }
};

struct PlayList
{
    QString name;
    qint64 id;
    QString description;
    QStringList tags;
    int playCount;
    PlayListCreator creator;
    QList<Music> containsMusic;

    static PlayList fromJson(const QJsonObject& json)
    {
        PlayList playlist;
        playlist.name = JVAL_STR(name);
        playlist.id = JVAL_LONG(id);
        playlist.description = JVAL_STR(description);
        playlist.playCount = JVAL_INT(playCount);
        playlist.creator = PlayListCreator::fromJson(json.value("creator").toObject());
        QJsonArray tagsArray = json.value("tags").toArray();
        foreach (const QJsonValue& val, tagsArray)
            playlist.tags.append(val.toString());
        QJsonArray musicArray = json.value("containsMusic").toArray();
        foreach (const QJsonValue& val, musicArray)
        {
            Music music = Music::fromJson(val.toObject());
            playlist.containsMusic.append(music);
        }
        return playlist;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json.insert("name", name);
        json.insert("id", id);
        json.insert("description", description);
        json.insert("playCount", playCount);
        json.insert("creator", creator.toJson());
        QJsonArray tagsArray;
        foreach (const QString& tag, tags)
            tagsArray.append(tag);
        json.insert("tags", tagsArray);
        QJsonArray musicArray;
        foreach (const Music& music, containsMusic)
            musicArray.append(music.toJson());
        json.insert("containsMusic", musicArray);
        return json;
    }

    bool operator==(const PlayList& pl) const
    {
        return this->id == pl.id;
    }
};

typedef QList<Music> SongList;
typedef QList<PlayList> PlayListList;

#endif // MUSIC_H

