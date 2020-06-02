#ifndef OMC8000ITEM_H
#define OMC8000ITEM_H

#include <QList>
#include <QVariant>
#include <QString>
#include "OMC8000Lib/OMC8000Lib.h"

using namespace Utils::OMC8000;

enum PlcViewTreeColumn
{
    PlcViewTreeColumn_Id = 0,
    PlcViewTreeColumn_Type,
    PlcViewTreeColumn_Read,
    PlcViewTreeColumn_Write,
    PlcViewTreeColumn_Counter,
    PlcViewTreeColumn_ReadTimeStamp,
    PlcViewTreeColumn_WriteTimeStamp,
    PlcViewTreeColumn_Description,
    PlcViewTreeColumn_Exception,
    /* size */
    PlcViewTreeColumnSize
};

const char* const PlcViewTreeColumnStr[PlcViewTreeColumnSize] =
{
    "Task/Area/Node Id",
    "Type",
    "Read",
    "Write",
    "Counter",
    "ReadTime",
    "WriteTime",
    "Description",
    "Exception"
};

enum PlcViewItemStateColors
{
    PlcViewItemStateColor_Black = 0,
    PlcViewItemStateColor_Blue,
    PlcViewItemStateColor_DarkGreen,
    PlcViewItemStateColor_Gold,
    PlcViewItemStateColor_Gray,
    PlcViewItemStateColor_LightGreen,
    PlcViewItemStateColor_Orange,
    PlcViewItemStateColor_Pink,
    PlcViewItemStateColor_Red,
    PlcViewItemStateColor_Violet
};

class PlcViewItem
{
public:
    PlcViewItem(OMC8000NodeInterface *data = 0, PlcViewItem *parent = 0);
    ~PlcViewItem();

    void appendChild(PlcViewItem *child);

    PlcViewItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    PlcViewItem *parent();

    //editable functions
    bool insertChildren(int position, int count, int columns);
    bool removeChildren(int position, int count);
    int childNumber() const;
    bool setData(int column, const QVariant &value);
    bool setData(const QVariant &value);

    QVariant icon() const;
    QVariant iconState() const;
    void setcolorOn(PlcViewItemStateColors stateColorOn) { this->stateColorOn = stateColorOn;}
    void setcolorOff(PlcViewItemStateColors stateColorOff) { this->stateColorOff = stateColorOff;}

    OMC8000NodeInterface* node() const { return omc8000Node; }

 private:
    QString GetNodeValue() const;
    QString GetNodeWriteValue() const;
    void SetNodeValue(UINT32 val);

 private:
     QList<PlcViewItem*> childItems; //child nodes
     QList<QVariant> itemData;      //column
     PlcViewItem *parentItem;        //paren
     PlcViewItemStateColors stateColorOn;
     PlcViewItemStateColors stateColorOff;

     OMC8000Lib *omc8000; //root node
     OMC8000NodeInterface *omc8000Node; //child param
};

#endif // OMC8000ITEM_H
