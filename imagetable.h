#ifndef IMAGETABLE_H
#define IMAGETABLE_H

#include <QTableView>
#include <QFile>
#include <QElapsedTimer>

class QStandardItemModel;
class QMenu;
class Optimizer;
class ImageTable : public QTableView
{
  Q_OBJECT
public:
    ImageTable(QWidget* parent);
    void addImage(QString file);
    void addImages(QStringList images);
    void renewOptionsCache();

public slots:
    void optimize();
    void optimizationProgress(int progress);
    void optimizationDone(int fileSize, qreal decrase);
    void clear();
    void stop();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void setRowColor(int row, QColor color);

signals:
    void busy(bool isBusy); // Emits true when starting optimization, false when done.
    void ready(bool isReady); // Emits true when it has images to optimize, false otherwise.
    void batchDone(qint64 saved, int time); // Emits when the current image batch is done.

private:
    Optimizer* mOptimizer;
    QStandardItemModel *mModel;
    QMenu *mContextMenu;
    QElapsedTimer mElapsedTime;
    int mCurrentOpt;
    qint64 mSaved;
    int mTotal;
    int mPercent;
    bool mReady;
    bool mBusy;
    bool mOptionsCache;
    bool mStop;
};

#endif // IMAGETABLE_H
