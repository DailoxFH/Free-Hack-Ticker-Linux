#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "checker.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->setFixedSize(QSize(320, 251));
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    amILoggedIn = false;
    checkSetting("updateInterval", 10, ui->spinBoxUpdateInterval->maximum(), ui->spinBoxUpdateInterval->minimum());
    checkSetting("notificationDuration", 10, ui->spinBoxNotificationDuration->maximum());
    checkSetting("updateChatBox", 0);
    checkSetting("updatePrivateMessages", 0);
    checkSetting("darkMode", 0);
    checkSetting("rememberMe", 0);

    amILoggedIn = true;

    if(loadSettings.value("darkMode").toInt() == 1) {
        ui->checkBoxDarkMode->setChecked(true);
        on_checkBoxDarkMode_stateChanged(2);

    }

    if(loadSettings.value("updateChatBox").toInt() == 1) {
        ui->checkBoxChatBox->setChecked(true);
        updateChatBox = true;
    }

    if(loadSettings.value("updatePrivateMessages").toInt() == 1) {
        ui->checkBoxPM->setChecked(true);
        updatePMs = true;;

    }

    notifationDuration = loadSettings.value("notificationDuration").toInt();
    updateInterval = loadSettings.value("updateInterval").toInt();

    ui->spinBoxNotificationDuration->setValue(notifationDuration);
    ui->spinBoxUpdateInterval->setValue(updateInterval);

    QString path = loadSettings.fileName();
    QStringList splittedPath = path.split("/");
    splittedPath.removeLast();
    QString configPath = splittedPath.join("/");

    QString tempCookiePath;
    cookiePath = configPath+"/cookies.txt";
    if(loadSettings.value("rememberMe").toInt() == 1) {
        tempCookiePath = cookiePath;
        rememberMe = true;
        ui->checkBox->setChecked(true);
    } else {
        tempCookiePath = "";
        rememberMe = false;
        ui->checkBox->setChecked(false);
    }


    cook = tempCookiePath.toUtf8().constData();
    checker = new Checker(this, &cook, configPath);



    QTimer *timerCheckForUpdates = new QTimer(this);
    connect(timerCheckForUpdates, SIGNAL(timeout()), this, SLOT(doGlobalUpdate()));

    QTimer *timerCheckLoginByCookie = new QTimer(this);
    connect(timerCheckLoginByCookie, SIGNAL(timeout()), this, SLOT(checkLoginByCookie()));

    QTimer *timerCheckLoginByCheck = new QTimer(this);
    connect(timerCheckLoginByCheck, SIGNAL(timeout()), this, SLOT(checkLoginByCheck()));

    timerCheckForUpdates->start(notifationDuration*60000);
    timerCheckLoginByCookie->start(1500);
    timerCheckLoginByCheck->start(1*60000);


    createActions();
    createTrayIcon();
    setIcon();
    trayIcon->show();
    setWindowTitle(tr("Free-Hack Ticker"));
    connect(trayIcon, SIGNAL(activated()), this, SLOT(iconActivated()));

    checkLoginByCookie();


    QTimer::singleShot(8500, this, SLOT(doGlobalUpdate()));
}


void MainWindow::checkForNewCBMessages() {
    checker->checkForNewCBMessages();
}

void MainWindow::checkForNewPosts() {
    checker->checkForNewPosts();
}


void MainWindow::doGlobalUpdate() {
    checker->doGlobalUpdate();
}

void MainWindow::checkForNewPMs() {
    checker->checkForNewPMs();

}

void MainWindow::checkSetting(QString setting, int defaultValue, int max, int min) {
    bool error = false;
    if(loadSettings.contains(setting)) {
        bool okInt;
        int val = loadSettings.value(setting).toInt(&okInt);

        if(!okInt) {
            error = true;
        } else {
            if(max != 0 || min != 0) {
                if(val > max || val < min) {
                    error = true;
                }
            }
        }
        if(error) {
            loadSettings.setValue(setting,  defaultValue);
            QMessageBox::critical(this, "Invalide Einstellungen", "Invalide Einstellungen erkannt. Die betroffenen Einstellungen wurden zurückgesetzt. Bitte starte die Anwendung neu");
            exit(1);
        }
    } else {
        loadSettings.setValue(setting, defaultValue);
    }

}


MainWindow::~MainWindow()
{
    trayIcon->contextMenu()->clear();
    delete checker;
    delete trayIcon;
    delete minimizeAction;
    delete restoreAction;
    delete quitAction;
    delete trayIconMenu;
    delete ui;

}

