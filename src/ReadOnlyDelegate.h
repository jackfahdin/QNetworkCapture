#ifndef READONLYDELEGATE_H
#define READONLYDELEGATE_H

#include <QItemDelegate>
#include <QStyleOptionViewItem>

class ReadOnlyDelegate : public QItemDelegate
{
public:
    explicit ReadOnlyDelegate(QWidget *parent = nullptr) : QItemDelegate(parent) {}

    // 重写 createEditor 函数，返回 NULL，以确保用户无法编辑视图中的项
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(parent)
        Q_UNUSED(option)
        Q_UNUSED(index)
        return nullptr;
    }
};

#endif // READONLYDELEGATE_H
