#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QTextDocument>
#include <QDesktopServices>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Checker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QSettings loadSettings;
    std::string cook;
    bool amILoggedIn;
    bool updatePMs;
    bool updateChatBox;
    bool rememberMe;
    bool encryptCookie;

    void setVisible(bool visible) override;
    void showMessage(const QString title, const QString message, QSystemTrayIcon::MessageIcon icon=QSystemTrayIcon::Information);
    void loggedIn(bool in);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void setIcon();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void checkLoginByCookie();
    void checkLoginByCheck();
    void doGlobalUpdate();

    void on_checkBoxDarkMode_stateChanged(int arg1);

    void on_spinBoxNotificationDuration_valueChanged(int arg1);

    void on_spinBoxUpdateInterval_valueChanged(int arg1);

    void on_checkBox_stateChanged(int arg1);

    void on_checkBoxChatBox_stateChanged(int arg1);

    void on_checkBoxPM_stateChanged(int arg1);

    void on_label_9_linkActivated(const QString &link);

    void on_label_10_linkActivated(const QString &link);

    void on_comboBox_activated();

    void on_checkBox_2_stateChanged(int arg1);

    void on_btnResetSettings_clicked();

    void on_checkBoxMinimize_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    Checker *checker;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QPalette darkPalette;
    QPalette normalPalette;
    QString cookiePath;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    int updateInterval;
    int notifationDuration;
    bool darkMode;
    bool sawDisclaimer;

    void checkSetting(QString setting, int defaultValue, int max=0, int min=0);
    void initDarkMode();
    void checkForNewPMs();
    void checkForNewCBMessages();
    void checkForNewPosts();
    void createActions();
    void createTrayIcon();

};
#endif // MAINWINDOW_H
