#include "imagetable.h"
#include "imagedelegate.h"
#include "optimizer.h"
#include "tools/general.h"

#include <QApplication>
#include <QDateTime>
#include <QSettings>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDir>
#include <QPalette>
#include <QStringList>
#include <QUrl>
#include <QProcess>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QPainter>
#include <QElapsedTimer>

Q_DECLARE_METATYPE ( QFileInfo )

ImageTable::ImageTable(QWidget* parent) : QTableView(parent), mCurrentOpt(0), mSaved(0), mTotal(0), mPercent(0), mOptionsCache(false), mStop(false)
{
  qRegisterMetaType<QFileInfo>("QFileInfo");

  setIconSize(QSize(50,50));
  setItemDelegate(new ImageDelegate);
  setAcceptDrops(true);
  setDragEnabled(false);

  mModel = new QStandardItemModel(0, 5, this);
  mModel->setHeaderData(0, Qt::Horizontal, tr("Image") );
  mModel->setHeaderData(1, Qt::Horizontal, tr("Initial Size"));
  mModel->setHeaderData(2, Qt::Horizontal, tr("Result Size"));
  mModel->setHeaderData(3, Qt::Horizontal, tr("Reduction") );
  mModel->setHeaderData(4, Qt::Horizontal, tr("Progress") );

  setModel(mModel);
  verticalHeader()->hide();
  horizontalHeader()->setStretchLastSection(true);
}

void ImageTable::addImage(QString file)
{
  QFileInfo image(file);

  if (!mReady && mModel->rowCount() > 0) {
    clear();
  }

  if (!image.exists())
    return;

  if (image.isDir()) {
    QDir directory(file);

    foreach(QString innerFile, directory.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
      addImage(directory.path() + QDir::separator() + innerFile);
      qApp->processEvents(); // Avoid hanging the UI
    }

    return;
  }

  // Avoid detecting the type altogether if it's not a .png
  if (image.suffix().toLower() != "png")
    return;

  //Search for a duplicate
  //TODO: Search for by path, not only by name
  if (mModel->findItems(image.fileName()).count() > 0)
    return;

  QPixmap imagePixmap(image.filePath(), "PNG");

  // Detect if it's not a true PNG.
  if (imagePixmap.isNull())
    return;

  // Resize
  if (imagePixmap.width() > 150)
    imagePixmap = imagePixmap.scaledToWidth(150);

  if (imagePixmap.height() > 250)
    imagePixmap = imagePixmap.scaledToHeight(150);

  //Add it
  QStandardItem *item = new QStandardItem(QIcon(imagePixmap), image.fileName());
  item->setData(image.filePath(), Qt::UserRole+2);
  item->setData(qVariantFromValue(image), Qt::UserRole+3);

  mModel->appendRow(item);
  mModel->setData(mModel->index(item->row(), 1), representBytes(image.size()));
  mModel->setData(mModel->index(item->row(), 3), "");
  mModel->setData(mModel->index(item->row(), 4), 0);

  setRowHeight(item->row(), 54);

  resizeColumnToContents(item->column());

  if (columnWidth(item->column()) > 150)
    setColumnWidth(item->column(), 150);

  resizeColumnToContents(3);

  if (!mReady) {
    emit ready(true);
    mReady = true;
  }
}

void ImageTable::addImages(QStringList images)
{
  QString image;

  foreach (image, images) {
    addImage(image);
    qApp->processEvents();
  }
}

void ImageTable::renewOptionsCache()
{
  mOptionsCache = false;
}

void ImageTable::optimize()
{
  if (mModel->rowCount() < 1)
    return;

  mOptimizer = 0;

  if (mStop) {
    mStop = false;
    clear();
    return;
  }

  if (!mModel->index(mCurrentOpt, 0).isValid())
  {
    emit busy(false);
    mBusy = false;
    mStop = false;
    emit ready(false);
    mReady = false;
    setSelectionMode(QAbstractItemView::MultiSelection);
    emit batchDone(mSaved, mElapsedTime.elapsed());
    mCurrentOpt = 0;
    return;
  }

  if (mCurrentOpt == 0)
  {
    //Starting
    emit busy(true);
    mBusy = true;
    mStop = false;

    setSelectionMode(QAbstractItemView::NoSelection);
    mElapsedTime.start();
    mSaved = 0;
  }

  static Optimizer::Options options;
  if (!mOptionsCache) {
    QSettings settings;
    settings.beginGroup("options");
    options.optimizationLevel = settings.value("png/optimizationLevel", 3).toInt();
    options.errorRecovery = settings.value("png/errorRecovery").toBool();
    options.backup = settings.value("png/backup").toBool();
    options.output = settings.value("output").toBool();
    options.directory = settings.value("directory").toString();
    options.advanced = settings.value("advanced/enabled").toBool();
    options.arguments = settings.value("advanced/arguments").toString();
    settings.endGroup();
    mOptionsCache = true;
  }

  mOptimizer = new Optimizer(mModel->data(mModel->index(mCurrentOpt, 0), Qt::UserRole+2).toString(), options);
  connect(mOptimizer, SIGNAL(done(int, qreal)), this, SLOT(optimizationDone(int, qreal)));
  connect(mOptimizer, SIGNAL(progress(int)), this, SLOT(optimizationProgress(int)));

  if (mCurrentOpt < mModel->rowCount()) {
    mCurrentOpt++;
  }
}

