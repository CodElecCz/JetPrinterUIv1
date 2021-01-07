#include <QIcon>
#include "plcviewitem.h"

template <class T> class VPtr
{
public:
    static T* asPtr(QVariant v)
    {
    return  (T *) v.value<void *>();
    }

    static QVariant asQVariant(T* ptr)
    {
    return qVariantFromValue((void *) ptr);
    }
};

PlcViewItem::PlcViewItem(INode *node, PlcViewItem *parent)
    : stateColorOn(PlcViewItemStateColor_LightGreen), stateColorOff(PlcViewItemStateColor_Gray)
{
   parentItem = parent;

   if(parent==0)
   {
       //set header
       for(int i = 0; i<PlcViewTreeColumnSize; i++)
       {
           itemData << PlcViewTreeColumnStr[i];
       }
   }
   else
   {
       //set no data
       for(int i = 0; i<PlcViewTreeColumnSize; i++)
       {
           itemData << QString("No Data");
       }
   }

   treeNode = node;
}

QString PlcViewItem::GetNodeValue(void) const
{
    QString s("");

    switch(treeNode->Type())
    {
    case NodeType_Bit:
        {
        bool bval;
        treeNode->Read(&bval);
        s.sprintf("%01X", bval);
        }
        break;
    case NodeType_Byte:
        {
        UINT8 bval;
        treeNode->Read(&bval);
        s.sprintf("%02X", bval);
        }
        break;
    case NodeType_Word:
        {
        UINT16 bval;
        treeNode->Read(&bval);
        s.sprintf("%04X", bval);
        }
        break;
    case NodeType_DWord:
        {
        UINT32 bval;
        treeNode->Read(&bval);
        s.sprintf("%08X", bval);
        }
        break;
    case NodeType_Real32:
        {
        float bval;
        treeNode->Read(&bval);
        s.sprintf("%0.02f", bval);
        }
        break;
    default:
        break;
    }
    return s;
}

QString PlcViewItem::GetNodeWriteValue(void) const
{
    QString s("");

    switch(treeNode->Type())
    {
    case NodeType_Bit:
        {
        bool bval;
        treeNode->Write(&bval);
        s.sprintf("%01X", bval);
        }
        break;
    case NodeType_Byte:
        {
        UINT8 bval;
        treeNode->Write(&bval);
        s.sprintf("%02X", bval);
        }
        break;
    case NodeType_Word:
        {
        UINT16 bval;
        treeNode->Write(&bval);
        s.sprintf("%04X", bval);
        }
        break;
    case NodeType_DWord:
        {
        UINT32 bval;
        treeNode->Write(&bval);
        s.sprintf("%08X", bval);
        }
        break;
    case NodeType_Real32:
        {
        float bval;
        treeNode->Write(&bval);
        s.sprintf("%0.02f", bval);
        }
        break;
    default:
        break;
    }
    return s;
}

void PlcViewItem::SetNodeValue(UINT32 val)
{
    UINT32_UT Val;
    Val.dword = val;

    switch(treeNode->Type())
    {
    case NodeType_Bit:
        {
        bool v = Val.bit.b0;
        treeNode->Write(v);
        }
        break;
    case NodeType_Byte:
        {
        UINT8 b = Val.byte.byte0;
        treeNode->Write(b);
        }
        break;
    case NodeType_Word:
        {
        UINT16 w = Val.word.word0;
        treeNode->Write(w);
        }
        break;
    case NodeType_DWord:
        {
        UINT32 w = Val.dword;
        treeNode->Write(w);
        }
        break;
    case NodeType_Real32:
        {
        float w = Val.real32;
        treeNode->Write(w);
        }
        break;
    default:
        break;
    }
}

PlcViewItem::~PlcViewItem()
{
    qDeleteAll(childItems);
}

void PlcViewItem::appendChild(PlcViewItem *child)
{
    childItems.append(child);
}

