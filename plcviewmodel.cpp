#include <QFile>
#include <QIcon>
#include <QSpinBox>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QThread>
#include <QMetaObject>
#include "plcviewmodel.h"

PlcViewModel::PlcViewModel(OMC8000Lib* omc8000Lib, const QString xmlFile, QObject *parent) :
    QAbstractItemModel(parent)
{
    //qDebug()<<"PlcViewModel threadId:0x" + QString::number((INT32)thread()->currentThreadId(), 16);

    rootItem = new PlcViewItem();

    if(!xmlFile.isNull() && !xmlFile.isEmpty())
    {
        setupModelData(omc8000Lib, xmlFile, rootItem);
    }
}

PlcViewModel::~PlcViewModel()
{
    delete rootItem;
}

void PlcViewModel::loadModelData(OMC8000Lib* omc8000Lib, const QString xmlFile)
{
    if(!xmlFile.isNull() && !xmlFile.isEmpty())
    {
        setupModelData(omc8000Lib, xmlFile, rootItem);
    }
}

void PlcViewModel::updateNode(QString itemName, QModelIndex parentIndex)
{
    int varCount = this->rowCount(parentIndex);
    for(int i = 0; i<varCount; i++)
    {
         QModelIndex varIndex = parentIndex.child(i, PlcViewTreeColumn_Id);
         QString varName = varIndex.data().toString();
         if(varName == itemName)
         {
             QModelIndex index = parentIndex.child(i, PlcViewTreeColumn_Read);
             emit dataChanged(index, index);
             break;
         }
         else
         {
             updateNode(itemName, varIndex);
         }
    }
}

void PlcViewModel::update(QString itemName)
{
    //qDebug()<<"PlcViewModel::update(QString) ["+ itemName + "] threadId:0x" + QString::number((INT32)thread()->currentThreadId(), 16);

    int taskCount = this->rowCount(QModelIndex());
    for(int i = 0; i<taskCount; i++)
    {
        QModelIndex taskIndex = this->index(i, PlcViewTreeColumn_Id, QModelIndex());

        int areaCount = this->rowCount(taskIndex);
        for(int j = 0; j<areaCount; j++)
        {
            QModelIndex areaIndex = taskIndex.child(j, PlcViewTreeColumn_Id);
            QString areaName = areaIndex.data().toString();
            if(areaName == itemName)
            {
                QModelIndex index0 = taskIndex.child(j, PlcViewTreeColumn_Read);
                QModelIndex index1 = taskIndex.child(j, PlcViewTreeColumn_WriteTimeStamp);
                emit dataChanged(index0, index1);
                break;
            }
            else
            {
                //updateNode(itemName, areaIndex); ???
            }
        }
    }   
}

void PlcViewModel::update()
{
    //qDebug()<<"PlcViewModel::update() threadId:0x" + QString::number((INT32)thread()->currentThreadId(), 16);

    int taskCount = this->rowCount(QModelIndex());
    for(int i = 0; i<taskCount; i++)
    {
        QModelIndex startIndex = this->index(i, PlcViewTreeColumn_Type, QModelIndex());
        QModelIndex stopIndex = this->index(i, PlcViewTreeColumnSize-1, QModelIndex());

        emit dataChanged(startIndex, stopIndex);
    }
}

void PlcViewModel::OnValueChanged(OMC8000NodeInterface* node, UINT32 val)
{
    if (thread()!=QThread::currentThread())
    {
        QString *nodeId = new QString(node->Id());
        //qDebug()<<"PlcViewModel::OnValueChanged() [" + *nodeId + "] threadId:0x" + QString::number((int)thread()->currentThreadId(), 16);

        QGenericArgument arg("QString", nodeId);
        QMetaObject::invokeMethod(this, "update", arg);
    }
    else
        emit this->update(node->Id());
}

QVariant PlcViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    PlcViewItem *item = static_cast<PlcViewItem*>(index.internalPointer());

    if(role == Qt::DisplayRole)
    {
        return item->data(index.column());
    }
    else if(role == Qt::EditRole)
    {
        return item->data(index.column());
    }
    else if(role == Qt::DecorationRole)
    {
        if(index.column()==PlcViewTreeColumn_Id)
            return item->icon();
        else if(index.column()==PlcViewTreeColumn_Read)
        {
            return item->iconState();
        }
    }

    return QVariant();
}

