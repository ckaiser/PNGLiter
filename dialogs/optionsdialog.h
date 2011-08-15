#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
    class OptionsDialog;
}

class OptionsDialog : public QDialog {
    Q_OBJECT
public:
    OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();

private slots:
    void saveOptions();
    void browse();
    void openUrl(QString url);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::OptionsDialog *ui;
};

#endif // OPTIONSDIALOG_H
