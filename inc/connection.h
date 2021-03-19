#ifndef CONNECTION_H
#define CONNECTION_H

#include <openssl/md5.h>
#include <string>
//#include <sstream>
//#include <iomanip>
//#include <iostream>
#include <curl/curl.h>
#include <cstring>
#include <QTextStream>
#include <QFile>
#include <QString>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>

class Connection
{
public:
    //TODO: Mal aus dem Mist ne Liste oder so machen
    Connection(std::string *path, bool *encryptCookie);
    ~Connection();
    char *str2md5(const char *str, int length);

    void login(const char *username, const char *password);
    std::string getPosts();
    std::string getPMs();
    bool checkCookie(bool writeToFile=false);
    void readFromFileAndSetCookie(std::string &file);
    void logOut(std::string &logoutHash);
    std::string getMainSite();
    bool isLoggedIn();
    void cleanup(bool noCleanup=false);
    std::string getChatBox();
    std::string request(std::string url, std::string username="", std::string password="");
private:

    const std::string BASE_URL = "https://free-hack.com/";
    const std::string POST_URL = BASE_URL+"misc.php?show=latestposts&vsacb_resnr=15";
    const std::string LOGOUT_URL = BASE_URL+"login.php?do=logout&";
    const std::string PM_URL = BASE_URL+"private.php?folderid=0";
    const std::string CHATBOX_URL = BASE_URL+"misc.php?show=ccbmessages";
    const std::string LOGIN_URL = BASE_URL+"login.php?do=login";
    const std::string CHECK_URL = BASE_URL+"member.php?78756-Dailox";

    std::string *cookiePath;
    CURL *curl;
    bool cleaned;
    bool *encryptCookie;
};

#endif // CONNECTION_H