int PlcViewModel::rowCount(const QModelIndex &parent) const
{
    PlcViewItem *parentItem;
    if (parent.column() > 0)
        return 0;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<PlcViewItem*>(parent.internalPointer());

     return parentItem->childCount();
}

int PlcViewModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<PlcViewItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QModelIndex PlcViewModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

     PlcViewItem *parentItem;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<PlcViewItem*>(parent.internalPointer());

     PlcViewItem *childItem = parentItem->child(row);
     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
}

QModelIndex PlcViewModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    PlcViewItem *childItem = static_cast<PlcViewItem*>(child.internalPointer());
    PlcViewItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

Qt::ItemFlags PlcViewModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flag = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (!index.isValid())
        return 0;

    if(index.column()==PlcViewTreeColumn_Write)
        flag |= Qt::ItemIsEditable;

    return flag;
}

QVariant PlcViewModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
 {
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
         return rootItem->data(section);

     return QVariant();
 }

PlcViewItem *PlcViewModel::getItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        PlcViewItem *item = static_cast<PlcViewItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

bool PlcViewModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool result = false;

    if (role != Qt::EditRole)
        return false;

    PlcViewItem *item = getItem(index);
    switch(value.type())
    {
    case QVariant::String:
    case QVariant::Int:
        result = item->setData(index.column(), value);
        break;
    default:
        result = item->setData(value);
        break;
    }

    if (result)
        emit dataChanged(index, index);

    return result;
}

bool PlcViewModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    PlcViewItem *parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();

    return success;
}

bool PlcViewModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    PlcViewItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

void PlcViewModel::setupDefaultNode(PlcViewItem* parentItem, OMC8000NodeInterface* parentNode)
{
    PlcViewItem* nodeItem;
    vector<OMC8000NodeInterface*> nodeList = parentNode->GetNodes();
    for(UINT32 i = 0; i<nodeList.size(); i++)
    {
        OMC8000NodeInterface* node  = nodeList[i];

        node->OnNodeValueChanged += event_handler(this, &PlcViewModel::OnValueChanged);
        nodeItem = new PlcViewItem(node, parentItem);

        //search for subnodes
        setupDefaultNode(nodeItem, node);

        parentItem->appendChild(nodeItem);
    }
}

PlcViewItemStateColors Omc8000ItemColor(QString scolor)
{
    PlcViewItemStateColors color;
    if(!scolor.isNull() && !scolor.isEmpty())
    {
        if(scolor.contains("Black", Qt::CaseInsensitive))
            color = PlcViewItemStateColor_Black;
        else if(scolor.contains("Blue", Qt::CaseInsensitive))
            color = PlcViewItemStateColor_Blue;
        else if(scolor.contains("DarkGreen", Qt::CaseInsensitive))
            color = PlcViewItemStateColor_DarkGreen;
        else if(scolor.contains("Gold", Qt::CaseInsensitive))
            color = PlcViewItemStateColor_Gold;
        else if(scolor.contains("Geay", Qt::CaseInsensitive))
            color = PlcViewItemStateColor_Gray;
        else if(scolor.contains("LightGreen", Qt::CaseInsensitive) || scolor.contains("Green", Qt::CaseInsensitive))
            color = PlcViewItemStateColor_LightGreen;
        else if(scolor.contains("Orange", Qt::CaseInsensitive))
            color = PlcViewItemStateColor_Orange;
        else if(scolor.contains("Pink", Qt::CaseInsensitive))
            color = PlcViewItemStateColor_Pink;
        else if(scolor.contains("Red", Qt::CaseInsensitive))
            color = PlcViewItemStateColor_Red;
        else if(scolor.contains("Violet", Qt::CaseInsensitive))
            color = PlcViewItemStateColor_Violet;
        else
            color = PlcViewItemStateColor_Gray;
    }

    return color;
}

