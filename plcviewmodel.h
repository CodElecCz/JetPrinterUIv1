#ifndef PLCVIEWMODEL_H
#define PLCVIEWMODEL_H

#include <QAbstractItemModel>
#include <QtXml/QDomDocument>
#include <QTimer>
#include "plcviewitem.h"
#include "OMC8000Lib/OMC8000Lib.h"

using namespace Utilities::OMC8000;

class PlcViewModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    PlcViewModel(OMC8000Lib* omc8000,const QString xmlFile, QObject *parent = 0);
    ~PlcViewModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    //editable
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                      int role = Qt::EditRole);
    bool insertRows(int position, int rows,
                 const QModelIndex &parent = QModelIndex());
    bool removeRows(int position, int rows,
                 const QModelIndex &parent = QModelIndex());

    void loadModelData(OMC8000Lib* omc8000lib, const QString xmlFile);

private slots:
    void update(QString itemName);
    void update();

private:
    void setupModelData(OMC8000Lib* omc8000lib, const QString xmlFile, PlcViewItem *parent);
    void setupDefaultNode(PlcViewItem* parentItem, INode* parentNode);
    void setupNode(QDomElement parentElem, PlcViewItem* parentItem, INode* parentNode);
    PlcViewItem *getItem(const QModelIndex &index) const;
    void OnValueChanged(INode* node, UINT32 val);
    void updateNode(QString itemName, QModelIndex parentIndex);

    PlcViewItem *rootItem;
};

#endif // PLCVIEWMODEL_H
