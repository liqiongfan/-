#include "tableform.h"
#include "ui_tableform.h"
#include <QtDebug>

TableForm::TableForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableForm), row(0)
{
    ui->setupUi(this);

    // Set global settings for TabWidget
    ui->_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->_table->setSelectionBehavior(QTableWidget::SelectRows);
    ui->_table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->_table->verticalHeader()->setVisible(false);

    this->installEventFilter(this);

    // Accept the signals for move the cursor around the table
    this->connect(this, &TableForm::moveCursor, [=](Row_Direction direction){

        this->row = ui->_table->selectedItems().at(0)->row();

        if ( direction == Row_Direction::UP ) {
            if ( row > 0 ) {
                selectRow(--row);
            }
        } else if ( direction == Row_Direction::DOWN ) {
            if ( row >= 0 && ( row < ui->_table->rowCount() ) ) {
                selectRow(++row);
            }
        }
    });
}

TableForm::~TableForm()
{
    delete ui;
}

// Redering the QTableHeaders
void TableForm::renderTableHeaders(QVariant headers)
{
    // Q_UNUSED(headers)
    if ( headers.type() == QVariant::List ) {
        // Each value need to be QVariant
        QList<QVariant> list = headers.toList();
        ui->_table->setColumnCount(list.count() + 1);
        ui->_table->setHorizontalHeaderItem(0, new QTableWidgetItem("#"));
        for (int i = 1; i < list.count() + 1; i++ ) {
            ui->_table->setHorizontalHeaderItem(i, new QTableWidgetItem(list.at(i).toString()));
        }
    } else if ( headers.type() == QVariant::StringList ) {
        QStringList stringList = headers.toStringList();
        ui->_table->setColumnCount(stringList.count() + 1);
        stringList.insert(0, "#");
        ui->_table->setHorizontalHeaderLabels(stringList);
    }
}

// Rendering the QTableWidget's body with the body data and fields
void TableForm::renderTableLists(QVariantList data)
{
    int listCount = data.count();
    // ui->_table->setRowCount(listCount);
    for (int i = 0; i < listCount; i++) {
        if ( data.at(i).type() == QVariant::Map ) {
            QVariantMap mapVal = data.at(i).toMap();
            this->appendRow(mapVal);
        }
    }
}

// Add an row into the Table Widget
void TableForm::appendRow(QVariantMap &val)
{
    if ( val.count() <= 0 ) {
        return ;
    }
    int rowCount = ui->_table->rowCount();
    ui->_table->insertRow(rowCount);

    QTableWidgetItem *item = new QTableWidgetItem(QString::number(rowCount+1));
    item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    ui->_table->setItem(rowCount, 0, item);

    for (int j = 0; j < this->fieldList.count(); j++) {

        QString value = val[this->fieldList.at(j)].toString();
        QTableWidgetItem *item = new QTableWidgetItem(value);
        item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
        ui->_table->setItem(rowCount, j + 1, item);
    }
}

void TableForm::removeRow(int row)
{
    // 如果行数大于实际行数，不做任何事情返回
    if ( ui->_table->rowCount() < row ) return ;
    ui->_table->removeRow(row);
}

void TableForm::setTableFields(QVariant fields)
{
    if ( fields.type() != QVariant::List ) {
        for ( int i = 0; i < fields.toList().count(); i++ ) {
            fieldList.insert(i, fields.toList().at(i).toString());
        }
    } else if ( fields.type() != QVariant::StringList ) {
        fieldList = fields.toStringList();
    } else { return ; }
}

// Obtain the QTableWidget has focus or not
bool TableForm::hasFocus()
{
    return ui->_table->hasFocus();
}

// Let the QTableWidget get the focus
void TableForm::setFocus()
{
    return ui->_table->setFocus();
}

// Select the Table row with the given row
void TableForm::selectRow(int row)
{
    return ui->_table->selectRow(row);
}

// Obtain the selected items from the QTableWidget
QList<QTableWidgetItem *> TableForm::selectedItems()
{
    return ui->_table->selectedItems();
}

// Obtain the QTableWidgetItem from the given row & column
QTableWidgetItem *TableForm::item(int row, int column)
{
    return ui->_table->item(row, column);
}

// Edit the Table Item
void TableForm::editItem(QTableWidgetItem *item)
{
    ui->_table->editItem(item);
}

// Close the editor
void TableForm::closePersistentEditor(QTableWidgetItem *item)
{
    ui->_table->closePersistentEditor(item);
}

// Open the editor
void TableForm::openPersistentEditor(QTableWidgetItem *item)
{
    ui->_table->openPersistentEditor(item);
}

// Update the Table item
void TableForm::updateItemValue(int row, int column, QString val)
{
    QTableWidgetItem *item = ui->_table->takeItem(row, column);
    item->setText(val);
    ui->_table->setItem(row, column, item);
}

int TableForm::currentSelectRow()
{
    if ( ui->_table->selectedItems().isEmpty() ) {
        return -1;
    }
    return ui->_table->selectedItems().at(0)->row();
}

// Obtain the current row
int TableForm::currentRow()
{
    return ui->_table->rowCount() - 1;
}

// Clears all items from Tables
void TableForm::clearTable()
{
    int row = ui->_table->rowCount();
    for (int i = row - 1; i >= 0; i--) {
        ui->_table->removeRow(i);
    }
}

// Move the cursor with the given direction
void TableForm::selectTableRow(TableForm::Row_Direction direction)
{
    if ( direction == Row_Direction::UP ) {
        if ( row > 0 ) {
            selectRow(--row);

        }
    } else if ( direction == Row_Direction::DOWN ) {
        if ( row >= 0 && ( row < ui->_table->rowCount() ) ) {
            selectRow(++row);
        }
    }
}


















