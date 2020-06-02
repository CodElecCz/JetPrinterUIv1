#include <QSpinBox>
#include <QPainter>
#include "spinboxdelegate.h"
#include "hexspinbox.h"

SpinBoxDelegate::SpinBoxDelegate(QObject *parent) :
   QStyledItemDelegate(parent)// QItemDelegate(parent)
{
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
     const QStyleOptionViewItem &/* option */,
     const QModelIndex &/* index */) const
 {
     //QSpinBox *editor = new QSpinBox(parent);
     HexSpinBox *editor = new HexSpinBox(parent);
     editor->setMinimum(0);
     editor->setMaximum(0xffff);

     return editor;
 }

void SpinBoxDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
 {
    QString val = index.model()->data(index, Qt::EditRole).toString();
    bool ok;
    int value = val.toInt(&ok, 16);

     //QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
     HexSpinBox *spinBox = static_cast<HexSpinBox*>(editor);
     spinBox->setValue(value);
 }

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
 {
     //QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
     HexSpinBox *spinBox = static_cast<HexSpinBox*>(editor);
     spinBox->interpretText();
     int value = spinBox->value();

     model->setData(index, value, Qt::EditRole);
 }

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
 {
     editor->setGeometry(option.rect);
 }
