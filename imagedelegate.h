#ifndef IMAGEDELEGATE_H
#define IMAGEDELEGATE_H

#include <QItemDelegate>

class ImageDelegate : public QItemDelegate
{
public:
    ImageDelegate( QObject * parent = 0 );
    void paint (QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

#endif // IMAGEDELEGATE_H
