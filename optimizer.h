#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <QProcess>

class QStandardItemModel;
class Optimizer : public QProcess
{
  Q_OBJECT

public:
  struct Options
  {
    int optimizationLevel;
    bool errorRecovery;
    bool backup;
    bool output;
    bool advanced;
    QString directory;
    QString arguments;
  };

  Optimizer(QString filePath, Options options);

signals:
  void done(int fileSize, qreal decrease);
  void progress(int progress);

private slots:
  void readOutput();

};

#endif // OPTIMIZER_H
