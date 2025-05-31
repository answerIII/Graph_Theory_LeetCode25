/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGroupBox *gbLandmarksBasic;
    QGridLayout *gridLayout_2;
    QLineEdit *txtBasicK;
    QLineEdit *txtBasicM;
    QPushButton *btnCreateBasic;
    QComboBox *cmbBasicMethod;
    QGroupBox *gbLandmarksBFS;
    QGridLayout *gridLayout_3;
    QComboBox *cmbBFSMethod;
    QLineEdit *txtBFSK;
    QLineEdit *txtBFSM;
    QPushButton *btnCreateBFS;
    QGroupBox *gbCalculations;
    QGridLayout *gridLayout;
    QLineEdit *txtS;
    QLineEdit *txtT;
    QCheckBox *chkInternalRepr;
    QPushButton *btnExact;
    QTextEdit *txtResults;
    QPushButton *btnApproxBFS;
    QPushButton *btnApproxBasic;
    QTextEdit *textEditStats;
    QTextEdit *textEditDegrees;
    QGroupBox *groupBoxCut;
    QGridLayout *gridLayout_4;
    QComboBox *comboCutMethod;
    QSpinBox *spinCutPercent;
    QPushButton *btnCutRun;
    QGroupBox *groupBoxCreateGraph;
    QGridLayout *gridLayout_5;
    QPushButton *createGraphButton;
    QCheckBox *chkIsDirected;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1140, 594);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gbLandmarksBasic = new QGroupBox(centralwidget);
        gbLandmarksBasic->setObjectName("gbLandmarksBasic");
        gbLandmarksBasic->setGeometry(QRect(20, 10, 311, 135));
        gridLayout_2 = new QGridLayout(gbLandmarksBasic);
        gridLayout_2->setObjectName("gridLayout_2");
        txtBasicK = new QLineEdit(gbLandmarksBasic);
        txtBasicK->setObjectName("txtBasicK");

        gridLayout_2->addWidget(txtBasicK, 1, 0, 1, 1);

        txtBasicM = new QLineEdit(gbLandmarksBasic);
        txtBasicM->setObjectName("txtBasicM");

        gridLayout_2->addWidget(txtBasicM, 2, 0, 1, 1);

        btnCreateBasic = new QPushButton(gbLandmarksBasic);
        btnCreateBasic->setObjectName("btnCreateBasic");

        gridLayout_2->addWidget(btnCreateBasic, 2, 1, 1, 1);

        cmbBasicMethod = new QComboBox(gbLandmarksBasic);
        cmbBasicMethod->addItem(QString());
        cmbBasicMethod->addItem(QString());
        cmbBasicMethod->addItem(QString());
        cmbBasicMethod->setObjectName("cmbBasicMethod");
        cmbBasicMethod->setEditable(false);

        gridLayout_2->addWidget(cmbBasicMethod, 0, 0, 1, 2);

        gbLandmarksBFS = new QGroupBox(centralwidget);
        gbLandmarksBFS->setObjectName("gbLandmarksBFS");
        gbLandmarksBFS->setGeometry(QRect(20, 150, 311, 135));
        gridLayout_3 = new QGridLayout(gbLandmarksBFS);
        gridLayout_3->setObjectName("gridLayout_3");
        cmbBFSMethod = new QComboBox(gbLandmarksBFS);
        cmbBFSMethod->addItem(QString());
        cmbBFSMethod->addItem(QString());
        cmbBFSMethod->addItem(QString());
        cmbBFSMethod->setObjectName("cmbBFSMethod");
        cmbBFSMethod->setEditable(false);

        gridLayout_3->addWidget(cmbBFSMethod, 0, 0, 1, 2);

        txtBFSK = new QLineEdit(gbLandmarksBFS);
        txtBFSK->setObjectName("txtBFSK");

        gridLayout_3->addWidget(txtBFSK, 1, 0, 1, 1);

        txtBFSM = new QLineEdit(gbLandmarksBFS);
        txtBFSM->setObjectName("txtBFSM");

        gridLayout_3->addWidget(txtBFSM, 2, 0, 1, 1);

        btnCreateBFS = new QPushButton(gbLandmarksBFS);
        btnCreateBFS->setObjectName("btnCreateBFS");

        gridLayout_3->addWidget(btnCreateBFS, 2, 1, 1, 1);

        gbCalculations = new QGroupBox(centralwidget);
        gbCalculations->setObjectName("gbCalculations");
        gbCalculations->setGeometry(QRect(20, 300, 311, 231));
        gridLayout = new QGridLayout(gbCalculations);
        gridLayout->setObjectName("gridLayout");
        txtS = new QLineEdit(gbCalculations);
        txtS->setObjectName("txtS");

        gridLayout->addWidget(txtS, 0, 0, 1, 1);

        txtT = new QLineEdit(gbCalculations);
        txtT->setObjectName("txtT");

        gridLayout->addWidget(txtT, 1, 0, 1, 1);

        chkInternalRepr = new QCheckBox(gbCalculations);
        chkInternalRepr->setObjectName("chkInternalRepr");

        gridLayout->addWidget(chkInternalRepr, 2, 0, 1, 2);

        btnExact = new QPushButton(gbCalculations);
        btnExact->setObjectName("btnExact");

        gridLayout->addWidget(btnExact, 3, 0, 1, 1);

        txtResults = new QTextEdit(gbCalculations);
        txtResults->setObjectName("txtResults");
        txtResults->setReadOnly(true);

        gridLayout->addWidget(txtResults, 3, 1, 3, 1);

        btnApproxBFS = new QPushButton(gbCalculations);
        btnApproxBFS->setObjectName("btnApproxBFS");

        gridLayout->addWidget(btnApproxBFS, 4, 0, 1, 1);

        btnApproxBasic = new QPushButton(gbCalculations);
        btnApproxBasic->setObjectName("btnApproxBasic");

        gridLayout->addWidget(btnApproxBasic, 5, 0, 1, 1);

        textEditStats = new QTextEdit(centralwidget);
        textEditStats->setObjectName("textEditStats");
        textEditStats->setGeometry(QRect(380, 50, 381, 311));
        textEditStats->setReadOnly(true);
        textEditDegrees = new QTextEdit(centralwidget);
        textEditDegrees->setObjectName("textEditDegrees");
        textEditDegrees->setGeometry(QRect(800, 50, 311, 461));
        textEditDegrees->setReadOnly(true);
        groupBoxCut = new QGroupBox(centralwidget);
        groupBoxCut->setObjectName("groupBoxCut");
        groupBoxCut->setGeometry(QRect(380, 390, 191, 141));
        gridLayout_4 = new QGridLayout(groupBoxCut);
        gridLayout_4->setObjectName("gridLayout_4");
        comboCutMethod = new QComboBox(groupBoxCut);
        comboCutMethod->addItem(QString());
        comboCutMethod->addItem(QString());
        comboCutMethod->setObjectName("comboCutMethod");

        gridLayout_4->addWidget(comboCutMethod, 0, 0, 1, 2);

        spinCutPercent = new QSpinBox(groupBoxCut);
        spinCutPercent->setObjectName("spinCutPercent");
        spinCutPercent->setMaximum(100);
        spinCutPercent->setValue(10);

        gridLayout_4->addWidget(spinCutPercent, 1, 0, 1, 1);

        btnCutRun = new QPushButton(groupBoxCut);
        btnCutRun->setObjectName("btnCutRun");

        gridLayout_4->addWidget(btnCutRun, 1, 1, 1, 1);

        groupBoxCreateGraph = new QGroupBox(centralwidget);
        groupBoxCreateGraph->setObjectName("groupBoxCreateGraph");
        groupBoxCreateGraph->setGeometry(QRect(590, 410, 182, 101));
        gridLayout_5 = new QGridLayout(groupBoxCreateGraph);
        gridLayout_5->setObjectName("gridLayout_5");
        createGraphButton = new QPushButton(groupBoxCreateGraph);
        createGraphButton->setObjectName("createGraphButton");

        gridLayout_5->addWidget(createGraphButton, 1, 0, 1, 1);

        chkIsDirected = new QCheckBox(groupBoxCreateGraph);
        chkIsDirected->setObjectName("chkIsDirected");

        gridLayout_5->addWidget(chkIsDirected, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1140, 23));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        gbLandmarksBasic->setTitle(QCoreApplication::translate("MainWindow", "Landmarks Basic", nullptr));
        btnCreateBasic->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\320\267\320\264\320\260\321\202\321\214", nullptr));
        cmbBasicMethod->setItemText(0, QCoreApplication::translate("MainWindow", "highest-degree", nullptr));
        cmbBasicMethod->setItemText(1, QCoreApplication::translate("MainWindow", "best-coverage", nullptr));
        cmbBasicMethod->setItemText(2, QCoreApplication::translate("MainWindow", "random", nullptr));

        gbLandmarksBFS->setTitle(QCoreApplication::translate("MainWindow", "Landmarks BFS", nullptr));
        cmbBFSMethod->setItemText(0, QCoreApplication::translate("MainWindow", "highest-degree", nullptr));
        cmbBFSMethod->setItemText(1, QCoreApplication::translate("MainWindow", "best-coverage", nullptr));
        cmbBFSMethod->setItemText(2, QCoreApplication::translate("MainWindow", "random", nullptr));

        btnCreateBFS->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\320\267\320\264\320\260\321\202\321\214", nullptr));
        gbCalculations->setTitle(QCoreApplication::translate("MainWindow", "\320\222\321\213\321\207\320\270\321\201\320\273\320\265\320\275\320\270\321\217", nullptr));
        txtS->setText(QString());
        txtT->setText(QString());
        chkInternalRepr->setText(QCoreApplication::translate("MainWindow", "\320\230\321\201\320\277-\321\202\321\214 \320\262\320\275\321\203\321\202-\320\275\320\265\320\265 \320\277\321\200\320\265\320\264-\320\275\320\270\320\265", nullptr));
        btnExact->setText(QCoreApplication::translate("MainWindow", "\320\242\320\276\321\207\320\275\321\213\320\271 \320\260\320\273\320\263\320\276\321\200\320\270\321\202\320\274", nullptr));
        btnApproxBFS->setText(QCoreApplication::translate("MainWindow", "Landmarks BFS", nullptr));
        btnApproxBasic->setText(QCoreApplication::translate("MainWindow", "Landmarks Basic", nullptr));
        textEditStats->setPlaceholderText(QCoreApplication::translate("MainWindow", "\320\241\321\202\320\260\321\202\320\270\321\201\321\202\320\270\320\272\320\260 \320\263\321\200\320\260\321\204\320\260", nullptr));
        textEditDegrees->setPlaceholderText(QCoreApplication::translate("MainWindow", "\320\241\321\202\320\260\321\202\320\270\321\201\321\202\320\270\320\272\320\260 \321\201\321\202\320\265\320\277\320\265\320\275\320\265\320\271 \320\262\320\265\321\200\321\210\320\270\320\275", nullptr));
        groupBoxCut->setTitle(QCoreApplication::translate("MainWindow", "\320\243\320\264\320\260\320\273\320\265\320\275\320\270\320\265 \320\262\320\265\321\200\321\210\320\270\320\275", nullptr));
        comboCutMethod->setItemText(0, QCoreApplication::translate("MainWindow", "Random_cut", nullptr));
        comboCutMethod->setItemText(1, QCoreApplication::translate("MainWindow", "Max_cut", nullptr));

        btnCutRun->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
        groupBoxCreateGraph->setTitle(QCoreApplication::translate("MainWindow", "\320\241\320\276\320\267\320\264\320\260\321\202\321\214 \320\263\321\200\320\260\321\204", nullptr));
        createGraphButton->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\320\267\320\264\320\260\321\202\321\214 \320\263\321\200\320\260\321\204", nullptr));
        chkIsDirected->setText(QCoreApplication::translate("MainWindow", "\320\236\321\200\320\270\320\265\320\275\321\202\320\270\321\200\320\276\320\262\320\260\320\275\320\275\321\213\320\271", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