PlcViewItem *PlcViewItem::child(int row)
{
    return childItems.value(row);
}

int PlcViewItem::childCount() const
{
    return childItems.count();
}

int PlcViewItem::columnCount() const
{
    return itemData.count();
}

QVariant PlcViewItem::data(int column) const
{
    QString svar("");
    SYSTEMTIME t;
    INodeException* e = NULL;

    if(treeNode!=NULL)
    {
        switch(column)
        {
        case PlcViewTreeColumn_Id:
            return QString(treeNode->Id());
        case PlcViewTreeColumn_Type:
            return QString(treeNode->TypeStr());
        case PlcViewTreeColumn_Read:
            if(treeNode->Access()&NodeAccess_Read)
            {
                switch(treeNode->Type())
                {
                case NodeType_Bit:
                    break;
                default:
                    return GetNodeValue();
                }
            }
            break;
        case PlcViewTreeColumn_Write:
            if(treeNode->Access()&NodeAccess_Write)
            {
                return GetNodeWriteValue();
            }
            break;
        case PlcViewTreeColumn_Counter:
            switch(treeNode->Type())
            {
            case NodeType_Bit:
            case NodeType_Byte:
            case NodeType_Word:
            case NodeType_DWord:
            case NodeType_Real32:
                svar.sprintf("%d", treeNode->ReadCounter());
                return svar;
            }
            break;
        case PlcViewTreeColumn_ReadTimeStamp:
            if(treeNode->Access()&NodeAccess_Read)
            {
                switch(treeNode->Type())
                {
                case NodeType_Task:
                    t = treeNode->ReadTimeStamp();
                    svar.sprintf("%02d:%02d:%02d.%03d", (int)t.wHour, (int)t.wMinute, (int)t.wSecond, (int)t.wMilliseconds);
                    return svar;
                }
            }
            break;
        case PlcViewTreeColumn_WriteTimeStamp:
            if(treeNode->Access()&NodeAccess_Write)
            {
                switch(treeNode->Type())
                {
                case NodeType_Bit:
                case NodeType_Byte:
                case NodeType_Word:
                case NodeType_DWord:
                case NodeType_Real32:
                    t = treeNode->WriteTimeStamp();
                    svar.sprintf("%02d:%02d:%02d.%03d", (int)t.wHour, (int)t.wMinute, (int)t.wSecond, (int)t.wMilliseconds);
                    return svar;
                }
            }
            break;
        case PlcViewTreeColumn_Description:
            return QString(treeNode->Description());
            break;
        case PlcViewTreeColumn_Exception:
            e = treeNode->GetLastException();
            if(e!=nullptr)
            {
                QString svar;
                svar.sprintf("[%s] %s; function: %s; file: %s; line: %d\n", e->id(), e->what(), e->fnc(), e->file(), e->line());
                return svar;
            }
            break;
        default:
            break;
        }
    }
    else
        return itemData.value(column);

    return svar;
}

int PlcViewItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<PlcViewItem*>(this));

    return 0;
}

PlcViewItem *PlcViewItem::parent()
{
    return parentItem;
}

bool PlcViewItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
             return false;

    for (int row = 0; row < count; ++row)
    {
         PlcViewItem *item = new PlcViewItem(NULL, this);
         childItems.insert(position, item);
    }

    return true;
}

bool PlcViewItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
    {
        const char* id = childItems.at(position)->treeNode->Id();
        delete childItems.takeAt(position);
        treeNode->RemoveNode(id);
    }

    return true;
}

int PlcViewItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<PlcViewItem*>(this));

    return 0;
}

bool PlcViewItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
             return false;

    itemData[column] = value;

    if(treeNode!=NULL)
    {
        switch(column)
        {
        case PlcViewTreeColumn_Write:
            bool ok;
            UINT32 v = value.toUInt(&ok);
            if(ok)
            {
                SetNodeValue(v);
            }
            break;
        }
    }
    return true;
}

