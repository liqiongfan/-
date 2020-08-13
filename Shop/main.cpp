#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    db::sqlite::database db("shop.sqlite");
    db.executeSql("CREATE DATABASE shop; USE shop;");
    db.executeSql(R"(
          CREATE TABLE shop_user (
            id integer primary key autoincrement,
            user_name text not null,        -- 用户名
            password text not null,         -- 密码
            token text not null,            -- 令牌
            status integer not null         -- 状态
          )
    )");

/*
    db.executeSql(R"(
      INSERT INTO shop_user
      (user_name, password, token, status )
      VALUES
      ("shop", ?, "", 1)
    )", { MD5("shop").toStr().c_str() });
*/


    db.executeSql(R"(
        CREATE TABLE shop_order (
              id integer primary key autoincrement,
              user_id integer not null,         -- 用户ID
              machine_no text not null,         -- 机器编号
              order_id text not null,           -- 订单ID
              category_count integer not null,  -- 类别数目
              quantity integer not null,        -- 总数量
              total_price text not null,        -- 总金额
              cut_price text not null,          -- 抹零金额
              preference_price text not null,   -- 优惠金额
              retail_price text not null,       -- 零售实付金额
              date text not null,               -- 日期
              status integer not null,          -- 订单状态 0: 等待结账  1：结账成功  2：结账失败
              msg text not null default '',     -- 订单的操作结果原因
              create_time text not null,        -- 创建日期
              update_time text not null         -- 更新日期
        );
    )");

    db.executeSql(R"(
        CREATE TABLE shop_payments (
              id integer primary key autoincrement,
              name text not null,           -- 支付方式
              status integer not null,      -- 可用状态 0:可用 1:不可用
              create_time text not null,    -- 创建日期
              update_time text not null     -- 更新日期
        );
    )");

    db.executeSql(R"(
        CREATE TABLE shop_order_pay (
              id integer primary key autoincrement,
              order_id text not null,           -- 订单ID
              pay_id   text not null,           -- 每一笔支付对应的单号
              payment_id  integer not null,     -- 支付方式ID
              price text not null,              -- 支付金额
              status integer not null,          -- 支付状态 0: 正在支付中 1: 支付成功  2:支付失败
              create_time text not null,        -- 创建日期
              update_time text not null         -- 更新日期
        );
    )");


    db.executeSql(R"(
        CREATE TABLE shop_order_goods (
              id integer primary key autoincrement,
              order_id text not null,           -- 订单ID
              category_id  integer not null,    -- 订单明细里面的分类
              line_no integer not null,         -- 行号
              name text not null,               -- 商品名称
              quantity integer not null,        -- 商品数量
              price text not null,              -- 商品单价
              avg_price text not null,          -- 平均单价
              discount text not null default '100',   -- 商品折扣值
              status integer not null,          -- 商品状态 0: 正在结账操作 1：结账完成  2： 结账失败
              create_time text not null,        -- 创建日期
              update_time text not null         -- 更新日期
        );
    )");

    return a.exec();
}
