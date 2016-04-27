/********************************************************************************
** Form generated from reading UI file 'IDEWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSplitter>
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
    QAction *actionAbout;
    QAction *actionCATs_Help;
    QAction *actionJavaScript_Help;
    QAction *actionReference;
    QAction *actionAbort;
    QAction *actionAutoSet_WD_to_script_path;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QStackedWidget *stackedWidget;
    QWidget *page_5;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    QPlainTextEdit *plainTextEdit;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QLabel *workingDirLabel;
    QTextBrowser *textBrowser;
    QWidget *page_2;
    QVBoxLayout *verticalLayout_4;
    QSplitter *splitter_2;
    QLabel *label;
    QTextBrowser *textBrowser_2;
    QWidget *page;
    QGridLayout *gridLayout;
    QLabel *label_2;
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
        IDEWindow->resize(1137, 735);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(IDEWindow->sizePolicy().hasHeightForWidth());
        IDEWindow->setSizePolicy(sizePolicy);
        action_Open = new QAction(IDEWindow);
        action_Open->setObjectName(QStringLiteral("action_Open"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Open->setIcon(icon);
        action_Save_script = new QAction(IDEWindow);
        action_Save_script->setObjectName(QStringLiteral("action_Save_script"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Save_script->setIcon(icon1);
        action_Exit = new QAction(IDEWindow);
        action_Exit->setObjectName(QStringLiteral("action_Exit"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Exit->setIcon(icon2);
        actionSwitch_to_Editor = new QAction(IDEWindow);
        actionSwitch_to_Editor->setObjectName(QStringLiteral("actionSwitch_to_Editor"));
        actionSwitch_to_Editor->setCheckable(true);
        actionSwitch_to_Editor->setChecked(true);
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icons/editor_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSwitch_to_Editor->setIcon(icon3);
        actionSwitch_to_Debugger = new QAction(IDEWindow);
        actionSwitch_to_Debugger->setObjectName(QStringLiteral("actionSwitch_to_Debugger"));
        actionSwitch_to_Debugger->setCheckable(true);
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icons/debugger_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSwitch_to_Debugger->setIcon(icon4);
        actionChange_working_directory = new QAction(IDEWindow);
        actionChange_working_directory->setObjectName(QStringLiteral("actionChange_working_directory"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/icons/working_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionChange_working_directory->setIcon(icon5);
        actionRun_script = new QAction(IDEWindow);
        actionRun_script->setObjectName(QStringLiteral("actionRun_script"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/icons/run_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRun_script->setIcon(icon6);
        actionDebug = new QAction(IDEWindow);
        actionDebug->setObjectName(QStringLiteral("actionDebug"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/icons/debug_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDebug->setIcon(icon7);
        actionSave_script_as = new QAction(IDEWindow);
        actionSave_script_as->setObjectName(QStringLiteral("actionSave_script_as"));
        actionSave_script_as->setIcon(icon1);
        actionAbout = new QAction(IDEWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionCATs_Help = new QAction(IDEWindow);
        actionCATs_Help->setObjectName(QStringLiteral("actionCATs_Help"));
        actionJavaScript_Help = new QAction(IDEWindow);
        actionJavaScript_Help->setObjectName(QStringLiteral("actionJavaScript_Help"));
        actionReference = new QAction(IDEWindow);
        actionReference->setObjectName(QStringLiteral("actionReference"));
        actionReference->setCheckable(true);
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/icons/help.jpg"), QSize(), QIcon::Normal, QIcon::On);
        actionReference->setIcon(icon8);
        actionAbort = new QAction(IDEWindow);
        actionAbort->setObjectName(QStringLiteral("actionAbort"));
        actionAbort->setEnabled(false);
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/icons/abort.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAbort->setIcon(icon9);
        actionAutoSet_WD_to_script_path = new QAction(IDEWindow);
        actionAutoSet_WD_to_script_path->setObjectName(QStringLiteral("actionAutoSet_WD_to_script_path"));
        actionAutoSet_WD_to_script_path->setCheckable(true);
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
        horizontalLayout = new QHBoxLayout(page_5);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        splitter = new QSplitter(page_5);
        splitter->setObjectName(QStringLiteral("splitter"));
        sizePolicy1.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy1);
        splitter->setOrientation(Qt::Vertical);
        plainTextEdit = new QPlainTextEdit(splitter);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(2);
        sizePolicy2.setVerticalStretch(3);
        sizePolicy2.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());
        plainTextEdit->setSizePolicy(sizePolicy2);
        plainTextEdit->setSizeIncrement(QSize(0, 0));
        splitter->addWidget(plainTextEdit);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(2);
        sizePolicy3.setVerticalStretch(1);
        sizePolicy3.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy3);
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        tab->setEnabled(true);
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        workingDirLabel = new QLabel(tab);
        workingDirLabel->setObjectName(QStringLiteral("workingDirLabel"));
        QFont font;
        font.setPointSize(9);
        workingDirLabel->setFont(font);

        verticalLayout_2->addWidget(workingDirLabel);

        textBrowser = new QTextBrowser(tab);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(2);
        sizePolicy4.setVerticalStretch(2);
        sizePolicy4.setHeightForWidth(textBrowser->sizePolicy().hasHeightForWidth());
        textBrowser->setSizePolicy(sizePolicy4);
        QFont font1;
        font1.setFamily(QStringLiteral("Courier New"));
        font1.setPointSize(10);
        textBrowser->setFont(font1);

        verticalLayout_2->addWidget(textBrowser);

        tabWidget->addTab(tab, QString());
        splitter->addWidget(tabWidget);

        horizontalLayout->addWidget(splitter);

        stackedWidget->addWidget(page_5);
        page_2 = new QWidget();
        page_2->setObjectName(QStringLiteral("page_2"));
        verticalLayout_4 = new QVBoxLayout(page_2);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        splitter_2 = new QSplitter(page_2);
        splitter_2->setObjectName(QStringLiteral("splitter_2"));
        splitter_2->setOrientation(Qt::Vertical);
        label = new QLabel(splitter_2);
        label->setObjectName(QStringLiteral("label"));
        sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy2);
        splitter_2->addWidget(label);
        textBrowser_2 = new QTextBrowser(splitter_2);
        textBrowser_2->setObjectName(QStringLiteral("textBrowser_2"));
        sizePolicy3.setHeightForWidth(textBrowser_2->sizePolicy().hasHeightForWidth());
        textBrowser_2->setSizePolicy(sizePolicy3);
        textBrowser_2->setFont(font1);
        splitter_2->addWidget(textBrowser_2);

        verticalLayout_4->addWidget(splitter_2);

        stackedWidget->addWidget(page_2);
        page = new QWidget();
        page->setObjectName(QStringLiteral("page"));
        gridLayout = new QGridLayout(page);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_2 = new QLabel(page);
        label_2->setObjectName(QStringLiteral("label_2"));
        QFont font2;
        font2.setPointSize(11);
        label_2->setFont(font2);
        label_2->setScaledContents(false);
        label_2->setAlignment(Qt::AlignCenter);
        label_2->setWordWrap(false);

        gridLayout->addWidget(label_2, 0, 0, 1, 1, Qt::AlignHCenter|Qt::AlignTop);

        stackedWidget->addWidget(page);

        verticalLayout->addWidget(stackedWidget);

        IDEWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(IDEWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1137, 26));
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
        menu_About->addAction(actionAbout);
        menu_About->addAction(actionCATs_Help);
        menu_About->addAction(actionJavaScript_Help);
        menu_View->addAction(actionSwitch_to_Editor);
        menu_View->addAction(actionSwitch_to_Debugger);
        menu_Setup->addAction(actionChange_working_directory);
        menu_Setup->addAction(actionAutoSet_WD_to_script_path);
        menu_Run->addAction(actionRun_script);
        menu_Run->addAction(actionDebug);
        menu_Run->addAction(actionAbort);
        mainToolBar->addAction(actionSwitch_to_Editor);
        mainToolBar->addAction(actionSwitch_to_Debugger);
        mainToolBar->addAction(actionChange_working_directory);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionRun_script);
        mainToolBar->addAction(actionDebug);
        mainToolBar->addAction(actionAbort);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionReference);

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
        actionAbout->setText(QApplication::translate("IDEWindow", "About", 0));
        actionCATs_Help->setText(QApplication::translate("IDEWindow", "CATs Help", 0));
        actionJavaScript_Help->setText(QApplication::translate("IDEWindow", "JavaScript Help", 0));
        actionReference->setText(QApplication::translate("IDEWindow", "Reference", 0));
#ifndef QT_NO_TOOLTIP
        actionReference->setToolTip(QApplication::translate("IDEWindow", "Web Browser for CATs and JavaScript reference", 0));
#endif // QT_NO_TOOLTIP
        actionAbort->setText(QApplication::translate("IDEWindow", "Abort", 0));
        actionAutoSet_WD_to_script_path->setText(QApplication::translate("IDEWindow", "Script location is the working directory", 0));
        workingDirLabel->setText(QApplication::translate("IDEWindow", "Working directory: ", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("IDEWindow", "Output", 0));
        label->setText(QString());
        label_2->setText(QApplication::translate("IDEWindow", "CATs IDE\n"
"\n"
"\n"
"CATs developed by RNDr. Petr Kulh\303\241nek, PhD. at NCBR.\n"
"This CATs IDE application has been developed by Mgr. Jaroslav O\304\276ha.\n"
"Licensed under GNU General Public License.", 0));
        menu_File->setTitle(QApplication::translate("IDEWindow", "&File", 0));
        menu_About->setTitle(QApplication::translate("IDEWindow", "&Help", 0));
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
