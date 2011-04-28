#include "imagedelegate.h"
#include <QApplication>
#include <QPainter>
#include <QDebug>

ImageDelegate::ImageDelegate(QObject *parent) : QItemDelegate(parent)
{
}

void ImageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
  QItemDelegate::paint(painter, option, index);

  if (index.column() == 4) {
      int progress = index.data().toInt();

      QStyleOptionProgressBar progressBarOption;

      QRect optionRectCentered = option.rect;
      optionRectCentered.setTop(optionRectCentered.top() + (option.rect.height()/3.5));
      optionRectCentered.setBottom(optionRectCentered.bottom() - (option.rect.height()/3.5));

      optionRectCentered.setLeft(optionRectCentered.left() + 3);
      optionRectCentered.setRight(optionRectCentered.right() - 3);

      progressBarOption.rect = optionRectCentered;
      progressBarOption.minimum = 0;
      progressBarOption.maximum = 100;
      progressBarOption.progress = progress;
      progressBarOption.textVisible = false;

      QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                         &progressBarOption, painter);
  }
}
