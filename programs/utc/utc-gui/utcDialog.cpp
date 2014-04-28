#include <QtWidgets>
#include "utcDialog.h"

extern "C" {
char* convertTimestamp(uint ts, int tz);
}

utcDialog::utcDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    convertButton->setEnabled(false);
}

void utcDialog::on_tsLineEdit_textChanged()
{
    convertButton->setEnabled(is_tsLineEdit_valid());
}

void utcDialog::on_convertButton_clicked()
{
    QString str(convertTimestamp(timestamp, tzSpinBox->value()));
    outTextEdit->setPlainText(str);    
}

void utcDialog::on_convertButton2_clicked()
{
    uint ts = dateTimeEdit->dateTime().toTime_t();
    int tz = tzSpinBox2->value();
    if(tz) {
        ts += (-1) * tz * 3600;
    }
    QString str;
    str = QString("%1").arg(ts);
    outTextEdit2->setPlainText(str);
}

bool utcDialog::is_tsLineEdit_valid()
{
    bool ok;

    timestamp = tsLineEdit->text().toUInt(&ok, 10);
    return ok;
}