void MainWindow::checkLoginByCookie() {
    checker->checkLoginByCookie();
}

void MainWindow::checkLoginByCheck() {
    checker->checkLoginByCheck();
}

void MainWindow::loggedIn(bool in) {
    if(in) {
        amILoggedIn = true;
        ui->lblStatus->setText("Eingeloggt");
        ui->lblStatus->setStyleSheet("color: green;");
        int index = ui->comboBox->findText("Login");
        ui->comboBox->setItemText(index, "Logout");
        ui->checkBox->setEnabled(false);
    } else {
        amILoggedIn = false;
        ui->lblStatus->setText("Nicht eingeloggt");
        ui->lblStatus->setStyleSheet("color: red;");
        int index = ui->comboBox->findText("Logout");
        ui->comboBox->setItemText(index, "Login");
        ui->checkBox->setEnabled(true);
    }
}



void MainWindow::on_comboBox_textActivated(const QString &arg1)
{
    if(arg1 == "Login")
    {
        QByteArray ba1 = ui->lineEditPassword->text().toUtf8();
        char *pw = ba1.data();
        QByteArray ba2 = ui->lineEditusername->text().toUtf8();
        char *username = ba2.data();
        checker->login(username, pw);
        ba1.remove(0, ba1.length());
        ba2.remove(0, ba2.length());
        ui->lineEditPassword->clear();
    }
    if(arg1 == "Logout") {
        checker->logout();
        on_checkBox_stateChanged(1);
    }

    if(arg1 == "Update/Fetch") {
        checker->doGlobalUpdate();
        QMessageBox::information(this, "Fertig", "Fertig mit updaten");
    }
}

void MainWindow::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}

void MainWindow::setIcon()
{
    QIcon icon(":/resources/icons/main-icon");

    trayIcon->setIcon(icon);
    setWindowIcon(icon);
}


void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            QWidget::showNormal();
            break;
        default:
            ;
    }
}

void MainWindow::showMessage(const QString title, const QString message)
{
    trayIcon->showMessage(title, message, trayIcon->icon(), notifationDuration*1000);
}

void MainWindow::createActions()
{

    minimizeAction = new QAction(tr("Minmieren"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    restoreAction = new QAction(tr("Anzeigen"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("Beenden"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);

    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
    //Hängt vom Theme ab bzw ist diese Einstellung vermutlich eh total überflüssig xD
    trayIconMenu->setPalette(darkPalette);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

}

void MainWindow::initDarkMode() {

    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::black);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
}

void MainWindow::on_checkBoxDarkMode_stateChanged(int arg1)
{
    if(arg1 == 2) {
        initDarkMode();
        this->setPalette(darkPalette);
        loadSettings.setValue("darkMode", 1);
    } else{
        this->setPalette(normalPalette);
        loadSettings.setValue("darkMode", 0);
    }
}

void MainWindow::on_spinBoxNotificationDuration_valueChanged(int arg1)
{
    notifationDuration = arg1;
    loadSettings.setValue("notificationDuration", arg1);
}

void MainWindow::on_spinBoxUpdateInterval_valueChanged(int arg1)
{
    updateInterval = arg1;
    loadSettings.setValue("updateInterval", arg1);
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(arg1 == 2) {
        rememberMe = true;
        loadSettings.setValue("rememberMe", 1);
        cook = cookiePath.toUtf8().constData();
    } else {
        if(!cookiePath.isEmpty() && QFile::exists(cookiePath)) {
            QFile::remove(cookiePath);
        }
        rememberMe = false;
        loadSettings.setValue("rememberMe", 0);
        cook = "";

    }
    ui->checkBox->setChecked(rememberMe);
}

void MainWindow::on_checkBoxChatBox_stateChanged(int arg1)
{
    if(arg1 == 2) {
        updateChatBox = true;
        loadSettings.setValue("updateChatBox", 1);
    } else {
        updateChatBox = false;
        loadSettings.setValue("updateChatBox", 0);
    }
}

void MainWindow::on_checkBoxPM_stateChanged(int arg1)
{
    if(arg1 == 2) {
        updatePMs = true;
        loadSettings.setValue("updatePrivateMessages", 1);
    } else {
        updatePMs = false;
        loadSettings.setValue("updatePrivateMessages", 0);
    }
}

void MainWindow::on_label_9_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_label_10_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}
