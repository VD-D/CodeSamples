/********************************************************************************
** Form generated from reading UI file 'findreplace.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FINDREPLACE_H
#define UI_FINDREPLACE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FindReplace
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLineEdit *lineEditReplace;
    QLabel *labelReplace;
    QLineEdit *lineEditFind;
    QLabel *labelFind;
    QHBoxLayout *buttonsLayout;
    QPushButton *replaceNextButton;
    QPushButton *replaceAllButton;
    QHBoxLayout *optionsLayout;
    QCheckBox *wholeWordCheck;
    QCheckBox *caseSensitiveCheck;
    QLabel *searchKeyReplaced;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *FindReplace)
    {
        if (FindReplace->objectName().isEmpty())
            FindReplace->setObjectName("FindReplace");
        FindReplace->resize(439, 190);
        verticalLayoutWidget = new QWidget(FindReplace);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(10, 10, 421, 212));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(10);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(10, 5, 10, 5);
        lineEditReplace = new QLineEdit(verticalLayoutWidget);
        lineEditReplace->setObjectName("lineEditReplace");

        gridLayout->addWidget(lineEditReplace, 2, 1, 1, 1);

        labelReplace = new QLabel(verticalLayoutWidget);
        labelReplace->setObjectName("labelReplace");

        gridLayout->addWidget(labelReplace, 2, 0, 1, 1);

        lineEditFind = new QLineEdit(verticalLayoutWidget);
        lineEditFind->setObjectName("lineEditFind");

        gridLayout->addWidget(lineEditFind, 1, 1, 1, 1);

        labelFind = new QLabel(verticalLayoutWidget);
        labelFind->setObjectName("labelFind");

        gridLayout->addWidget(labelFind, 1, 0, 1, 1);


        verticalLayout->addLayout(gridLayout);

        buttonsLayout = new QHBoxLayout();
        buttonsLayout->setObjectName("buttonsLayout");
        replaceNextButton = new QPushButton(verticalLayoutWidget);
        replaceNextButton->setObjectName("replaceNextButton");

        buttonsLayout->addWidget(replaceNextButton);

        replaceAllButton = new QPushButton(verticalLayoutWidget);
        replaceAllButton->setObjectName("replaceAllButton");

        buttonsLayout->addWidget(replaceAllButton);


        verticalLayout->addLayout(buttonsLayout);

        optionsLayout = new QHBoxLayout();
        optionsLayout->setObjectName("optionsLayout");
        optionsLayout->setContentsMargins(10, -1, 20, -1);
        wholeWordCheck = new QCheckBox(verticalLayoutWidget);
        wholeWordCheck->setObjectName("wholeWordCheck");

        optionsLayout->addWidget(wholeWordCheck);

        caseSensitiveCheck = new QCheckBox(verticalLayoutWidget);
        caseSensitiveCheck->setObjectName("caseSensitiveCheck");

        optionsLayout->addWidget(caseSensitiveCheck);


        verticalLayout->addLayout(optionsLayout);

        searchKeyReplaced = new QLabel(verticalLayoutWidget);
        searchKeyReplaced->setObjectName("searchKeyReplaced");

        verticalLayout->addWidget(searchKeyReplaced);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        QWidget::setTabOrder(lineEditFind, lineEditReplace);
        QWidget::setTabOrder(lineEditReplace, replaceNextButton);
        QWidget::setTabOrder(replaceNextButton, replaceAllButton);

        retranslateUi(FindReplace);

        QMetaObject::connectSlotsByName(FindReplace);
    } // setupUi

    void retranslateUi(QDialog *FindReplace)
    {
        FindReplace->setWindowTitle(QCoreApplication::translate("FindReplace", "Find and Replace", nullptr));
        labelReplace->setText(QCoreApplication::translate("FindReplace", "Replace:", nullptr));
        labelFind->setText(QCoreApplication::translate("FindReplace", "Find:", nullptr));
        replaceNextButton->setText(QCoreApplication::translate("FindReplace", "Replace Next", nullptr));
        replaceAllButton->setText(QCoreApplication::translate("FindReplace", "Replace All", nullptr));
        wholeWordCheck->setText(QCoreApplication::translate("FindReplace", "Match Whole Word", nullptr));
        caseSensitiveCheck->setText(QCoreApplication::translate("FindReplace", "Case Sensitive", nullptr));
        searchKeyReplaced->setText(QCoreApplication::translate("FindReplace", "-", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FindReplace: public Ui_FindReplace {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FINDREPLACE_H
