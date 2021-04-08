#include "htmlparser.h"
#include <QTextDocument>

HtmlParser::HtmlParser()
{
}

void HtmlParser::parseLogout(const QString &html, QString &output) {
    QRegularExpression re("logouthash=.*?(?=\\s)");
    QRegularExpressionMatch match = re.match(html);
    output = match.captured(0).replace("\"", "");
}

void HtmlParser::parsePosts(const QString &html, QMultiMap<int, QString> &output) {

    if(!output.isEmpty()) {
        output.clear();
    }
    QStringList all = html.split(">");
    QStringList time;
    QStringList links;
    int curObject = 0;
    QRegularExpression rxForTitles("(?<=title=\")[^\"]+");
    for(int i=0; i < all.length(); i++) {
        QString curLine = all.at(i);
        if(curLine == "") {
            continue;
        }
        if(curLine.contains("</tr")) {
            curObject += 1;
        }
        if(curLine.contains("Datum/Zeit")) {
            if(all.at(i+1).contains("class")) {
                output.insert(curObject, all.at(i+2).split("<").at(0));
            } else {
                output.insert(curObject, all.at(i+1).split("<").at(0));
            }
        }
        if(curLine.contains("https://free-hack.com")) {
            QRegularExpressionMatch match = rxForTitles.match(all.at(i-1));
            output.insert(curObject, match.captured(0));
        }

        if(curLine.contains("member.php?")) {
            QRegularExpressionMatch match = rxForTitles.match(curLine);
            output.insert(curObject, match.captured(0));
        }

        if(curLine.contains("forumdisplay.php?")) {
            output.insert(curObject, all.at(i+1).split("<").at(0));
        }
    }
}


void HtmlParser::parsePMs(const QString &html, QString &output) {
    QRegularExpression rxPMLinks("pmid=[0-9]*");
    QRegularExpressionMatch match = rxPMLinks.match(html);
    output = match.captured(0);
}


void HtmlParser::parseChatBox(const QString &html, QMultiMap<int, QString> &output) {


    if(!output.isEmpty()) {
        output.clear();
    }

    QStringList all = html.split(">");

    for(int i=0; i < all.length(); i++) {
        all.replace(i, all.at(i)+">");
    }

    int curObject = 0;
    for(int i=0; i < all.length(); i++) {
        QString curLine = all.at(i);

        if(curLine.contains("</tr")) {
            curObject++;
        }


        if(curLine.contains("member.php")) {
            QString temp = all.at(i+2);
            QString user;
            QString time;
            if(temp.contains("<b")) {
                user = all.at(i+3);
            } else {
                user = temp;
            }

            time = all.at(i-1);
            time = time.replace("</span>", "");
            time = time.replace(0, 1, "");
            user = user.replace("</font>", "");
            user = user.replace("</b>", "");
            output.insert(curObject, user);
            output.insert(curObject, time);
        }


        if(curLine.contains("vertical-align:bottom;")) {
            QString temp = all.at(i+2);
            QString message;
            if(temp.contains("<img src=")) {
                int counter = 1;
                bool fontFound = false;
                while(!fontFound) {
                    QString curStr = all.at(i+counter);
                    if(curStr.contains("</font")) {
                        fontFound = true;
                    } else {
                        message += curStr;
                    }
                    counter++;
                }
            }

            message += temp;
            QStringList finalList;

            QString finalMsg;
            if(message.contains("font")) {
                finalList = message.split(">");

                if(finalList.length() > 1) {
                    for(int i=1; i < finalList.length(); i++) {
                        finalMsg += finalList.at(i);
                    }
                } else {
                    finalMsg += message.at(1);
                }
            }

            QString veryFinalMsg;
            if(finalMsg.isEmpty()) {
                finalMsg = message;
            }
            if(finalMsg.contains("<img") || finalMsg.contains("<a href=")) {
                QStringList tempList = finalMsg.split("<");
                for(int i=0; i < tempList.length(); i++) {
                    if(!tempList.at(i).contains("img src=") && !tempList.at(i).contains("a href=")) {
                        veryFinalMsg += tempList.at(i);
                    }
                    if(tempList.at(i).contains("a href=")) {
                        QString link = tempList.at(i).split("\"").at(1);
                        veryFinalMsg += link.replace("\"", "");
                    }
                }
            }


            if(veryFinalMsg.isEmpty()) {
                veryFinalMsg = message;
            }

            veryFinalMsg = veryFinalMsg.replace("</font>", "");

            output.insert(curObject, veryFinalMsg);
        }
    }
}

void HtmlParser::getDifferenceByValue(QMultiMap<int, QString> &hash, const QString &what, QStringList &output) {
    QMapIterator<int, QString> i(hash);
    while(i.hasNext()) {
        i.next();
        if(i.value() == what) {
            output = hash.values(i.key());
        }
    }
}


void HtmlParser::getValues(QMultiMap<int, QString> &hash, int what, QStringList &output) {
    QMapIterator<int, QString> i(hash);
    int count = 0;
    int currentKey = 0;
    while(i.hasNext()) {
        i.next();
        QString val = i.value();
        if(i.key() != currentKey) {
            count = 0;
            currentKey = i.key();
        }
        if(count == what) {
            if(val.contains('\r')) {
                val.replace('\r', "");
            }
            if(val.contains('\n')) {
                val.replace('\n', "");
            }
            if(val.contains('\t')) {
                val.replace('\t', "");
            }
            output.append(val);
        }
        count++;
    }
}