bool PlcViewItem::setData(const QVariant &value)
{
    treeNode = VPtr<INode>::asPtr(value);
    return true;
}

QVariant PlcViewItem::icon() const
{

    if(treeNode!=NULL)
    {
        switch(treeNode->Type())
        {
        case NodeType_Bit:
            return QIcon(":/images/Letter-B-black.ico");
        case NodeType_Byte:
            return QIcon(":/images/Letter-B-black.ico");
        case NodeType_Word:
            return QIcon(":/images/Letter-W-black.ico");
        case NodeType_DWord:
        case NodeType_Real32:
            return QIcon(":/images/Letter-D-black.ico");
        case NodeType_UInt64:
        case NodeType_Real64:
            return QIcon(":/images/Letter-U-black.ico");
        case NodeType_Task:
            return QIcon(":/images/task.ico");
        case NodeType_WordArray:
        case NodeType_BitArray:
        case NodeType_DWordArray:
            //return QIcon(":/images/Letter-A-black.ico");
            return QIcon(":/images/list.png");
        default:
            ;
        }
    }

    return QVariant();
}

QVariant PlcViewItem::iconState() const
{
    if(treeNode!=NULL)
    {
        switch(treeNode->Type())
        {
        case NodeType_Bit:
            bool bval;
            treeNode->Read(&bval);
            if(!bval)
            {
                switch(stateColorOff)
                {
                case PlcViewItemStateColor_Black:
                    return QIcon(":/images/Letters/O/black.ico");
                case PlcViewItemStateColor_Blue:
                    return QIcon(":/images/Letters/O/blue.ico");
                case PlcViewItemStateColor_DarkGreen:
                    return QIcon(":/images/Letters/O/dg.ico");
                case PlcViewItemStateColor_Gold:
                    return QIcon(":/images/Letters/O/gold.ico");
                case PlcViewItemStateColor_Gray:
                    return QIcon(":/images/Letters/O/grey.ico");
                case PlcViewItemStateColor_LightGreen:
                    return QIcon(":/images/Letters/O/lg.ico");
                case PlcViewItemStateColor_Orange:
                    return QIcon(":/images/Letters/O/orange.ico");
                case PlcViewItemStateColor_Pink:
                    return QIcon(":/images/Letters/O/pink.ico");
                case PlcViewItemStateColor_Red:
                    return QIcon(":/images/Letters/O/red.ico");
                case PlcViewItemStateColor_Violet:
                    return QIcon(":/images/Letters/O/violet.ico");
                default:
                    return QIcon(":/images/Letters/O/grey.ico");
                }
            }
            else
            {
                switch(stateColorOn)
                {
                case PlcViewItemStateColor_Black:
                    return QIcon(":/images/Letters/I/black.ico");
                case PlcViewItemStateColor_Blue:
                    return QIcon(":/images/Letters/I/blue.ico");
                case PlcViewItemStateColor_DarkGreen:
                    return QIcon(":/images/Letters/I/dg.ico");
                case PlcViewItemStateColor_Gold:
                    return QIcon(":/images/Letters/I/gold.ico");
                case PlcViewItemStateColor_Gray:
                    return QIcon(":/images/Letters/I/grey.ico");
                case PlcViewItemStateColor_LightGreen:
                    return QIcon(":/images/Letters/I/lg.ico");
                case PlcViewItemStateColor_Orange:
                    return QIcon(":/images/Letters/I/orange.ico");
                case PlcViewItemStateColor_Pink:
                    return QIcon(":/images/Letters/I/pink.ico");
                case PlcViewItemStateColor_Red:
                    return QIcon(":/images/Letters/I/red.ico");
                case PlcViewItemStateColor_Violet:
                    return QIcon(":/images/Letters/I/violet.ico");
                default:
                    return QIcon(":/images/Letters/I/grey.ico");
                }
            }
            break;
        default:
            break;
        }
    }

    return QVariant();
}
