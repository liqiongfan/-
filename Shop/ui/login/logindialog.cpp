#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("登录");
}

LoginDialog::~LoginDialog()
{
    delete ui;
}


void LoginDialog::on_pushButton_clicked()
{
    QString userName = ui->userName->text();
    QString password = ui->password->text();
    if (userName.isEmpty() || password.isEmpty() ) {
        QMessageBox::critical(this, "警告", "请先输入有效账号或者密码! ");
        return ;
    }
    ShopUserService shopUserService;
    QVariantMap data = shopUserService.match(userName, password);
    if (data.isEmpty()) {
        QMessageBox::critical(this, "警告", "您的账号或者密码错误! ");
        return ;
    }

    // 登录成功以后，处理额外的业务逻辑
    QMessageBox::information(this, "通知", "登录成功");

    // 隐藏
    this->hide();
    emit hideWindow();
}