void PlcViewModel::setupNode(QDomElement parentElem, PlcViewItem* parentItem, OMC8000NodeInterface* parentNode)
{
    OMC8000NodeInterface *node;
    PlcViewItem* nodeItem;

    QDomNode nodeNode = parentElem.firstChild();
    while(!nodeNode.isNull())
    {
        //node
        QDomElement nodeElem = nodeNode.toElement();
        if(!nodeElem.isNull())
        {
            QString sid = nodeElem.attribute("id");
            QString stype = nodeElem.attribute("type");
            QString offset = nodeElem.attribute("offset");
            QString access = nodeElem.attribute("access");
            QString sdesc = nodeElem.attribute("desc");

            //default RW
            bool isR = true;
            bool isW = true;
            if(!access.isNull() && !access.isEmpty())
            {
                isR = (access.indexOf(QChar('R'))>=0);
                isW = (access.indexOf(QChar('W'))>=0);
            }
            OMC8000NodeAccess node_access = (OMC8000NodeAccess)((isR? OMC8000NodeAccessRead:0)|(isW? OMC8000NodeAccessWrite:0));

            if(stype.contains("REAL", Qt::CaseInsensitive))
            {
                parentNode->AddNode(sid.toLatin1().constData(), OMC8000NodeReal32, offset.toUShort(), node_access);
                node = parentNode->GetNodeInterface(sid.toLatin1().constData());
                node->Description(sdesc.toUtf8().constData());
                //node->OnNodeValueChanged += event_handler(this, &PlcViewModel::OnValueChanged);
                nodeItem = new PlcViewItem(node, parentItem);

                //nodes
                setupNode(nodeElem, nodeItem, node);

                parentItem->appendChild(nodeItem);
            }
            else if(stype.contains("DWORD", Qt::CaseInsensitive))
            {
                parentNode->AddNode(sid.toLatin1().constData(), OMC8000NodeDWord, offset.toUShort(), node_access);
                node = parentNode->GetNodeInterface(sid.toLatin1().constData());
                node->Description(sdesc.toUtf8().constData());
                //node->OnNodeValueChanged += event_handler(this, &PlcViewModel::OnValueChanged);
                nodeItem = new PlcViewItem(node, parentItem);

                //nodes
                setupNode(nodeElem, nodeItem, node);

                parentItem->appendChild(nodeItem);
            }
            else if(stype.contains("WORD", Qt::CaseInsensitive))
            {
                parentNode->AddNode(sid.toLatin1().constData(), OMC8000NodeWord, offset.toUShort(), node_access);
                node = parentNode->GetNodeInterface(sid.toLatin1().constData());
                node->Description(sdesc.toUtf8().constData());
                //node->OnNodeValueChanged += event_handler(this, &PlcViewModel::OnValueChanged);
                nodeItem = new PlcViewItem(node, parentItem);

                //nodes
                setupNode(nodeElem, nodeItem, node);

                parentItem->appendChild(nodeItem);
            }
            else if(stype.contains("BYTE", Qt::CaseInsensitive))
            {
                parentNode->AddNode(sid.toLatin1().constData(), OMC8000NodeByte, offset.toUShort(), node_access);
                node = parentNode->GetNodeInterface(sid.toLatin1().constData());
                node->Description(sdesc.toUtf8().constData());
                //node->OnNodeValueChanged += event_handler(this, &PlcViewModel::OnValueChanged);
                nodeItem = new PlcViewItem(node, parentItem);

                //nodes
                setupNode(nodeElem, nodeItem, node);

                parentItem->appendChild(nodeItem);
            }
            if(stype.contains("BIT", Qt::CaseInsensitive) || stype.contains("BOOL", Qt::CaseInsensitive))
            {
                parentNode->AddNode(sid.toLatin1().constData(), OMC8000NodeBit, offset.toUShort(), node_access);
                node = parentNode->GetNodeInterface(sid.toLatin1().constData());
                node->Description(sdesc.toUtf8().constData());
                //node->OnNodeValueChanged += event_handler(this, &PlcViewModel::OnValueChanged);
                nodeItem = new PlcViewItem(node, parentItem);

                //color
                QString scolor_on = nodeElem.attribute("color_on");
                if(!scolor_on.isNull() && !scolor_on.isEmpty())
                {
                    nodeItem->setcolorOn(Omc8000ItemColor(scolor_on));
                }
                QString scolor_off = nodeElem.attribute("color_off");
                if(!scolor_off.isNull() && !scolor_off.isEmpty())
                {
                    nodeItem->setcolorOff(Omc8000ItemColor(scolor_off));
                }

                //delay
                QString sdelayon = nodeElem.attribute("delay_on");
                QString sdelayoff = nodeElem.attribute("delay_off");
                if(!sdelayon.isNull() && !sdelayon.isEmpty() && !sdelayoff.isNull() && !sdelayoff.isEmpty())
                {
                    node->DelayTimer(sdelayon.toULong(), sdelayoff.toULong());
                }

                //nodes
                setupNode(nodeElem, nodeItem, node);

                parentItem->appendChild(nodeItem);
            }
        }
        nodeNode = nodeNode.nextSibling();
    }
}

