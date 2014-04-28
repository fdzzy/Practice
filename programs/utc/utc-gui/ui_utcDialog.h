/********************************************************************************
** Form generated from reading UI file 'utcDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UTCDIALOG_H
#define UI_UTCDIALOG_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_utcDialog
{
public:
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_6;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *tsLineEdit;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QSpinBox *tzSpinBox;
    QSpacerItem *horizontalSpacer;
    QTextEdit *outTextEdit;
    QVBoxLayout *verticalLayout;
    QPushButton *convertButton;
    QPushButton *exitButton;
    QSpacerItem *verticalSpacer;
    QWidget *tab_2;
    QLabel *label_8;
    QWidget *widget1;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_9;
    QDateTimeEdit *dateTimeEdit;
    QLabel *label_7;
    QSpinBox *tzSpinBox2;
    QHBoxLayout *horizontalLayout_4;
    QTextEdit *outTextEdit2;
    QVBoxLayout *verticalLayout_3;
    QPushButton *convertButton2;
    QPushButton *exitButton2;
    QSpacerItem *verticalSpacer_3;

    void setupUi(QDialog *utcDialog)
    {
        if (utcDialog->objectName().isEmpty())
            utcDialog->setObjectName(QStringLiteral("utcDialog"));
        utcDialog->resize(507, 279);
        tabWidget = new QTabWidget(utcDialog);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(20, 10, 451, 251));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        widget = new QWidget(tab);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(10, 20, 421, 171));
        horizontalLayout_6 = new QHBoxLayout(widget);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        tsLineEdit = new QLineEdit(widget);
        tsLineEdit->setObjectName(QStringLiteral("tsLineEdit"));

        horizontalLayout->addWidget(tsLineEdit);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(widget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        tzSpinBox = new QSpinBox(widget);
        tzSpinBox->setObjectName(QStringLiteral("tzSpinBox"));
        tzSpinBox->setMinimum(-12);
        tzSpinBox->setMaximum(12);

        horizontalLayout_2->addWidget(tzSpinBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout_2);

        outTextEdit = new QTextEdit(widget);
        outTextEdit->setObjectName(QStringLiteral("outTextEdit"));
        outTextEdit->setEnabled(true);
        QFont font;
        font.setPointSize(15);
        font.setBold(true);
        font.setWeight(75);
        outTextEdit->setFont(font);
        outTextEdit->setReadOnly(true);

        verticalLayout_2->addWidget(outTextEdit);


        horizontalLayout_6->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        convertButton = new QPushButton(widget);
        convertButton->setObjectName(QStringLiteral("convertButton"));

        verticalLayout->addWidget(convertButton);

        exitButton = new QPushButton(widget);
        exitButton->setObjectName(QStringLiteral("exitButton"));

        verticalLayout->addWidget(exitButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout_6->addLayout(verticalLayout);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        label_8 = new QLabel(tab_2);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(432, -227, 71, 27));
        widget1 = new QWidget(tab_2);
        widget1->setObjectName(QStringLiteral("widget1"));
        widget1->setGeometry(QRect(20, 20, 411, 171));
        verticalLayout_4 = new QVBoxLayout(widget1);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_9 = new QLabel(widget1);
        label_9->setObjectName(QStringLiteral("label_9"));

        horizontalLayout_3->addWidget(label_9);

        dateTimeEdit = new QDateTimeEdit(widget1);
        dateTimeEdit->setObjectName(QStringLiteral("dateTimeEdit"));
        dateTimeEdit->setMinimumSize(QSize(160, 0));
        dateTimeEdit->setBaseSize(QSize(10, 0));
        dateTimeEdit->setDateTime(QDateTime(QDate(2014, 1, 1), QTime(8, 0, 0)));
        dateTimeEdit->setDate(QDate(2014, 1, 1));
        dateTimeEdit->setTime(QTime(8, 0, 0));
        dateTimeEdit->setMaximumDateTime(QDateTime(QDate(2099, 1, 1), QTime(22, 59, 59)));
        dateTimeEdit->setMinimumDateTime(QDateTime(QDate(1970, 1, 1), QTime(8, 0, 0)));
        dateTimeEdit->setMaximumDate(QDate(2099, 1, 1));
        dateTimeEdit->setMinimumDate(QDate(1970, 1, 1));
        dateTimeEdit->setMaximumTime(QTime(22, 59, 59));
        dateTimeEdit->setMinimumTime(QTime(8, 0, 0));
        dateTimeEdit->setCurrentSection(QDateTimeEdit::MonthSection);
        dateTimeEdit->setCalendarPopup(true);
        dateTimeEdit->setCurrentSectionIndex(0);
        dateTimeEdit->setTimeSpec(Qt::UTC);

        horizontalLayout_3->addWidget(dateTimeEdit);

        label_7 = new QLabel(widget1);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout_3->addWidget(label_7);

        tzSpinBox2 = new QSpinBox(widget1);
        tzSpinBox2->setObjectName(QStringLiteral("tzSpinBox2"));
        tzSpinBox2->setMinimum(-12);
        tzSpinBox2->setMaximum(12);

        horizontalLayout_3->addWidget(tzSpinBox2);


        verticalLayout_4->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        outTextEdit2 = new QTextEdit(widget1);
        outTextEdit2->setObjectName(QStringLiteral("outTextEdit2"));
        outTextEdit2->setEnabled(true);
        outTextEdit2->setFont(font);
        outTextEdit2->setReadOnly(true);

        horizontalLayout_4->addWidget(outTextEdit2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        convertButton2 = new QPushButton(widget1);
        convertButton2->setObjectName(QStringLiteral("convertButton2"));

        verticalLayout_3->addWidget(convertButton2);

        exitButton2 = new QPushButton(widget1);
        exitButton2->setObjectName(QStringLiteral("exitButton2"));

        verticalLayout_3->addWidget(exitButton2);

        verticalSpacer_3 = new QSpacerItem(82, 48, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_3);


        horizontalLayout_4->addLayout(verticalLayout_3);


        verticalLayout_4->addLayout(horizontalLayout_4);

        tabWidget->addTab(tab_2, QString());
        label_8->raise();
        outTextEdit2->raise();
        convertButton2->raise();
        exitButton2->raise();
        label_9->raise();
        dateTimeEdit->raise();
        label_7->raise();
        tzSpinBox2->raise();
#ifndef QT_NO_SHORTCUT
        label->setBuddy(tsLineEdit);
        label_2->setBuddy(tzSpinBox);
        label_8->setBuddy(tzSpinBox);
        label_9->setBuddy(tsLineEdit);
        label_7->setBuddy(tzSpinBox);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(tsLineEdit, tzSpinBox);
        QWidget::setTabOrder(tzSpinBox, convertButton);
        QWidget::setTabOrder(convertButton, exitButton);
        QWidget::setTabOrder(exitButton, tabWidget);
        QWidget::setTabOrder(tabWidget, outTextEdit);
        QWidget::setTabOrder(outTextEdit, dateTimeEdit);
        QWidget::setTabOrder(dateTimeEdit, tzSpinBox2);
        QWidget::setTabOrder(tzSpinBox2, outTextEdit2);
        QWidget::setTabOrder(outTextEdit2, convertButton2);
        QWidget::setTabOrder(convertButton2, exitButton2);

        retranslateUi(utcDialog);
        QObject::connect(exitButton, SIGNAL(clicked()), utcDialog, SLOT(reject()));
        QObject::connect(exitButton2, SIGNAL(clicked()), utcDialog, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(utcDialog);
    } // setupUi

    void retranslateUi(QDialog *utcDialog)
    {
        utcDialog->setWindowTitle(QApplication::translate("utcDialog", "Timestamp conversion", 0));
        label->setText(QApplication::translate("utcDialog", "Timestamp:", 0));
        label_2->setText(QApplication::translate("utcDialog", "Timezone:", 0));
        convertButton->setText(QApplication::translate("utcDialog", "Convert", 0));
        exitButton->setText(QApplication::translate("utcDialog", "Exit", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("utcDialog", "Timestamp to Date", 0));
        label_8->setText(QApplication::translate("utcDialog", "Timezone:", 0));
        label_9->setText(QApplication::translate("utcDialog", "Date/Time:", 0));
        dateTimeEdit->setDisplayFormat(QApplication::translate("utcDialog", "MM/dd/yyyy hh:mm", 0));
        label_7->setText(QApplication::translate("utcDialog", "Timezone:", 0));
        convertButton2->setText(QApplication::translate("utcDialog", "Convert", 0));
        exitButton2->setText(QApplication::translate("utcDialog", "Exit", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("utcDialog", "Date to Timestamp", 0));
    } // retranslateUi

};

namespace Ui {
    class utcDialog: public Ui_utcDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UTCDIALOG_H
