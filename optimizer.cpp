#include "optimizer.h"

#include <QApplication>
#include <QStandardItemModel>
#include <QDebug>
#include <QDir>

Optimizer::Optimizer(QString filePath, Options options) : QProcess(0)
{
  if (options.advanced) {
    start("optipng.exe " + options.arguments.arg(filePath));
  }
  else {
    QStringList arguments;
    arguments << QString("-o%1").arg(options.optimizationLevel);

    if (options.errorRecovery)
      arguments << "-error";

    if (options.backup)
      arguments << "-keep";

    if (options.output)
    {
      if (options.directory.isEmpty())
        options.directory = qApp->applicationDirPath();

      QDir dir(options.directory);

      if (!dir.exists())
        dir.mkdir(dir.path());

      arguments << QString("-dir %1").arg(dir.path());
    }

    arguments << filePath;
    start("optipng.exe", arguments);
  }

  connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
}

void Optimizer::readOutput()
{
  static int messageCount = 0;

  if (messageCount != 0 && messageCount <= 9)
    emit progress(messageCount*10); //TODO An actual progressbar a not just a placebo :)

  messageCount++;

  QString output(readAllStandardOutput());


  if (output.contains("already optimized"))
  {
    messageCount = 0;
    emit done(0, 0);
  }
  else if (output.contains("Error:"))
  {
    //TODO: Get the error and show it somewhere?
    messageCount = 0;
    emit done(-1, -1);
  }
  else if (output.contains("Output file size ="))
  {
    messageCount = 0;

    int outFileSize = output.indexOf("Output file size = ") + 19;
    int endOutFileSize = output.indexOf(" bytes", outFileSize);
    QString outputSize = output.mid(outFileSize, (endOutFileSize-outFileSize));

    int decreasePos = output.indexOf("tes = ", endOutFileSize) + 6;
    int endDecreasePos = output.indexOf("% decrease", decreasePos);
    QString decrease = output.mid(decreasePos, (endDecreasePos-decreasePos));

    emit done(outputSize.toInt(), decrease.toFloat());
  }
}
