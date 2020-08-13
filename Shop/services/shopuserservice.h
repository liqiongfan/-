#ifndef SHOPUSERSERVICE_H
#define SHOPUSERSERVICE_H

#include "db/db.h"
#include "crypto/MD5/md5.h"

class ShopUserService : public db::orm
{
public:
    ShopUserService();

    QVariantMap match(QString &name, QString &pwd);

    // orm interface
protected:
    QString getTableName() {
        return "shop_user";
    }
};

#endif // SHOPUSERSERVICE_H
