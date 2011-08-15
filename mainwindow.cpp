#include "mainwindow.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/optionsdialog.h"
#include "tools/general.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QFileDialog>
#include <QLabel>
#include <QSettings>
#include <QSysInfo>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // Widgets
  mOptimizeAction = new QAction(QIcon(":/images/optimize"), "&Optimize", this);
  mOptimizeAction->setEnabled(false);

  mActionGroup = new QActionGroup(this);

  mClearAction = new QAction(QIcon(":/images/clear")    , "C&lear", this);
  mClearAction->setEnabled(false);

  QAction *addAction       = new QAction(QIcon(":/images/addImages"), "&Add Images", this);
  QAction *configureAction = new QAction(QIcon(":/images/configure"), "&Configure", this);
  QAction *aboutAction     = new QAction(QIcon(":/images/about")    , "A&bout", this);

  mActionGroup->addAction(addAction);
  mActionGroup->addAction(mClearAction);
  mActionGroup->addAction(configureAction);
  mActionGroup->addAction(aboutAction);

  ui->infoWidget->setVisible(false);

  ui->mainToolBar->addAction(mOptimizeAction);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addAction(addAction);
  ui->mainToolBar->addAction(mClearAction);
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addAction(configureAction);
  ui->mainToolBar->addAction(aboutAction);

  // Signals & Slots
  connect(ui->imageTable , SIGNAL(busy(bool)) , this, SLOT(setBusy(bool)));
  connect(ui->imageTable , SIGNAL(ready(bool)), mOptimizeAction, SLOT(setEnabled(bool)));

  connect(ui->imageTable , SIGNAL(batchDone(qint64,int)), this, SLOT(showResults(qint64,int)));
  connect(mOptimizeAction, SIGNAL(triggered()), ui->imageTable, SLOT(optimize()));
  connect(addAction      , SIGNAL(triggered()), this, SLOT(addDialog()));
  connect(mClearAction   , SIGNAL(triggered()), ui->imageTable, SLOT(clear()));
  connect(ui->imageTable , SIGNAL(ready(bool)), mClearAction, SLOT(setEnabled(bool)));

  connect(configureAction, SIGNAL(triggered()), this, SLOT(showOptions()));
  connect(aboutAction    , SIGNAL(triggered()), this, SLOT(showAbout()));

  // Visual
#ifdef Q_WS_WIN
    setStyleSheet("QToolBar { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(250, 252, 253, 255), stop:0.502825 rgba(230, 240, 250, 255), stop:0.51 rgba(220, 230, 244, 255), stop:1 rgba(220, 230, 244, 255)); border-bottom: 1px solid #a0afc3; }");
    ui->infoWidget->setStyleSheet("#infoWidget { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(250, 252, 253, 255), stop:0.502825 rgba(230, 240, 250, 255), stop:0.51 rgba(220, 230, 244, 255), stop:1 rgba(220, 230, 244, 255)); border-top: 1px solid #a0afc3; }");
#endif

  // Settings
  QSettings settings;
  restoreGeometry(settings.value("geometry").toByteArray());

  // Checking for the OptiPNG exe
  if (!QFile::exists("optipng.exe")) {
    QMessageBox::warning(this, tr("Fatal Error"), tr("OptiPNG not found"));
    setEnabled(false);
    return;
  }

  // Arguments
  QStringList args = QCoreApplication::arguments();
  args.removeAt(0); // Remove the executable path.

  if (!args.isEmpty()) {
    ui->imageTable->addImages(args);
  }
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::addDialog()
{
  QStringList files = QFileDialog::getOpenFileNames(this, tr("Add Images"), "", tr("PNG Files (*.png)"));
  ui->imageTable->addImages(files);
}

void MainWindow::showAbout()
{
  AboutDialog *aboutDialog = new AboutDialog(this);
  aboutDialog->exec();
  delete aboutDialog;
}

void MainWindow::showOptions()
{
  OptionsDialog *optionsDialog = new OptionsDialog(this);

  if (optionsDialog->exec() == QDialog::Accepted) {
    ui->imageTable->renewOptionsCache();
  }

  delete optionsDialog;
}


void MainWindow::showResults(qint64 saved, int time)
{
  if (saved >= 0 && saved <= 5) {
    ui->savingsLabel->setText(tr("none"));
  }
  else {
    ui->savingsLabel->setText(representBytes(saved));
  }

  QString timeString;

  int seconds = (time / 1000) % 60;
  int minutes = (time / 1000) / 60;
  int hours   = (time / 1000) / 3600;

  if (hours >= 1)
    timeString = QString::number(hours) + " hours ";

  if (minutes >= 1)
    timeString += QString::number(minutes) + " minutes ";

  if (seconds >= 1)
    timeString += QString::number(seconds) + " seconds";

  ui->timeLabel->setText(timeString);
  ui->infoWidget->show();
}

void MainWindow::setBusy(bool busy)
{
  mActionGroup->setDisabled(busy);

  disconnect(mOptimizeAction, SIGNAL(triggered()), ui->imageTable, SLOT(stop()));
  disconnect(mOptimizeAction, SIGNAL(triggered()), ui->imageTable, SLOT(optimize()));

  if (busy) {
    mOptimizeAction->setText("&Abort");
    mOptimizeAction->setIcon(QIcon(":/images/close"));
    mOptimizeAction->setEnabled(true);
    connect(mOptimizeAction, SIGNAL(triggered()), ui->imageTable, SLOT(stop()));
  }
  else {
    mOptimizeAction->setText("&Optimize");
    mOptimizeAction->setIcon(QIcon(":/images/optimize"));
    connect(mOptimizeAction, SIGNAL(triggered()), ui->imageTable, SLOT(optimize()));
  }
}

void MainWindow::changeEvent(QEvent *e)
{
  QMainWindow::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
  QSettings().setValue("geometry", saveGeometry());
  QMainWindow::closeEvent(e);
}
