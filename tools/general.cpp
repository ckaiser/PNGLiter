#include <QApplication>
#include <QString>

QString representBytes(qint64 bytes) {
  qreal kb = 1024;
  qreal mb = 1024 * kb;
  qreal gb = 1024 * mb;

  if (bytes < kb) {
    return qApp->tr("%1 b").arg(bytes);
  }
  else if (bytes < mb) {
    return qApp->tr("%1 kb").arg(qRound(bytes/kb));
  }
  else if (bytes > mb && bytes < gb) {
    return qApp->tr("%1 mb").arg(QString::number(bytes/mb, 'g', 3));
  }
  else if (bytes > gb) {
    return qApp->tr("%1 gb").arg(QString::number(bytes/gb, 'g', 3));
  }
}
