#ifndef CHECKER_H
#define CHECKER_H

#include "htmlparser.h"
#include "connection.h"

class MainWindow;


class Checker
{
public:
    Checker(MainWindow *ui, std::string *refCookiePath, QString &configPath);
    ~Checker();
    void checkLoginByCookie();
    void checkLoginByCheck(bool login=false);
    void checkForNewCBMessages();
    void checkForNewPosts();
    void checkForNewPMs();

    void login(char *username, char *password);
    void logout();

    bool checkForUpdates(QMultiMap<int, QString> box1, QMultiMap<int,QString> box2, bool posts=false);
    void doGlobalUpdate();
    void writeToFile(const QString path, const QString final);
private:
    HtmlParser *parser;
    MainWindow *ui;
    Connection *connection;

    QString oldChatBoxPath;
    QString oldDataPath;
    QString defCookiePath;

    bool isAlreadyRunning;

    void generalCheck(QString fPath, QString newData, bool posts);

};


#endif // CHECKER_H