void ImageTable::optimizationProgress(int progress)
{
  mModel->setData(mModel->index(mCurrentOpt-1, 4), progress);
}

void ImageTable::optimizationDone(int fileSize, qreal decrease)
{
  mModel->setData(mModel->index(mCurrentOpt-1, 4), 100);

  QFileInfo info = mModel->item(mCurrentOpt-1, 0)->data(Qt::UserRole+3).value<QFileInfo>();

  // Stats:
  if (fileSize > 0)
    mSaved += (info.size() - fileSize);

  if (fileSize == 0)
  {
    setRowColor(mCurrentOpt-1, Qt::yellow);
  }
  else if (fileSize == -1)
  {
    setRowColor(mCurrentOpt-1, Qt::red);
  }
  else
  {
    mModel->setData(mModel->index(mCurrentOpt-1, 2), representBytes(fileSize));
    mModel->setData(mModel->index(mCurrentOpt-1, 3), tr("%1%\n(-%2)").arg(decrease).arg(representBytes((info.size() - fileSize))));
    setRowColor(mCurrentOpt-1, Qt::green);
  }

  optimize();
}

void ImageTable::stop()
{
  if (mBusy) {
    mStop/*, hammertime*/ = true;

    if (mOptimizer)
      mOptimizer->deleteLater();

    optimize();
  }
}

void ImageTable::clear()
{
  mModel->removeRows(0, mModel->rowCount());
  emit ready(false);
  emit busy(false);
  mCurrentOpt = 0;
  mReady = false;
}

void ImageTable::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void ImageTable::dragMoveEvent(QDragMoveEvent *event)
{
  if (event->mimeData()->hasUrls())
    event->acceptProposedAction();
}

void ImageTable::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void ImageTable::dropEvent(QDropEvent *event)
{
  QUrl url;

  foreach (url, event->mimeData()->urls()) {
    addImage(url.toLocalFile());
    qApp->processEvents();
  }
}

void ImageTable::keyPressEvent(QKeyEvent *event)
{
  if ((event->key() == Qt::Key_Backspace
   || event->key() == Qt::Key_Delete)
    && !mBusy)  {
    QModelIndexList indexes;

    while((indexes = selectionModel()->selectedIndexes()).size()) {
        model()->removeRow(indexes.first().row());
    }

    if (model()->rowCount() == 0) {
      emit ready(false);
      mReady = false;
    }
  }

  QTableView::keyPressEvent(event);
}

void ImageTable::mousePressEvent(QMouseEvent *event)
{
  if (rowAt(event->pos().y()) == -1
      || event->button() == Qt::RightButton) {
    setSelection(QRect(1,1,1,1), QItemSelectionModel::Clear); // Clear the selection when clicking in the void.
  }

  QTableView::mousePressEvent(event);
}

void ImageTable::paintEvent(QPaintEvent *event)
{
  if (mModel->rowCount() == 0)
  {
    QPainter painter(viewport());

    painter.setOpacity(0.6);

    QPixmap watermark(":/images/addImages.Watermark");
    QPoint watermarkPoint = rect().center() - QPoint(watermark.size().width()/2, (watermark.size().height()/2) + 35);

    painter.drawPixmap(watermarkPoint, watermark);

    painter.setOpacity(0.7);

    painter.setPen(palette().color(QPalette::Text));
    painter.drawText(rect(), Qt::AlignCenter, tr("Drag and drop images or folders here"));
  }

  QTableView::paintEvent(event);
}

void ImageTable::setRowColor(int row, QColor color)
{ // Faster?
  QLinearGradient gradient(290, 0, 0, 0);

  gradient.setColorAt(0, color);

  color.setAlpha(0);
  gradient.setColorAt(1, color);

  QBrush brush(gradient);
  mModel->setData(mModel->index(row, 4), brush, Qt::BackgroundRole);
}
