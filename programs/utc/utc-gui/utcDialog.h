#ifndef UTCDIALOG_H
#define UTCDIALOG_H

#include <QDialog>
#include "ui_utcDialog.h"

class utcDialog : public QDialog, public Ui::utcDialog
{
    Q_OBJECT

public:
    utcDialog(QWidget *parent = 0);

private slots:
    void on_tsLineEdit_textChanged();
    void on_convertButton_clicked();
    void on_convertButton2_clicked();

private:
    bool is_tsLineEdit_valid();

    uint timestamp;
};

#endif
