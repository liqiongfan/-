#ifndef TABLEFORM_H
#define TABLEFORM_H

#include <QKeyEvent>
#include <QWidget>
#include <QAbstractItemView>
#include <QTableWidget>
#include <QHeaderView>

namespace Ui {
class TableForm;
}

class TableForm : public QWidget
{
    Q_OBJECT

public:
    enum class Row_Direction { UP = 1, DOWN = 2 };
    explicit TableForm(QWidget *parent = nullptr);
    ~TableForm();

    // Rendering the Table Lists data
    void renderTableHeaders(QVariant headers);
    void renderTableLists(QVariantList data);
    void appendRow(QVariantMap &val);
    void removeRow(int row);
    void setTableFields(QVariant fields);

    // Focus or not
    bool hasFocus();
    void setFocus();
    void selectRow(int row);

    // Obtain the current selected item
    QList<QTableWidgetItem *> selectedItems();
    QTableWidgetItem *item(int row, int column);
    void editItem(QTableWidgetItem *item);
    void closePersistentEditor(QTableWidgetItem *item);
    void openPersistentEditor(QTableWidgetItem *item);
    void updateItemValue(int row, int column, QString val);
    int currentSelectRow();
    int currentRow();
    void clearTable();

    // Move the cursor
    void selectTableRow(Row_Direction direction);

Q_SIGNALS:
    void moveCursor(Row_Direction direction);

private:
    QStringList     fieldList;
    Ui::TableForm         *ui;
    int                   row;
};


#endif // TABLEFORM_H
