#ifndef HTMLPARSER_H
#define HTMLPARSER_H
#include <QString>
#include <QStringList>
#include <QMultiHash>
#include <QMultiMap>
#include <QRegularExpression>
#include <QMessageBox>

class HtmlParser
{
public:
    static const int MAX = 3;

    static const int TOPIC = 0;
    static const int TIME = 1;
    static const int OWNER = 2;
    static const int TITLE = 3;

    HtmlParser();
    void parsePosts(const QString &html, QMultiMap<int, QString> &output);
    void getValues(QMultiMap<int, QString> &hash, int what, QStringList &output);
    void getDifferenceByValue(QMultiMap<int, QString> &hash, const QString &where, QStringList &output);
    void parsePMs(const QString &html, QString &output);
    void parseChatBox(const QString &html, QMultiMap<int, QString> &output);
    void getDifferenceByValueChatBox(QString what, QStringList &output);
    void parseLogout(const QString &html, QString &output);
private:
    QString html;
};

#endif // HTMLPARSER_H
