#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QDesktopServices>
#include <QUrl>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->versionLabel->setText(qApp->applicationVersion());

    connect(ui->mainLabel, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
    connect(ui->qtLabel, SIGNAL(linkActivated(QString)), qApp, SLOT(aboutQt()));
    connect(ui->urlLabel, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::openUrl(QString url)
{
  QDesktopServices::openUrl(QUrl(url));
}

void AboutDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
