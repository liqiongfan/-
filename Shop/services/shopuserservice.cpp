#include "shopuserservice.h"

ShopUserService::ShopUserService()
{

}

QVariantMap ShopUserService::match(QString &name, QString &pwd)
{
    return this->where("user_name", "=", name)
        ->where("password", "=", QString::fromStdString(MD5(pwd.toStdString()).toStr()))
        ->first();
}
