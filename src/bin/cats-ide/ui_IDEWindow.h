/********************************************************************************
** Form generated from reading UI file 'IDEWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IDEWINDOW_H
#define UI_IDEWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_IDEWindow
{
public:
    QAction *action_Open;
    QAction *action_Save_script;
    QAction *action_Exit;
    QAction *actionSwitch_to_Editor;
    QAction *actionSwitch_to_Debugger;
    QAction *actionChange_working_directory;
    QAction *actionRun_script;
    QAction *actionDebug;
    QAction *actionSave_script_as;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QStackedWidget *stackedWidget;
    QWidget *page_5;
    QGridLayout *gridLayout;
    QPlainTextEdit *plainTextEdit;
    QTabWidget *tabWidget;
    QWidget *tab;
    QTextBrowser *textBrowser;
    QWidget *page_6;
    QGridLayout *gridLayout_6;
    QLabel *label;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu_About;
    QMenu *menu_View;
    QMenu *menu_Setup;
    QMenu *menu_Run;
    QStatusBar *statusBar;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *IDEWindow)
    {
        if (IDEWindow->objectName().isEmpty())
            IDEWindow->setObjectName(QStringLiteral("IDEWindow"));
        IDEWindow->resize(1126, 634);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(IDEWindow->sizePolicy().hasHeightForWidth());
        IDEWindow->setSizePolicy(sizePolicy);
        action_Open = new QAction(IDEWindow);
        action_Open->setObjectName(QStringLiteral("action_Open"));
        action_Save_script = new QAction(IDEWindow);
        action_Save_script->setObjectName(QStringLiteral("action_Save_script"));
        action_Exit = new QAction(IDEWindow);
        action_Exit->setObjectName(QStringLiteral("action_Exit"));
        actionSwitch_to_Editor = new QAction(IDEWindow);
        actionSwitch_to_Editor->setObjectName(QStringLiteral("actionSwitch_to_Editor"));
        actionSwitch_to_Editor->setCheckable(true);
        actionSwitch_to_Editor->setChecked(true);
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/editor_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSwitch_to_Editor->setIcon(icon);
        actionSwitch_to_Debugger = new QAction(IDEWindow);
        actionSwitch_to_Debugger->setObjectName(QStringLiteral("actionSwitch_to_Debugger"));
        actionSwitch_to_Debugger->setCheckable(true);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/debugger_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSwitch_to_Debugger->setIcon(icon1);
        actionChange_working_directory = new QAction(IDEWindow);
        actionChange_working_directory->setObjectName(QStringLiteral("actionChange_working_directory"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/working_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionChange_working_directory->setIcon(icon2);
        actionRun_script = new QAction(IDEWindow);
        actionRun_script->setObjectName(QStringLiteral("actionRun_script"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icons/run_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRun_script->setIcon(icon3);
        actionDebug = new QAction(IDEWindow);
        actionDebug->setObjectName(QStringLiteral("actionDebug"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icons/debug_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDebug->setIcon(icon4);
        actionSave_script_as = new QAction(IDEWindow);
        actionSave_script_as->setObjectName(QStringLiteral("actionSave_script_as"));
        centralWidget = new QWidget(IDEWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy1);
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        stackedWidget = new QStackedWidget(centralWidget);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        sizePolicy1.setHeightForWidth(stackedWidget->sizePolicy().hasHeightForWidth());
        stackedWidget->setSizePolicy(sizePolicy1);
        stackedWidget->setBaseSize(QSize(0, 0));
        page_5 = new QWidget();
        page_5->setObjectName(QStringLiteral("page_5"));
        sizePolicy1.setHeightForWidth(page_5->sizePolicy().hasHeightForWidth());
        page_5->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(page_5);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        plainTextEdit = new QPlainTextEdit(page_5);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(2);
        sizePolicy2.setVerticalStretch(3);
        sizePolicy2.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());
        plainTextEdit->setSizePolicy(sizePolicy2);
        plainTextEdit->setSizeIncrement(QSize(0, 0));

        gridLayout->addWidget(plainTextEdit, 0, 0, 1, 1);

        tabWidget = new QTabWidget(page_5);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(2);
        sizePolicy3.setVerticalStretch(1);
        sizePolicy3.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy3);
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        tab->setEnabled(true);
        textBrowser = new QTextBrowser(tab);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(-20, -90, 1063, 277));
        sizePolicy3.setHeightForWidth(textBrowser->sizePolicy().hasHeightForWidth());
        textBrowser->setSizePolicy(sizePolicy3);
        tabWidget->addTab(tab, QString());

        gridLayout->addWidget(tabWidget, 1, 0, 1, 1);

        stackedWidget->addWidget(page_5);
        page_6 = new QWidget();
        page_6->setObjectName(QStringLiteral("page_6"));
        sizePolicy1.setHeightForWidth(page_6->sizePolicy().hasHeightForWidth());
        page_6->setSizePolicy(sizePolicy1);
        gridLayout_6 = new QGridLayout(page_6);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        label = new QLabel(page_6);
        label->setObjectName(QStringLiteral("label"));
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        QFont font;
        font.setPointSize(14);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout_6->addWidget(label, 0, 0, 1, 1);

        stackedWidget->addWidget(page_6);

        verticalLayout->addWidget(stackedWidget);

        IDEWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(IDEWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1126, 26));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        menu_About = new QMenu(menuBar);
        menu_About->setObjectName(QStringLiteral("menu_About"));
        menu_View = new QMenu(menuBar);
        menu_View->setObjectName(QStringLiteral("menu_View"));
        menu_Setup = new QMenu(menuBar);
        menu_Setup->setObjectName(QStringLiteral("menu_Setup"));
        menu_Run = new QMenu(menuBar);
        menu_Run->setObjectName(QStringLiteral("menu_Run"));
        IDEWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(IDEWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        IDEWindow->setStatusBar(statusBar);
        mainToolBar = new QToolBar(IDEWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        IDEWindow->addToolBar(Qt::LeftToolBarArea, mainToolBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_View->menuAction());
        menuBar->addAction(menu_Setup->menuAction());
        menuBar->addAction(menu_Run->menuAction());
        menuBar->addAction(menu_About->menuAction());
        menu_File->addAction(action_Open);
        menu_File->addAction(action_Save_script);
        menu_File->addAction(actionSave_script_as);
        menu_File->addAction(action_Exit);
        menu_View->addAction(actionSwitch_to_Editor);
        menu_View->addAction(actionSwitch_to_Debugger);
        menu_Setup->addAction(actionChange_working_directory);
        menu_Run->addAction(actionRun_script);
        menu_Run->addAction(actionDebug);
        mainToolBar->addAction(actionSwitch_to_Editor);
        mainToolBar->addAction(actionSwitch_to_Debugger);
        mainToolBar->addAction(actionChange_working_directory);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionRun_script);
        mainToolBar->addAction(actionDebug);

        retranslateUi(IDEWindow);

        stackedWidget->setCurrentIndex(0);
        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(IDEWindow);
    } // setupUi

    void retranslateUi(QMainWindow *IDEWindow)
    {
        IDEWindow->setWindowTitle(QApplication::translate("IDEWindow", "MainWindow", 0));
        action_Open->setText(QApplication::translate("IDEWindow", "&Load script...", 0));
        action_Save_script->setText(QApplication::translate("IDEWindow", "&Save script...", 0));
        action_Save_script->setShortcut(QApplication::translate("IDEWindow", "Ctrl+S", 0));
        action_Exit->setText(QApplication::translate("IDEWindow", "&Exit", 0));
        actionSwitch_to_Editor->setText(QApplication::translate("IDEWindow", "Editor", 0));
#ifndef QT_NO_TOOLTIP
        actionSwitch_to_Editor->setToolTip(QApplication::translate("IDEWindow", "Switch to Editor", 0));
#endif // QT_NO_TOOLTIP
        actionSwitch_to_Debugger->setText(QApplication::translate("IDEWindow", "Debugger", 0));
#ifndef QT_NO_TOOLTIP
        actionSwitch_to_Debugger->setToolTip(QApplication::translate("IDEWindow", "Switch to Debugger", 0));
#endif // QT_NO_TOOLTIP
        actionChange_working_directory->setText(QApplication::translate("IDEWindow", "Change working directory...", 0));
        actionRun_script->setText(QApplication::translate("IDEWindow", "Run", 0));
        actionRun_script->setShortcut(QApplication::translate("IDEWindow", "Ctrl+R", 0));
        actionDebug->setText(QApplication::translate("IDEWindow", "Debug", 0));
        actionSave_script_as->setText(QApplication::translate("IDEWindow", "Save script as...", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("IDEWindow", "Output", 0));
        label->setText(QApplication::translate("IDEWindow", "No debugger has been launched yet.", 0));
        menu_File->setTitle(QApplication::translate("IDEWindow", "&File", 0));
        menu_About->setTitle(QApplication::translate("IDEWindow", "&About", 0));
        menu_View->setTitle(QApplication::translate("IDEWindow", "&View", 0));
        menu_Setup->setTitle(QApplication::translate("IDEWindow", "&Settings", 0));
        menu_Run->setTitle(QApplication::translate("IDEWindow", "&Run", 0));
    } // retranslateUi

};

namespace Ui {
    class IDEWindow: public Ui_IDEWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IDEWINDOW_H
