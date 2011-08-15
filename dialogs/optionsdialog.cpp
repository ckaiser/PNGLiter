#include "optionsdialog.h"
#include "../tools/qtwin.h"
#include "ui_optionsdialog.h"

#include <QSettings>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
  ui->setupUi(this);

  if (QtWin::isCompositionEnabled() && QtWin::extendFrameIntoClientArea(this)) {
    layout()->setMargin(2);
  }

  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveOptions()));
  connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browse()));
  connect(ui->advancedGroupBox, SIGNAL(toggled(bool)), ui->generalTab, SLOT(setDisabled(bool)));
  connect(ui->advancedLabel, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));

  // Loading
  QSettings settings;
  settings.beginGroup("options");
    ui->levelSlider->setValue(settings.value("png/optimizationLevel", 3).toInt());
    ui->errorRecoveryCheckBox->setChecked(settings.value("png/errorRecovery").toBool());
    ui->backupCheckBox->setChecked(settings.value("png/backup").toBool());

    ui->outputGroupBox->setChecked(settings.value("output").toBool());
    ui->directoryEdit->setText(settings.value("directory").toString());

    ui->advancedGroupBox->setChecked(settings.value("advanced/enabled").toBool());
    ui->argumentsEdit->setText(settings.value("advanced/arguments", "-o3 %1").toString());
  settings.endGroup();

  if (ui->advancedGroupBox->isChecked())
    ui->tabWidget->setCurrentIndex(1);
}

OptionsDialog::~OptionsDialog()
{
  delete ui;
}

void OptionsDialog::saveOptions()
{
  // Loading
  QSettings settings;

  settings.beginGroup("options");
    settings.setValue("png/optimizationLevel", ui->levelSlider->value());
    settings.setValue("png/errorRecovery"    ,  ui->errorRecoveryCheckBox->isChecked());
    settings.setValue("png/backup"           , ui->backupCheckBox->isChecked());

    settings.setValue("output"   , ui->outputGroupBox->isChecked());
    settings.setValue("directory", ui->directoryEdit->text());

    settings.setValue("advanced/enabled", ui->advancedGroupBox->isChecked());
    settings.setValue("advanced/arguments", ui->argumentsEdit->text());
  settings.endGroup();
}

void OptionsDialog::browse()
{
  QString dir = QFileDialog::getExistingDirectory(this, tr("Choose where to save the result PNGs"), ui->directoryEdit->text());

  if (!dir.isEmpty())
    ui->directoryEdit->setText(dir);
}

void OptionsDialog::openUrl(QString url)
{
  QDesktopServices::openUrl(QUrl(url));
}

void OptionsDialog::changeEvent(QEvent *e)
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
