#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "db/db.h"
#include "crypto/MD5/md5.h"

#include "ui/login/logindialog.h"
#include "ui/tableform/tableform.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    LoginDialog *loginDialog;

    // Bind signals & slots
    void bindEvent();

    // Show this main window
    void showWindow();

    // Render the list
    void renderHeader();

    bool eventFilter(QObject *obj, QEvent *ev) override;

    void renderPayments();
};
#endif // MAINWINDOW_H
