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

PlcViewItem::PlcViewItem(OMC8000NodeInterface *node, PlcViewItem *parent)
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

   omc8000Node = node;
}

QString PlcViewItem::GetNodeValue(void) const
{
    QString s("");

    switch(omc8000Node->Type())
    {
    case OMC8000NodeBit:
        {
        bool bval;
        omc8000Node->Read(&bval);
        s.sprintf("%01X", bval);
        }
        break;
    case OMC8000NodeByte:
        {
        UINT8 bval;
        omc8000Node->Read(&bval);
        s.sprintf("%02X", bval);
        }
        break;
    case OMC8000NodeWord:
        {
        UINT16 bval;
        omc8000Node->Read(&bval);
        s.sprintf("%04X", bval);
        }
        break;
    case OMC8000NodeDWord:
        {
        UINT32 bval;
        omc8000Node->Read(&bval);
        s.sprintf("%08X", bval);
        }
        break;
    case OMC8000NodeReal32:
        {
        float bval;
        omc8000Node->Read(&bval);
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

    switch(omc8000Node->Type())
    {
    case OMC8000NodeBit:
        {
        bool bval;
        omc8000Node->Write(&bval);
        s.sprintf("%01X", bval);
        }
        break;
    case OMC8000NodeByte:
        {
        UINT8 bval;
        omc8000Node->Write(&bval);
        s.sprintf("%02X", bval);
        }
        break;
    case OMC8000NodeWord:
        {
        UINT16 bval;
        omc8000Node->Write(&bval);
        s.sprintf("%04X", bval);
        }
        break;
    case OMC8000NodeDWord:
        {
        UINT32 bval;
        omc8000Node->Write(&bval);
        s.sprintf("%08X", bval);
        }
        break;
    case OMC8000NodeReal32:
        {
        float bval;
        omc8000Node->Write(&bval);
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
    UINT32_MB Val;
    Val.dword = val;

    switch(omc8000Node->Type())
    {
    case OMC8000NodeBit:
        {
        bool v = Val.bit.b0;
        omc8000Node->Write(v);
        }
        break;
    case OMC8000NodeByte:
        {
        UINT8 b = Val.byte.byte0;
        omc8000Node->Write(b);
        }
        break;
    case OMC8000NodeWord:
        {
        UINT16 w = Val.word.word0;
        omc8000Node->Write(w);
        }
        break;
    case OMC8000NodeDWord:
        {
        UINT32 w = Val.dword;
        omc8000Node->Write(w);
        }
        break;
    case OMC8000NodeReal32:
        {
        float w = Val.real32;
        omc8000Node->Write(w);
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
    OMC8000Exception* e = NULL;

    if(omc8000Node!=NULL)
    {
        switch(column)
        {
        case PlcViewTreeColumn_Id:
            return QString(omc8000Node->Id());
        case PlcViewTreeColumn_Type:
            return QString(omc8000Node->TypeStr());
        case PlcViewTreeColumn_Read:
            if(omc8000Node->Access()&OMC8000NodeAccessRead)
            {
                switch(omc8000Node->Type())
                {
                case OMC8000NodeBit:
                    break;
                default:
                    return GetNodeValue();
                }
            }
            break;
        case PlcViewTreeColumn_Write:
            if(omc8000Node->Access()&OMC8000NodeAccessWrite)
            {
                return GetNodeWriteValue();
            }
            break;
        case PlcViewTreeColumn_Counter:
            switch(omc8000Node->Type())
            {
            case OMC8000NodeBit:
            case OMC8000NodeByte:
            case OMC8000NodeWord:
            case OMC8000NodeDWord:
            case OMC8000NodeReal32:
                svar.sprintf("%d", omc8000Node->GetCounter());
                return svar;
            }
            break;
        case PlcViewTreeColumn_ReadTimeStamp:
            if(omc8000Node->Access()&OMC8000NodeAccessRead)
            {
                switch(omc8000Node->Type())
                {
                case OMC8000NodeArea:
                    t = omc8000Node->ReadTimeStamp();
                    svar.sprintf("%02d:%02d:%02d.%03d", (int)t.wHour, (int)t.wMinute, (int)t.wSecond, (int)t.wMilliseconds);
                    return svar;
                }
            }
            break;
        case PlcViewTreeColumn_WriteTimeStamp:
            if(omc8000Node->Access()&OMC8000NodeAccessWrite)
            {
                switch(omc8000Node->Type())
                {
                case OMC8000NodeBit:
                case OMC8000NodeByte:
                case OMC8000NodeWord:
                case OMC8000NodeDWord:
                case OMC8000NodeReal32:
                    t = omc8000Node->WriteTimeStamp();
                    svar.sprintf("%02d:%02d:%02d.%03d", (int)t.wHour, (int)t.wMinute, (int)t.wSecond, (int)t.wMilliseconds);
                    return svar;
                }
            }
            break;
        case PlcViewTreeColumn_Description:
            return QString(omc8000Node->Description());
            break;
        case PlcViewTreeColumn_Exception:
            e = omc8000Node->GetLastException();
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
        const char* id = childItems.at(position)->omc8000Node->Id();
        delete childItems.takeAt(position);
        omc8000Node->RemoveNode(id);
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

    if(omc8000Node!=NULL)
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
    omc8000Node = VPtr<OMC8000NodeInterface>::asPtr(value);
    return true;
}

QVariant PlcViewItem::icon() const
{

    if(omc8000Node!=NULL)
    {
        switch(omc8000Node->Type())
        {
        case OMC8000NodeBit:
            return QIcon(":/images/Letters/Letter-B-gold.ico");
        case OMC8000NodeByte:
            return QIcon(":/images/Letters/Letter-B-orange.ico");
        case OMC8000NodeWord:
        case OMC8000NodeDWord:
            return QIcon(":/images/Letters/Letter-W-lg.ico");
        case OMC8000NodeTask:
            return QIcon(":/images/Letters/scheduled_tasks.ico");
        case OMC8000NodeReal32:
            return QIcon(":/images/Letters/Letter-R-blue.ico");
        case OMC8000NodeArea:
            return QIcon(":/images/Letters/Letter-V-blue.ico");

        }
    }

    return QVariant();
}

QVariant PlcViewItem::iconState() const
{
    if(omc8000Node!=NULL)
    {
        switch(omc8000Node->Type())
        {
        case OMC8000NodeBit:
            bool bval;
            omc8000Node->Read(&bval);
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
