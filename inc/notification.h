#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>

class Notification : public QWidget {
    Q_OBJECT

    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity)

    void setPopupOpacity(float opacity);
    float getPopupOpacity() const;


public:
    Notification(QWidget *parent, int *location, int *notificationDuration, bool *darkMode);
    ~Notification();

    const int TOP_LEFT = 1;
    const int BOTTOM_LEFT = 2;
    const int TOP_RIGHT = 3;
    const int BOTTOM_RIGHT = 4;

    QStringList getCurrentQueue();

protected:
    void paintEvent(QPaintEvent *event);

public slots:
    void setMessage(const QString &messageTitle, const QString &text);
    void show();

private slots:
    void hideAnimation();
    void hide();

private:
    QLabel label;
    QLabel title;
    QGridLayout layout;
    QPropertyAnimation animation;
    QTimer *timer;
    QStringList toShow;
    int *notificationDuration;
    int *location;
    bool *darkMode;
    float popupOpacity;
};


#endif // NOTIFICATION_H
