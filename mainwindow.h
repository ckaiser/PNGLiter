#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class QAction;
class QActionGroup;
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addDialog();
    void showAbout();
    void showOptions();
    void showResults(qint64 saved, int time);
    void setBusy(bool busy);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);

private:
    Ui::MainWindow *ui;
    QAction *mOptimizeAction;
    QActionGroup *mActionGroup;
};

#endif // MAINWINDOW_H