void PlcViewModel::setupModelData(OMC8000Lib* omc8000, const QString xmlFile, PlcViewItem *parent)
{
    OMC8000Task *task;
    OMC8000NodeInterface *taskInterface;
    OMC8000NodeInterface *area;
    PlcViewItem* taskItem;
    PlcViewItem* areaItem;

    QDomDocument doc(xmlFile);
    QFile file(xmlFile);
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement docElem = doc.documentElement();
    QDomNode taskNode = docElem.firstChild();
    //tasks
    while(!taskNode.isNull())
    {
        //task
        QDomElement taskElem = taskNode.toElement();
        if(!taskElem.isNull())
        {
            QString sid = taskElem.attribute("id");
            QString sip = taskElem.attribute("ip");
            QString st = taskElem.attribute("loop_time");
            QString lp = taskElem.attribute("local_port");
            QString rp = taskElem.attribute("remote_port");
            QString sdesc = taskElem.attribute("desc");

            bool ip_task = (sip!=NULL && !sip.isEmpty());
            if(ip_task)
            {
                omc8000->AddTask(sid.toLatin1().data(), sip.toLatin1().constData(), lp.toUShort(), rp.toUShort(), st.toUInt());
            }
            else //serial
            {
                 QString scom = taskElem.attribute("com");
                 QString sbaudrate = taskElem.attribute("baud_rate");
                 omc8000->AddTask(sid.toLatin1().data(), scom.toLatin1().data(), (SerialProtocol::BaudRate)sbaudrate.toUInt(0, 10) , st.toUInt());
            }
            task = omc8000->GetTask(sid.toLatin1().constData());
            taskInterface = static_cast<OMC8000NodeInterface*>(task);
            if(!sdesc.isNull() && !sdesc.isEmpty())
                taskInterface->Description(sdesc.toUtf8().data());
            taskItem = new PlcViewItem(taskInterface, parent);

            //check default areas
            vector<OMC8000NodeInterface*> areaList = task->GetNodes();
            for(UINT32 i = 0; i<areaList.size(); i++)
            {
                area = areaList[i];
                area->OnNodeValueChanged += event_handler(this, &PlcViewModel::OnValueChanged);
                areaItem = new PlcViewItem(area, taskItem);

                //search for subnodes
                setupDefaultNode(areaItem, area);

                taskItem->appendChild(areaItem);
            }

            //areas
            QDomNode areaNode = taskElem.firstChild();
            while(!areaNode.isNull())
            {
                //area
                QDomElement areaElem = areaNode.toElement();
                if(!areaElem.isNull())
                {
                    QString sid = areaElem.attribute("id");
                    QString stype = areaElem.attribute("type");
                    QString svariable = areaElem.attribute("variable");
                    QString access = areaElem.attribute("access");
                    QString sdevid = areaElem.attribute("com_dev");
                    QString sdesc = areaElem.attribute("desc");

                    //default RW
                    bool isR = true;
                    bool isW = true;
                    if(!access.isNull() && !access.isEmpty())
                    {
                        isR = (access.indexOf(QChar('R'))>=0) || (access.indexOf(QChar('r'))>=0);
                        isW = (access.indexOf(QChar('W'))>=0) || (access.indexOf(QChar('w'))>=0);
                    }
                    OMC8000NodeAccess node_access = (OMC8000NodeAccess)((isR? OMC8000NodeAccessRead:0)|(isW? OMC8000NodeAccessWrite:0));

                    //serial dev id
                    UINT8 devid = 255;
                    if(!sdevid.isNull() && !sdevid.isEmpty())
                    {
                        devid = sdevid.toUInt();
                    }

                    task->AddArea(sid.toLatin1().constData(), svariable.toLatin1().constData(), node_access);
                    area = task->GetNodeInterface(sid.toLatin1().data());
                    area->Description(sdesc.toUtf8().constData());
                    area->OnNodeValueChanged += event_handler(this, &PlcViewModel::OnValueChanged);
                    areaItem = new PlcViewItem(area, taskItem);

                    //nodes
                    setupNode(areaElem, areaItem, area);

                    taskItem->appendChild(areaItem);
                }

                areaNode = areaNode.nextSibling();
            }

            parent->appendChild(taskItem);
        }
        taskNode = taskNode.nextSibling();
    }
}
