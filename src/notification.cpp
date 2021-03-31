#include "notification.h"

#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QMessageBox>

Notification::Notification(QWidget *parent, int *location, int *notificationDuration, bool *darkMode) : QWidget(parent) {

    this->location = location;
    this->notificationDuration = notificationDuration;
    this->darkMode = darkMode;

    setWindowFlags(Qt::FramelessWindowHint |
                   Qt::Tool |
                   Qt::WindowStaysOnTopHint);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    animation.setTargetObject(this);
    animation.setPropertyName("popupOpacity");
    connect(&animation, &QAbstractAnimation::finished, this, &Notification::hide);

    title.setAlignment(Qt::AlignTop | Qt::AlignCenter);
    title.setStyleSheet("QLabel { color: #b85656;}");
    layout.addWidget(&title);

    label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    label.setStyleSheet("QLabel { border:0px; margin: 0 0 0 0}");
    title.setStyleSheet("QLabel { font-weight:500; color : #9a0e0e;}");
    layout.addWidget(&label);
    setLayout(&layout);


    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &Notification::hideAnimation);
}

void Notification::paintEvent(QPaintEvent *event) {

    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect roundedRect;

    roundedRect.setX(rect().x());
    roundedRect.setY(rect().y());
    roundedRect.setWidth(rect().width());
    roundedRect.setHeight(rect().height());

    if(*darkMode) {
        painter.setBrush(QBrush(QColor(18, 18, 18, 200)));
        label.setStyleSheet("QLabel { color : white;}");
    } else {
        painter.setBrush(QBrush(QColor(255, 255, 255, 200)));
        label.setStyleSheet("QLabel { color : black;}");
    }


    painter.setPen(Qt::NoPen);

    painter.drawRoundedRect(roundedRect, 10, 10);
}

void Notification::setMessage(const QString &messageTitle, const QString &text) {
    if(toShow.isEmpty()){
        toShow.append(messageTitle+':'+text);
        title.setText(messageTitle);
        label.setText(text);
        adjustSize();
    } else {
        toShow.append(messageTitle+':'+text);
    }
}


void Notification::show() {
    setWindowOpacity(0.0);

    animation.setDuration(150);
    animation.setStartValue(0.0);
    animation.setEndValue(1.0);

    int tempLocation = *location;

    if(tempLocation == BOTTOM_RIGHT) {
        setGeometry(QApplication::desktop()->availableGeometry().width() - width() + QApplication::desktop() -> availableGeometry().x(),
                    QApplication::desktop()->availableGeometry().height() - height() + QApplication::desktop() -> availableGeometry().y(),
                    width(),
                    height());
    } else if(tempLocation == BOTTOM_LEFT) {
        setGeometry(QApplication::desktop() -> availableGeometry().x(),
                    QApplication::desktop()->availableGeometry().height() - height() + QApplication::desktop() -> availableGeometry().y(),
                    width(),
                    height());
    } else if(tempLocation == TOP_RIGHT) {
        setGeometry(QApplication::desktop()->availableGeometry().x() + QApplication::desktop() -> availableGeometry().width(),
                    QApplication::desktop()->availableGeometry().y(),
                    width(),
                    height());
    } else if(tempLocation == TOP_LEFT) {
        setGeometry(QApplication::desktop()->availableGeometry().x(),
                    QApplication::desktop()->availableGeometry().y(),
                    width(),
                    height());
    }

    QWidget::show();

    animation.start();
    timer->start(*notificationDuration*1000);
}


void Notification::hideAnimation() {
    timer->stop();
    animation.setDuration(1000);
    animation.setStartValue(1.0);
    animation.setEndValue(0.0);
    animation.start();
}

void Notification::hide() {
    if(getPopupOpacity() == 0.0){
        QWidget::hide();
        if(!toShow.isEmpty()) {
            if(toShow.length() >= 1) {
                toShow.removeAt(0);
            }

            if(!toShow.isEmpty()) {
                QStringList toUse = toShow.at(0).split(':');
                QString text = toUse.at(1);
                QString titleText = toUse.at(0);
                title.setText(titleText);
                label.setText(text);
                show();
            }
        }
    }
}

void Notification::setPopupOpacity(float opacity)
{
    popupOpacity = opacity;

    setWindowOpacity(opacity);
}

float Notification::getPopupOpacity() const
{
    return popupOpacity;
}

QStringList Notification::getCurrentQueue() {
    return toShow;
}

Notification::~Notification() {}
