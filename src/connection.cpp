#include "connection.h"
#include <QMessageBox>
#include <iostream>
#include <array>
#include <string>

Connection::Connection(std::string *path)
{
    cookiePath = path;
    curl = curl_easy_init();
    cleaned = false;

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
}

char *Connection::str2md5(const char *str, int length) {
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, str, 512);
        } else {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }

    return out;
}

std::string encryptDecrypt(std::string toEncrypt) {

    char key[18] = {'D', 'E', 'I', 'N', 'E', 'M', 'U', 'D', 'D', 'A', '6', 'c', 'd', 'U', 'P', '9', 'D', '7'};

    std::string output = toEncrypt;

    for (int i = 0; i < (int)toEncrypt.size(); i++)
        output[i] = toEncrypt[i] ^ key[i % (sizeof(key) / sizeof(char))];

    return output;
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, std::string *s)
{
    s->append(static_cast<char *>(ptr), size*nmemb);
    return size*nmemb;
}


void Connection::login(const char *username, const char *password) {
    std::string usernameS(username);
    std::string passwordS(password);
    request(LOGIN_URL, usernameS, passwordS);
    std::fill_n(&passwordS[0], passwordS.capacity()-1, 0xff);
}

void Connection::logOut(std::string &logoutHash) {
    std::string temp = LOGOUT_URL+logoutHash;
    request(temp);
}

std::string Connection::getMainSite() {
    return request(BASE_URL);
}


std::string Connection::getPosts() {

    return request(POST_URL);
}

bool Connection::checkCookie(bool writeToFile){
    struct curl_slist *cookies;
    curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
    bool exists = false;
    bool found = false;


    QString path = QString::fromStdString(*cookiePath);
    if(QFile::exists(path)) {
        exists = true;
    }

    if(cookies != NULL) {
        while(cookies) {
            if(writeToFile) {
                if(!exists) {
                    QFile file(path);
                    if( file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                        QTextStream stream(&file);
                        std::string final = encryptDecrypt(cookies->data);
                        stream << final.c_str() << "\n";
                        file.close();
                    }
                }
            }
            if(strstr(cookies->data, "bb_sessionhash") != NULL) {
                  found = true;
            }
            cookies = cookies->next;
        }
    }
    curl_slist_free_all(cookies);

    return found;
}

void Connection::readFromFileAndSetCookie(std::string &file) {
    QString toQS = QString::fromStdString(file);
    QString text;
    if(QFile::exists(toQS)) {
        QFile f(toQS);
        if(f.open(QIODevice::ReadOnly)) {
            QTextStream instream(&f);
            text = instream.readAll();
        }
    }
    QStringList temp = text.split("\n");

    foreach(auto &s, temp) {
        std::string toStd = encryptDecrypt(s.toUtf8().constData());
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, toStd.c_str());
    }
}


std::string Connection::getPMs() {
    return request(PM_URL);
}

std::string Connection::getChatBox() {
    return request(CHATBOX_URL);
}


bool Connection::isLoggedIn() {

    std::string output = request(CHECK_URL);
    return output.find("Du bist nicht angemeldet oder du hast keine Rechte diese Seite zu betreten. Dies könnte einer der Gründe sein") == std::string::npos;
}

void Connection::cleanup(bool noCleanup) {
    if(checkCookie()) {
        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
        curl_easy_perform(curl);
    }
    if(!noCleanup) {
        curl_easy_cleanup(curl);
    }

}

Connection::~Connection() {
    cleanup();
}
std::string Connection::request(std::string url, std::string username, std::string password) {
    std::string output;
    std::string params;
    int post = 0;

    if(username != "" && password != "") {
        params = "vb_login_username="+username+"&vb_login_password=&s=&securitytoken=guest&do=login&vb_login_md5password="+password+"&vb_login_md5_password_utf="+password;
        post = 1;
    }

    if(*cookiePath != "") {
        params += "&cookieuser=1";
        if(!checkCookie()) {
            readFromFileAndSetCookie(*cookiePath);
        }
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, post);


    curl_easy_perform(curl);



    if(*cookiePath != "" && username != "" && password != "") {
        checkCookie(true);
    }

    return output;
}
