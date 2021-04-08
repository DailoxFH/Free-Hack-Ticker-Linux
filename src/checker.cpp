#include "checker.h"

#include "mainwindow.h"

Checker::Checker(MainWindow *ui, std::string *refCookiePath, QString &configPath, bool *encryptCookie)
{
    this->ui = ui;
    oldChatBoxPath = configPath+"/old_chatbox.xml";
    oldDataPath = configPath+"/old_data.xml";
    defCookiePath = configPath+"/cookies.txt";

    parser = new HtmlParser();
    connection = new Connection(ui, refCookiePath, encryptCookie);


}

Checker::~Checker() {
    if(ui->amILoggedIn && !ui->rememberMe) {
        logout();
    }
    delete connection;
    delete parser;
}

void Checker::checkForNewCBMessages() {
    if(!ui->amILoggedIn || !ui->updateChatBox) {
        return;
    }

    QString newChatBox = QString::fromStdString(connection->getChatBox());

    generalCheck(oldChatBoxPath, newChatBox, false);
}

void Checker::generalCheck(QString fPath, QString newData, bool posts) {
    if(!ui->amILoggedIn) {
        return;
    }
    if(!posts && !ui->updateChatBox) {
        return;
    }

    if(!QFile::exists(fPath)) {
        writeToFile(fPath, newData);
        return;
    }

    QFile file(fPath);

    QString oldData;

    if(file.open(QIODevice::ReadOnly)) {
        QTextStream instream(&file);
        oldData = instream.readAll();
    }

    file.close();

    QMultiMap<int, QString> newMappedData;
    QMultiMap<int, QString> oldMappedData;

    if(posts) {
        parser->parsePosts(newData, newMappedData);
        parser->parsePosts(oldData, oldMappedData);
    } else {
        parser->parseChatBox(newData, newMappedData);
        parser->parseChatBox(oldData, oldMappedData);
    }

    if(checkForUpdates(oldMappedData, newMappedData, posts)) {
        writeToFile(fPath, newData);
    }
}

void Checker::checkForNewPosts() {
    QString newPosts = QString::fromStdString(connection->getPosts());
    generalCheck(oldDataPath, newPosts, true);
}

void Checker::checkForNewPMs() {
    if(!ui->amILoggedIn || !ui->updatePMs) {
        return;
    }
    QString result;
    parser->parsePMs(QString::fromStdString(connection->getPMs()), result);
    QByteArray temp = result.toUtf8();
    char *pm = temp.data();
    char *hashLastPM = connection->str2md5(pm, strlen(pm));
    QString hshLastPM = QString::fromUtf8(hashLastPM);
    temp.remove(0, temp.length());
    if(ui->loadSettings.contains("lastPM")) {
        if(ui->loadSettings.value("lastPM").toString() != hshLastPM) {
            ui->showMessage("Neue private Nachricht", "Du hast eine neue private Nachricht");
            ui->loadSettings.setValue("lastPM", hshLastPM);
        }
    } else {
        ui->loadSettings.setValue("lastPM", hshLastPM);
    }
}

void Checker::checkLoginByCookie() {
    if(!ui->amILoggedIn) {
        return;
    }
    if(ui->cook != "") {
        ui->loggedIn(QFile::exists(QString::fromStdString(ui->cook)));
    } else {
        ui->loggedIn(connection->checkCookie());
    }
}

void Checker::checkLoginByCheck(bool login) {
    bool ret = connection->isLoggedIn();
    if(!ret) {
        QFile::remove(defCookiePath);
        if(ui->isHidden() || login) {
            ui->showMessage("Ausgeloggt", "Du scheinst ausgeloggt zu sein. Entweder ist dein Nutzername/Passwort falsch oder deine Sitzung ist abgelaufen. Bitte logge dich erneut ein.", QSystemTrayIcon::Critical);
        }
    }
    ui->loggedIn(ret);
}

void Checker::doGlobalUpdate() {
    checkLoginByCheck();
    checkForNewPosts();
    checkForNewCBMessages();
    checkForNewPMs();
}


void Checker::login(char *username, char *password) {
    std::string hashedPW = connection->str2md5(password, strlen(password));
    connection->login(username, hashedPW.c_str());
    checkLoginByCheck(true);
    memset(&hashedPW, 0, sizeof(hashedPW));

    doGlobalUpdate();
}

void Checker::logout() {
    QString html = QString::fromStdString(connection->getMainSite());
    QString out;
    parser->parseLogout(html, out);
    std::string final = out.toStdString();
    connection->logOut(final);

    QFile::remove(defCookiePath);

    connection->cleanup(true);

    ui->loggedIn(false);
    ui->rememberMe = false;
}



bool Checker::checkForUpdates(QMultiMap<int, QString> oldBox, QMultiMap<int,QString> newBox, bool posts) {
    int toSearch;
    bool changed = false;

    QStringList oldList = oldBox.values(0);
    QStringList newList = newBox.values(0);
    if(oldList.at(HtmlParser::TIME) != newList.at(HtmlParser::TIME)) {
        toSearch = HtmlParser::TIME;
        changed = true;
    }

    if(!posts) {
        if(oldList.at(HtmlParser::TOPIC) != newList.at(HtmlParser::TOPIC)) {
            toSearch = HtmlParser::TOPIC;
            changed = true;

        }
    }

    if(changed) {
        QStringList list1;
        QStringList list2;
        parser->getValues(oldBox, toSearch, list1);
        parser->getValues(newBox, toSearch, list2);

        //Optional: Listen reversen. Könnte man noch implementieren.
//        for(int k = 0; k < (list1.size()/2); k++) list1.swapItemsAt(k,list1.size()-(1+k));
//        for(int k = 0; k < (list2.size()/2); k++) list2.swapItemsAt(k,list2.size()-(1+k));

        for(int i=0; i < list2.length(); i++) {
            if(list2.count(list2.at(i)) > list1.count(list2.at(i))) {
                QStringList result;
                parser->getDifferenceByValue(newBox, list2.at(i), result);
                int count = 0;
                QString message;

                QString topic;
                QString time;
                QString owner;

                foreach(auto &value, result) {
                    if(value == "") {
                        count = 0;
                        continue;

                    }
                    if(!posts && count == 2) {
                        time = time.replace("]", "");
                        time = time.replace("[", "");
                        QString msg = "'"+message+"'\n Erstellt von "+value+" am "+time;
                        ui->showMessage("Neue ChatBox Nachricht", msg);
                        count = 0;
                    } else if(count == HtmlParser::MAX) {
                        const QString msg = "'"+value+"'\n "+owner+" am '"+time+"' in '"+topic+"'";
                        ui->showMessage("Neuer Beitrag", msg);
                        count = 0;
                    }

                    switch(count) {
                        case HtmlParser::OWNER:
                            owner=value;
                            break;
                        case HtmlParser::TIME:
                            time=value;
                            break;
                        case HtmlParser::TOPIC:
                            if(!posts) {
                                message=value;
                            } else {
                                topic=value;
                            }
                            break;
                    }
                    count++;
                }
            }
        }
        return true;
    }
    return false;
}

void Checker::writeToFile(const QString path, const QString final) {
    QFile file(path);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QByteArray ba = final.toUtf8();
    const char *end = ba.data();
    file.write(end);
    file.close();
}
