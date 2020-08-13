#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 设置标题
    this->setWindowTitle(QString("%1 - 零售收银系统").arg("赚赚"));

    // 登录对话框
    loginDialog = new LoginDialog();
    loginDialog->show();

    // 绑定事件和事件槽
    bindEvent();

    // 渲染表头
    renderHeader();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::bindEvent()
{
    // 登录窗口和主窗口的信号关联
    QObject::connect(loginDialog, &LoginDialog::hideWindow, this, &MainWindow::showWindow);
    this->installEventFilter(this);
}

// 展示窗口
void MainWindow::showWindow()
{
    // 显式主窗口
    this->showMaximized();
    this->show();
}

// 渲染表头
void MainWindow::renderHeader()
{
    ui->tableForm->renderTableHeaders(QStringList({"条码", "编码", "名称", "数量", "单价", "总价"}));
    ui->tableForm->setTableFields(QStringList({"bar_code", "product_code", "name", "quantity", "price", "total_price"}));

    ui->tableForm->renderTableLists(QVariantList({
         QVariantMap({
             { "bar_code", "2221123" },
             { "product_code", "283832" },
             { "name", "hello" },
             { "quantity", "10"},
             { "price", "9.9" },
             { "total_price", "99.0"}
         }),
         QVariantMap({
             { "bar_code", "87475829" },
             { "product_code", "283832" },
             { "name", "world" },
             { "quantity", "11"},
             { "price", "9.9" },
             { "total_price", "99.9"}
         })
     }));

    QVariantMap row = QVariantMap({
          { "bar_code", "87475829" },
          { "product_code", "283832" },
          { "name", "world" },
          { "quantity", "10"},
          { "price", "8.88" },
          { "total_price", "88.80"}
      });
    ui->tableForm->appendRow(row);
}

// 处理事件
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if ( event->type() == QEvent::KeyRelease )
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if ( keyEvent->key() == Qt::Key_Up ) {
            emit ui->tableForm->moveCursor(TableForm::Row_Direction::UP);
        } else if ( Qt::Key_Down == keyEvent->key() ) {
            emit ui->tableForm->moveCursor(TableForm::Row_Direction::DOWN);
        }
        return true;
    }
    return QWidget::eventFilter(target, event);
}

// 渲染支付方式
// 渲染流程：从DB获取支付方式，然后渲染：支付方式: 金额
void MainWindow::renderPayments()
{



}

