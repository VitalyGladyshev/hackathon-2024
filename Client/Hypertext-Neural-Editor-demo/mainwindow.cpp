/************************************************
* Команда 2
* Гипертекстовый редактор
*
* Код класса главного окна текстового редактора
************************************************/

#include <QtWidgets>
#include <QMdiArea>
#include <QAction>
#include <QPrintDialog>
#include <QPixmap>
#include <QColor>
#include <QPagedPaintDevice>
#include <QPrintPreviewDialog>
#include <QPalette>

#include "mainwindow.h"
#include "documentwindow.h"
#include "settings.h"
#include "finddialog.h"
#include "hyperlinkdialog.h"
#include "helpviewdialog.h"
#include "aichat.h"

MainWindow::MainWindow(QWidget *parent /* = nullptr */)
    : QMainWindow(parent),
    _iUnnamedIndex(0),
    _pFindDialog(nullptr),
    _pMakeLinkDialog(nullptr),
    _pShowHelpDialog(nullptr)
{
    // Создаём объекты действий
    CreateActions();

    // Создаём пункт меню "Файл" главного окна
    _pmnuFile = new QMenu(tr("&File"));
    _pmnuFile->addAction(_pNewAct);
    _pmnuFile->addAction(_pOpenAct);
    _pmnuFile->addAction(_pSaveAct);
    _pmnuFile->addAction(_pSaveAsAct);
    _pmnuFile->addSeparator();
    _pmnuFile->addAction(_pPrintAct);
    _pmnuFile->addAction(_pPrintPreviewAct);
    _pmnuFile->addAction(_pSaveAsOdt);
    _pmnuFile->addAction(_pPrintPDFAct);
    _pmnuFile->addSeparator();
    _pmnuFile->addAction(_pQuitAct);
    menuBar()->addMenu(_pmnuFile);

    // Создаём пункт меню "Правка" главного окна
    _pMenuEdit = new QMenu(tr("&Edit"));
    _pMenuEdit->addAction(_pCutAct);
    _pMenuEdit->addAction(_pCopyAct);
    _pMenuEdit->addAction(_pPasteAct);
    _pMenuEdit->addSeparator();
    _pMenuEdit->addAction(_pUndoAct);
    _pMenuEdit->addAction(_pRedoAct);
    _pMenuEdit->addSeparator();
    _pMenuEdit->addAction(_pMakeLinkAct);
    _pMenuEdit->addAction(_pAddImageAct);
    _pMenuEdit->addSeparator();
    _pMenuEdit->addAction(_pFindAct);
    menuBar()->addMenu(_pMenuEdit);

    // Создаём пункт меню "Форматирование" главного окна
    _pMenuFormat = new QMenu(tr("F&ormat"));
    SetupFormatActions(_pMenuFormat);
    menuBar()->addMenu(_pMenuFormat);

    // Создаём пункт меню "Вкладки" главного окна
    _pMenuWindows = new QMenu(tr("&Tabs"));
    menuBar()->addMenu(_pMenuWindows);
    connect(_pMenuWindows, SIGNAL(aboutToShow()), SLOT(SlotWindows()));

    // Создаём пункт меню "Настройки" главного окна
    _pMenuSettings = new QMenu(tr("&Settings"));
    SetupLanguageActions(_pMenuSettings);
    _pMenuSettings->addSeparator();
    SetupThemeActions(_pMenuSettings);
    menuBar()->addMenu(_pMenuSettings);

    // Создаём пункт меню "Помощь" главного окна
    _pMenuHelp = new QMenu(tr("&Help"));
    _pMenuHelp->addAction(_pHelpAct);
    _pMenuHelp->addAction(_pAboutAct);
    menuBar()->addMenu(_pMenuHelp);

    // Cоздание виджета MDI
    _pMdiArea = new QMdiArea(this);
    _pMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _pMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _pMdiArea->setTabsClosable(true);
    setCentralWidget(_pMdiArea);
    connect(_pMdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(SlotUpdateMenus()));
    connect(_pNextAct, &QAction::triggered,
            _pMdiArea, &QMdiArea::activateNextSubWindow);
    connect(_pPreviousAct, &QAction::triggered,
            _pMdiArea, &QMdiArea::activatePreviousSubWindow);

    // Создание действия закрыть дочернее окно
    _pCloseAct = new QAction(tr("&Close"), this);
    connect(_pCloseAct, &QAction::triggered,
            _pMdiArea, &QMdiArea::closeActiveSubWindow);

    // Создание действия закрыть все дочерние окна
    _pCloseAllAct = new QAction(tr("Close &All"), this);
    connect(_pCloseAllAct, &QAction::triggered,
            _pMdiArea, &QMdiArea::closeAllSubWindows);

    //Создаём мапер сигналов дочених окон
    _pSignalMapper = new QSignalMapper(this);
    connect(_pSignalMapper,
            SIGNAL(mappedObject(QObject*)),
            this,
            SLOT(SlotSetActiveSubWindow(QObject*)));
        
    // us2_t-001 Спринт 1 Алексей: Реализовать тулбар
    _pToolBar = new QToolBar(this);
    _pToolBar->addAction(_pNewAct);
    _pToolBar->addAction(_pOpenAct);
    _pToolBar->addAction(_pSaveAct);
    _pToolBar->addAction(_pSaveAsAct);
    _pToolBar->addSeparator();
    _pToolBar->addAction(_pPrintAct);
    _pToolBar->addAction(_pPrintPreviewAct);
    _pToolBar->addAction(_pSaveAsOdt);
    _pToolBar->addAction(_pPrintPDFAct);
    _pToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBar(Qt::TopToolBarArea, _pToolBar);

    _pToolBarNavigation = new QToolBar(this);
    _pToolBarNavigation->addAction(_pBackwardAct);
    _pToolBarNavigation->addAction(_pHomeAct);
    _pToolBarNavigation->addAction(_pForwardAct);
    _pToolBarNavigation->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBar(Qt::TopToolBarArea, _pToolBarNavigation);

    _pEditToolBar = new QToolBar(this);
    _pEditToolBar->addAction(_pCutAct);
    _pEditToolBar->addAction(_pCopyAct);
    _pEditToolBar->addAction(_pPasteAct);
    _pEditToolBar->addSeparator();
    _pEditToolBar->addAction(_pUndoAct);
    _pEditToolBar->addAction(_pRedoAct);
    _pEditToolBar->addSeparator();
    _pEditToolBar->addAction(_pFindAct);
    _pEditToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBar(Qt::TopToolBarArea, _pEditToolBar);

    _pHelpToolBar = new QToolBar(this);
    _pHelpToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    _pHelpToolBar->addAction(_pHelpAct);
    addToolBar(Qt::TopToolBarArea, _pHelpToolBar);
    addToolBarBreak(Qt::TopToolBarArea);

    SetupFontActions();
    addToolBar(Qt::TopToolBarArea, _pToolBarFormat);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(Qt::TopToolBarArea, _pFontToolbar);

    //us6_t-001 Спринт 2 Алексей: Реализовать доквиджет для быстрого доступа к файлам на текущем диске
    _pFileManager = new FileManager(this);
    connect(_pFileManager, SIGNAL(SignalSetActive(QString)),
            this, SLOT(SlotSetActiveSubWindowByPath(QString)));
    _pDocWidget = new QDockWidget(tr("File manager"), this);
    _pDocWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    _pDocWidget->setWidget(_pFileManager);
    addDockWidget(Qt::LeftDockWidgetArea, _pDocWidget);
    _pDocWidget->close();

    // Создане доквиджета для ИИ-чата
    _pAIChat = new AIChat(this);
    _pDocWidgetAI = new QDockWidget(tr("AI chat"), this);
    _pDocWidgetAI->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    _pDocWidgetAI->setWidget(_pAIChat);
    addDockWidget(Qt::LeftDockWidgetArea, _pDocWidgetAI);  // RightDockWidgetArea

    _pListPath = new QList<QString>;

    _pCurrentDocument = nullptr;
    connect(_pMdiArea, &QMdiArea::subWindowActivated,
            this, &MainWindow::SetupActiveDocument);
    
    // Создаём статусбар
    statusBar()->showMessage("Ready", 3000);

    setWindowIcon(QPixmap(":/images/icons/khexedit.png"));

    // Открываем стартовый файл
    QString startFileName = QDir(QDir::currentPath()).filePath(":/pages/index.html");
    QFile file(startFileName);
    if(file.exists())
        OpenFile(startFileName);
    else
        SlotNewDoc();

    // Загружаем настройки
    if(Settings::GetInstance().GetLanguage() == Language::Russian)
    {
        if(_translator.load(":/translations/Hypertext-Neural-Editor_ru_RU.qm", ":/translations/"))
            qApp->installTranslator(&_translator);
    }
    else
    {
        if(_translator.load(":/translations/Hypertext-Neural-Editor_en_US.qm", ":/translations/"))
            qApp->installTranslator(&_translator);
    }

    if(Settings::GetInstance().GetTheme() == Theme::Light)
        SlotSetupLightTheme();
    else
        SlotSetupDarkTheme();

    SlotUpdateMenus();
    _pMdiArea->setViewMode(QMdiArea::TabbedView);

    menuBar()->hide();
    _pToolBar->hide();
    _pEditToolBar->hide();
    _pFontToolbar->hide();
    _pToolBarFormat->hide();
}

// Метод открытия файла в дочернем окне
bool MainWindow::OpenFile(const QString &pathFileName)
{
    if (pathFileName.isEmpty())
    {
        SlotStatusBarMessage(tr("Path empty!"));
        return false;
    }
    
    if (!_pListPath->contains(pathFileName))
    {
        DocumentWindow* pDocument = CreateNewDocument();
        if (pDocument->OpenFile(pathFileName))
        {
            pDocument->show();
            return true;
        }
        else
        {
            pDocument->close();
            return false;
        }
    }
    return false;
}

// Перегруженный метод закрытия виджета
void MainWindow::closeEvent(QCloseEvent *event)
{
    _pMdiArea->closeAllSubWindows();
    QList<QMdiSubWindow *> windowList = _pMdiArea->subWindowList();
    if (windowList.length() > 0)
        event->ignore();
    else
        event->accept();
}

// Перегруженный метод обработки события смены
void MainWindow::changeEvent(QEvent *pEvent)
{
    if (pEvent->type() == QEvent::LanguageChange)
        SetInterfaceLabels();
    QWidget::changeEvent(pEvent);
}

// Устанавливаем все надписи интерфейса
void MainWindow::SetInterfaceLabels()
{
    setWindowTitle("Путеводитель по Китаю"); //tr"("China Travel Guide"));

    _pmnuFile->setTitle(tr("&File"));
    _pMenuEdit->setTitle(tr("&Edit"));
    _pMenuFormat->setTitle(tr("F&ormat"));
    _pMenuWindows->setTitle(tr("&Tabs"));
    _pMenuSettings->setTitle(tr("&Settings"));
    _pMenuHelp->setTitle(tr("&Help"));

    _pToolBar->setWindowTitle(tr("File/print panel"));
    _pToolBarNavigation->setWindowTitle(tr("Navigation panel"));
    _pEditToolBar->setWindowTitle(tr("Edit panel"));
    _pHelpToolBar->setWindowTitle(tr("Help panel"));
    _pToolBarFormat->setWindowTitle(tr("Format panel"));
    _pFontToolbar->setWindowTitle(tr("Font panel"));

    _pDocWidget->setWindowTitle(tr("File manager"));
    _pDocWidgetAI->setWindowTitle(tr("AI chat"));
    _pAIChat->SetPropertiesButtonText(tr("Properties"));
    _pAIChat->SetSendButtonText(tr("Send"));

    _pNewAct->setText(tr("&New"));
    _pNewAct->setToolTip(tr("New document"));
    _pNewAct->setStatusTip(tr("Create a new file"));
    _pNewAct->setWhatsThis(tr("Create a new file"));

    _pOpenAct->setText(tr("&Open..."));
    _pOpenAct->setToolTip(tr("Open document"));
    _pOpenAct->setStatusTip(tr("Open an existing file"));
    _pOpenAct->setWhatsThis(tr("Open an existing file"));

    _pSaveAct->setText(tr("&Save"));
    _pSaveAct->setToolTip(tr("Save document"));
    _pSaveAct->setStatusTip(tr("Save the file to disk"));
    _pSaveAct->setWhatsThis(tr("Save the file to disk"));

    _pSaveAsAct->setText(tr("Save &As..."));
    _pSaveAsAct->setToolTip(tr("Save document as..."));
    _pSaveAsAct->setStatusTip(tr("Save the file to disk as..."));
    _pSaveAsAct->setWhatsThis(tr("Save the file to disk as..."));

    _pPrintAct->setText(tr("&Print file..."));
    _pPrintAct->setToolTip(tr("Print file"));
    _pPrintAct->setStatusTip(tr("Print file"));
    _pPrintAct->setWhatsThis(tr("Print file"));

    _pPrintPreviewAct->setText(tr("P&rint preview..."));
    _pPrintPreviewAct->setToolTip(tr("Print preview"));
    _pPrintPreviewAct->setStatusTip(tr("Print preview"));
    _pPrintPreviewAct->setWhatsThis(tr("Print preview"));

    _pSaveAsOdt->setText(tr("Save as OD&T file..."));
    _pSaveAsOdt->setToolTip(tr("Save as ODT file"));
    _pSaveAsOdt->setStatusTip(tr("Save as ODT file"));
    _pSaveAsOdt->setWhatsThis(tr("Save as ODT file"));

    _pPrintPDFAct->setText(tr("Save as P&DF file..."));
    _pPrintPDFAct->setToolTip(tr("Save as PDF file"));
    _pPrintPDFAct->setStatusTip(tr("Save as PDF file"));
    _pPrintPDFAct->setWhatsThis(tr("Save as PDF file"));

    _pBackwardAct->setText(tr("Backward navigation"));
    _pBackwardAct->setToolTip(tr("Backward navigation"));
    _pBackwardAct->setStatusTip(tr("Backward navigation"));
    _pBackwardAct->setWhatsThis(tr("Backward navigation"));

    _pHomeAct->setText(tr("Home navigation"));
    _pHomeAct->setToolTip(tr("Home navigation"));
    _pHomeAct->setStatusTip(tr("Home navigation"));
    _pHomeAct->setWhatsThis(tr("Home navigation"));

    _pForwardAct->setText(tr("Forward navigation"));
    _pForwardAct->setToolTip(tr("Forward navigation"));
    _pForwardAct->setStatusTip(tr("Forward navigation"));
    _pForwardAct->setWhatsThis(tr("Forward navigation"));

    _pCutAct->setText(tr("Cu&t"));
    _pCutAct->setToolTip(tr("Cut text"));
    _pCutAct->setStatusTip(
        tr("Cut the current selection's contents to the clipboard"));
    _pCutAct->setWhatsThis(
        tr("Cut the current selection's contents to the clipboard"));

    _pCopyAct->setText(tr("&Copy"));
    _pCopyAct->setToolTip(tr("Copy text"));
    _pCopyAct->setStatusTip(
        tr("Copy the current selection's contents to the clipboard"));
    _pCopyAct->setWhatsThis(
        tr("Copy the current selection's contents to the clipboard"));

    _pPasteAct->setText(tr("&Paste"));
    _pPasteAct->setToolTip(tr("Paste text"));
    _pPasteAct->setStatusTip(
        tr("Paste the clipboard's contents into the current selection"));
    _pPasteAct->setWhatsThis(
        tr("Paste the clipboard's contents into the current selection"));

    _pUndoAct->setText(tr("&Undo"));
    _pUndoAct->setToolTip(tr("Undo changes"));
    _pUndoAct->setStatusTip(tr("Undo changes"));
    _pUndoAct->setWhatsThis(tr("Undo changes"));

    _pRedoAct->setText(tr("&Redo"));
    _pRedoAct->setToolTip(tr("Redo changes"));
    _pRedoAct->setStatusTip(tr("Redo changes"));
    _pRedoAct->setWhatsThis(tr("Redo changes"));

    _pNextAct->setText(tr("&Next"));
    _pNextAct->setStatusTip(tr("Move the focus to the next window"));

    _pPreviousAct->setText(tr("&Previous"));
    _pPreviousAct->setStatusTip(tr("Move the focus to the previous window"));

    _pFindAct->setText(tr("&Find..."));
    _pFindAct->setToolTip(tr("Find text"));
    _pFindAct->setStatusTip(tr("Find text in current window"));
    _pFindAct->setWhatsThis(tr("Find text in current window"));

    _pMakeLinkAct->setText(tr("&Hyperlink..."));
    _pMakeLinkAct->setToolTip(tr("Make hyperlink"));
    _pMakeLinkAct->setStatusTip(tr("Make hyperlink"));
    _pMakeLinkAct->setWhatsThis(tr("Make hyperlink"));

    _pAddImageAct->setText(tr("&Image..."));
    _pAddImageAct->setToolTip(tr("Add image"));
    _pAddImageAct->setStatusTip(tr("Add image"));
    _pAddImageAct->setWhatsThis(tr("Add image"));

    _pCloseAct->setText(tr("&Close"));
    _pCloseAct->setStatusTip(tr("Close the active window"));

    _pCloseAllAct->setText(tr("Close &All"));
    _pCloseAllAct->setStatusTip(tr("Close all the windows"));

    _pAboutAct->setText(tr("&About..."));
    _pAboutAct->setToolTip(tr("Show About box"));
    _pAboutAct->setStatusTip(tr("Show the application's About box"));
    _pAboutAct->setWhatsThis(tr("Show the application's About box"));

    _pHelpAct->setText(tr("&Help..."));
    _pHelpAct->setToolTip(tr("Show help"));
    _pHelpAct->setStatusTip(tr("Show help"));
    _pHelpAct->setWhatsThis(tr("Show help"));

    _pQuitAct->setText(tr("&Quit"));
    _pQuitAct->setToolTip(tr("Quit"));
    _pQuitAct->setStatusTip(tr("Quit"));
    _pQuitAct->setWhatsThis(tr("Quit"));

    _pSetRussian->setText(tr("&Russian language"));
    _pSetRussian->setWhatsThis(tr("Russian language"));

    _pSetEnglish->setText(tr("&English language"));
    _pSetEnglish->setWhatsThis(tr("English language"));

    _pSetLightTheme->setText(tr("&Light theme"));
    _pSetLightTheme->setWhatsThis(tr("Light theme"));

    _pSetDarkTheme->setText(tr("&Dark theme"));
    _pSetDarkTheme->setWhatsThis(tr("Dark theme"));

    _pActionTextBold->setText(tr("&Bold"));
    _pActionTextBold->setToolTip(tr("Bold text"));
    _pActionTextBold->setStatusTip(tr("Bold text"));
    _pActionTextBold->setWhatsThis(tr("Bold text"));

    _pActionTextItalic->setText(tr("&Italic"));
    _pActionTextItalic->setToolTip(tr("Italic text"));
    _pActionTextItalic->setStatusTip(tr("Italic text"));
    _pActionTextItalic->setWhatsThis(tr("Italic text"));

    _pActionTextUnderline->setText(tr("&Underline"));
    _pActionTextUnderline->setToolTip(tr("Underline text"));
    _pActionTextUnderline->setStatusTip(tr("Underline text"));
    _pActionTextUnderline->setWhatsThis(tr("Underline text"));

    _pActionTextColor->setText(tr("&Color..."));
    _pActionTextColor->setToolTip(tr("Text color"));
    _pActionTextColor->setStatusTip(tr("Text color"));
    _pActionTextColor->setWhatsThis(tr("Text color"));

    _pAlignLeftAct->setText(tr("&Left"));
    _pAlignLeftAct->setToolTip(tr("Alignment to the left"));
    _pAlignLeftAct->setStatusTip(tr("Set the alignment to the left"));
    _pAlignLeftAct->setWhatsThis(tr("Set the alignment to the left"));

    _pAlignCenterAct->setText(tr("C&enter"));
    _pAlignCenterAct->setToolTip(tr("Alignment to the center"));
    _pAlignCenterAct->setStatusTip(tr("Set the alignment to the center"));
    _pAlignCenterAct->setWhatsThis(tr("Set the alignment to the center"));

    _pAlignRightAct->setText(tr("&Right"));
    _pAlignRightAct->setToolTip(tr("Alignment to the right"));
    _pAlignRightAct->setStatusTip(tr("Set the alignment to the right"));
    _pAlignRightAct->setWhatsThis(tr("Set the alignment to the right"));

    _pAlignJustifyAct->setText(tr("&Justify"));
    _pAlignJustifyAct->setToolTip(tr("Justify alignment"));
    _pAlignJustifyAct->setStatusTip(tr("Set the justify alignment"));
    _pAlignJustifyAct->setWhatsThis(tr("Set the justify alignment"));

    _pActionIndentMoreAct->setText(tr("I&ndent"));
    _pActionIndentMoreAct->setToolTip(tr("Indent"));
    _pActionIndentMoreAct->setStatusTip(tr("Increase indent"));
    _pActionIndentMoreAct->setWhatsThis(tr("Increase indent"));

    _pActionIndentLessAct->setText(tr("Unin&dent"));
    _pActionIndentLessAct->setToolTip(tr("Unindent"));
    _pActionIndentLessAct->setStatusTip(tr("Decrease indent"));
    _pActionIndentLessAct->setWhatsThis(tr("Decrease indent"));

    _pActionBackgroundColor->setText(tr("B&ackground color..."));
    _pActionBackgroundColor->setToolTip(tr("Background color"));
    _pActionBackgroundColor->setStatusTip(tr("Set the background color"));
    _pActionBackgroundColor->setWhatsThis(tr("Set the background color"));

    auto index = _pComboFontStyle->currentIndex();
    _pComboFontStyle->clear();
    _pComboFontStyle->addItems({tr("Standard"),
                                tr("Bullet List (Disc)"),
                                tr("Bullet List (Circle)"),
                                tr("Bullet List (Square)"),
                                tr("Task List (Unchecked)"),
                                tr("Task List (Checked)"),
                                tr("Ordered List (Decimal)"),
                                tr("Ordered List (Alpha lower)"),
                                tr("Ordered List (Alpha upper)"),
                                tr("Ordered List (Roman lower)"),
                                tr("Ordered List (Roman upper)"),
                                tr("Heading 1"),
                                tr("Heading 2"),
                                tr("Heading 3"),
                                tr("Heading 4"),
                                tr("Heading 5"),
                                tr("Heading 6")});
    _pComboFontStyle->setCurrentIndex(index);
}

// Возвращает указатель на текущий активный документ
DocumentWindow* MainWindow::GetActiveDocumentWindow()
{
    auto mdiChild = _pMdiArea->activeSubWindow();
    if (mdiChild)
        return qobject_cast<DocumentWindow*>(mdiChild->widget());
    else
    {
        _pCurrentDocument = nullptr;
        return nullptr;
    }
}

// Метод создаёт экземпляр дочернего MDI окна документа
DocumentWindow* MainWindow::CreateNewDocument()
{
    DocumentWindow* pDocument = new DocumentWindow(this);
    _pMdiArea->addSubWindow(pDocument);
    pDocument->setAttribute(Qt::WA_DeleteOnClose);
    pDocument->setWindowTitle("new_" + QString::number(_iUnnamedIndex++));
    pDocument->setWindowIcon(QPixmap(":/images/icons/filenew.png"));

    connect(pDocument, SIGNAL(SignalStatusBarMessage(QString)),
            this, SLOT(SlotStatusBarMessage(QString)));
    connect(pDocument, &DocumentWindow::IsOpen, this, &MainWindow::SlotOnOpen);
    connect(pDocument, &DocumentWindow::IsClose, this, &MainWindow::SlotDelPath);

    return pDocument;
}

// Слот - вывод сообщения в статусбаре главного окна
void MainWindow::SlotStatusBarMessage(QString Message)
{
    statusBar()->showMessage(Message);
}

// Слот создания нового документа
void MainWindow::SlotNewDoc()
{
    CreateNewDocument()->show();
}

// Слот загрузки документа
void MainWindow::SlotLoad()
{
    QString path = DocumentWindow::Load();
    if (path.isEmpty())
    {
        SlotStatusBarMessage(tr("Path empty!"));
        return;
    }
    if (!_pListPath->contains(path))
    {
        DocumentWindow* pDocument = CreateNewDocument();
        pDocument->OpenFile(path);
    }
}

// Слот сохранения документа
void MainWindow::SlotSave()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (pDocument)
        pDocument->Save();
}

// Слот активации действия сохранения документа
void MainWindow::SlotSaveEnable()
{
    _pSaveAct->setEnabled(true);
}

// Слот сохранить документ как
void MainWindow::SlotSaveAs()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (pDocument)
    {
        auto index = _pListPath->indexOf(pDocument->GetPathFileName());
        if (index > 0)
            _pListPath->remove(index);
        pDocument->SaveAs();
        _pListPath->append(pDocument->GetPathFileName());
    }
}

// Слот вызова окна "О программе"
void MainWindow::SlotAbout()
{
    QMessageBox::about(this, tr("Hypertext Neural Editor"), tr("<b>LLM Technologies</b><br>Version: 1.01"));
}

// Слот меню "Окна"
void MainWindow::SlotWindows()
{
    _pMenuWindows->clear();

    _pMenuWindows->addAction(_pCloseAct);
    _pMenuWindows->addAction(_pCloseAllAct);
    _pMenuWindows->addSeparator();
    _pMenuWindows->addAction(_pNextAct);
    _pMenuWindows->addAction(_pPreviousAct);
    _pMenuWindows->addSeparator();

    QList<QMdiSubWindow*> listDocuments = _pMdiArea->subWindowList();
    for(auto& document: listDocuments)
    {
        QAction* pAction = _pMenuWindows->addAction(document->windowTitle());
        pAction->setCheckable(true);
        pAction->setChecked(_pMdiArea->activeSubWindow() == document);
        connect(pAction, SIGNAL(triggered()), _pSignalMapper, SLOT(map()));
        _pSignalMapper->setMapping(pAction, document);
    }
}

// Слот навигация назад
void MainWindow::SlotBackward()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (pDocument)
        pDocument->backward();
}

// Слот навигация Домой
void MainWindow::SlotHome()
{
    _pMdiArea->closeAllSubWindows();
    QString startFileName = QDir(QDir::currentPath()).filePath(":/pages/index.html");
    QFile file(startFileName);
    if(file.exists())
        OpenFile(startFileName);

//    DocumentWindow* pDocument = GetActiveDocumentWindow();
//    if (pDocument)
//        pDocument->home();
}

// Слот навигация вперёд
void MainWindow::SlotForward()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (pDocument)
        pDocument->forward();
}

// Слот вырезать текст
void MainWindow::SlotCut()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (pDocument)
        pDocument->cut();
}

// Слот копировать текст
void MainWindow::SlotCopy()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (pDocument)
        pDocument->copy();
}

// Слот вставить текст
void MainWindow::SlotPaste()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (pDocument)
        pDocument->paste();
}

// Слот печати документа
void MainWindow::SlotPrint()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();

    if (!pDocument) return;
    QPrinter *printer = new QPrinter;
    QPrintDialog dlg = QPrintDialog(printer);
    dlg.setWindowTitle(tr("Print document"));

    if(dlg.exec() != QDialog::Accepted) return;

    pDocument->print(printer);
}

// Слот предварительного просмотра перед печатью документа
void MainWindow::SlotPrintPreview()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (pDocument)
    {
        QPrinter* printer = new QPrinter;
        QPrintPreviewDialog prPreviewDlg(printer, this);
        connect(&prPreviewDlg, SIGNAL(paintRequested(QPrinter*)), SLOT(SlotPrintPreviewDraw(QPrinter*)));

        if(prPreviewDlg.exec())
            pDocument->print(printer);
    }
}

// Слот отображения для предварительного просмотра
void MainWindow::SlotPrintPreviewDraw(QPrinter* printer)
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (pDocument)
        pDocument->print(printer);
}

// Слот сохранить документ в ODT
void MainWindow::SlotSaveAsOdt()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (pDocument)
    {
        QFileDialog fileDialog(this, tr("Save as ODT"), QDir::currentPath());
        fileDialog.setOptions(QFileDialog::DontUseNativeDialog);
        fileDialog.setAcceptMode(QFileDialog::AcceptSave);
        QStringList mimeTypes{"application/vnd.oasis.opendocument.text"};
        fileDialog.setMimeTypeFilters(mimeTypes);
        fileDialog.setDefaultSuffix("odt");
        if (fileDialog.exec() != QDialog::Accepted)
            return;
        const QString pathFileName = fileDialog.selectedFiles().constFirst();

        if (pathFileName.isEmpty())
            return;

        pDocument->SaveAsOdt(pathFileName);
    }
}

// Слот сохранить документ в PDF
void MainWindow::SlotPrintPDF()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (!pDocument)
        return;

    QFileDialog fileDialog(this, tr("Save document to pdf"),
                           QDir::currentPath(), tr("PDF files (*.pdf)"));
    fileDialog.setOptions(QFileDialog::DontUseNativeDialog);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setDefaultSuffix("pdf");

    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString pathFileName = fileDialog.selectedFiles().constFirst();

    if (pathFileName.isEmpty())
        return;

    QPrinter *printer = new QPrinter;
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(pathFileName);
    pDocument->print(printer);
}

// Слот поиск в тексте
void MainWindow::SlotFind()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (pDocument)
    {
        if (_pFindDialog)
        {
            _pFindDialog->close();
            _pFindDialog->deleteLater();
        }
        // Создаём диалог поиска
        _pFindDialog  = new FindDialog(this);
        _pFindDialog->setWindowTitle(tr("Find text"));
        _pFindDialog->SetWTCheckBoxLabel(tr("Find whole text"));

        _pFindDialog->ClearRequest();

        auto selectedText = pDocument->GetSelectedText();
        if (selectedText.isEmpty())
            _pFindDialog->ClearRequest();
        else
            _pFindDialog->SetLineEditText(selectedText);

        _pFindDialog->show();
    }
}

// Слот добавление гиперссылки
void MainWindow::SlotMakeHyperlink()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (pDocument)
    {
        if (_pMakeLinkDialog)
        {
            _pMakeLinkDialog->close();
            _pMakeLinkDialog->deleteLater();
        }
        // Создаём диалог добавления гиперссылки
        _pMakeLinkDialog  = new HyperlinkDialog(this);
        _pMakeLinkDialog->setWindowTitle(tr("Make hyperlink"));
        _pMakeLinkDialog->ClearTarget();
        auto selectedText = pDocument->GetSelectedText();
        if (selectedText.isEmpty())
            _pMakeLinkDialog->ClearText();
        else
            _pMakeLinkDialog->SetLineEditText(selectedText);
        _pMakeLinkDialog->show();
    }
}

// Слот добавить изображение
void MainWindow::SlotAddImage()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    if (pDocument)
        pDocument->AddImage();
}

// Слот показать справку
void MainWindow::SlotHelp()
{
    if(_pShowHelpDialog)
    {
        _pShowHelpDialog->close();
        _pShowHelpDialog->deleteLater();
    }
    // Создаём диалог показа помощи
    _pShowHelpDialog  = new HelpViewDialog("index.html",
                                          {":/documentation/help/"}, this);
    _pShowHelpDialog->setWindowTitle(tr("Help view"));

    _pShowHelpDialog->show();
}

// Слот сделать активными/не активными эементы интерфеса, если документ открыт
void MainWindow::SlotUpdateMenus()
{
    DocumentWindow* pDocument = GetActiveDocumentWindow();
    _pSaveAsAct->setEnabled(pDocument);
    _pPasteAct->setEnabled(pDocument);
    _pPrintAct->setEnabled(pDocument);
    _pPrintPreviewAct->setEnabled(pDocument);
    _pSaveAsOdt->setEnabled(pDocument);
    _pPrintPDFAct->setEnabled(pDocument);
    _pFindAct->setEnabled(pDocument);
    _pMakeLinkAct->setEnabled(pDocument);
    _pAddImageAct->setEnabled(pDocument);
    _pActionTextBold->setEnabled(pDocument);
    _pActionTextUnderline->setEnabled(pDocument);
    _pActionTextItalic->setEnabled(pDocument);

    _pAlignLeftAct->setEnabled(pDocument);
    _pAlignCenterAct->setEnabled(pDocument);
    _pAlignRightAct->setEnabled(pDocument);
    _pAlignJustifyAct->setEnabled(pDocument);
    _pActionIndentMoreAct->setEnabled(pDocument);
    _pActionIndentLessAct->setEnabled(pDocument);
    _pActionTextColor->setEnabled(pDocument);
    _pActionBackgroundColor->setEnabled(pDocument);

    _pCloseAct->setEnabled(pDocument);
    _pCloseAllAct->setEnabled(pDocument);
    _pNextAct->setEnabled(pDocument);
    _pPreviousAct->setEnabled(pDocument);

    _pComboFont->setEnabled(pDocument);
    _pComboFontStyle->setEnabled(pDocument);
    _pComboSize->setEnabled(pDocument);

    _pSaveAct->setEnabled(false);
    _pUndoAct->setEnabled(false);
    _pRedoAct->setEnabled(false);
    if (pDocument)
    {
        _pSaveAct->setEnabled(pDocument->document()->isModified());
        _pUndoAct->setEnabled(pDocument->document()->isUndoAvailable());
        _pRedoAct->setEnabled(pDocument->document()->isRedoAvailable());
    }

    _pBackwardAct->setEnabled(false);
    _pHomeAct->setEnabled(false);
    _pForwardAct->setEnabled(false);
    if (pDocument)
    {
        QFileInfo fi(pDocument->GetPathFileName());
        auto suffix = fi.suffix();

        if (suffix == "html" || suffix == "htm")
        {
            _pBackwardAct->setEnabled(true);
            _pHomeAct->setEnabled(true);
            _pForwardAct->setEnabled(true);
        }
    }

    bool textSelection = (pDocument &&
                          pDocument->textCursor().hasSelection());
    _pCutAct->setEnabled(textSelection);
    _pCopyAct->setEnabled(textSelection);
}

// Слот делает дочернее MDI окно активным
void MainWindow::SlotSetActiveSubWindow(QObject* pMdiSubWindow)
{
    if (pMdiSubWindow)
        _pMdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(pMdiSubWindow));
}

// Слот делает дочернее MDI окно активным по имени
void MainWindow::SlotSetActiveSubWindowByPath(QString path)
{
    QList<QMdiSubWindow*> listDocuments = _pMdiArea->subWindowList();
    for(auto& document: listDocuments)
    {
        auto doc = qobject_cast<DocumentWindow*>(document->widget());
        if(doc && (doc->GetPathFileName() == path))
        {
            _pMdiArea->setActiveSubWindow(document);
            return;
        }
    }
}

// Слот устанавливаем русский язык
void MainWindow::SlotSetupRussianLanguage()
{
    Settings::GetInstance().SetLanguage(Language::Russian);

    if(_translator.load(":/translations/Hypertext-Neural-Editor_ru_RU.qm", ":/translations/"))
    {
        qApp->installTranslator(&_translator);

        if (_pFindDialog)
        {
            _pFindDialog->close();
            _pFindDialog->deleteLater();
        }
        _pFindDialog = nullptr;

        if (_pMakeLinkDialog)
        {
            _pMakeLinkDialog->close();
            _pMakeLinkDialog->deleteLater();
        }
        _pMakeLinkDialog = nullptr;

        if(_pShowHelpDialog)
        {
            _pShowHelpDialog->close();
            _pShowHelpDialog->deleteLater();
        }
        _pShowHelpDialog = nullptr;
    }
}

// Слот устанавливаем английский язык
void MainWindow::SlotSetupEnglishLanguage()
{
    Settings::GetInstance().SetLanguage(Language::English);

    if(_translator.load(":/translations/Hypertext-Neural-Editor_en_US.qm", ":/translations/"))
    {
        qApp->installTranslator(&_translator);

        if (_pFindDialog)
        {
            _pFindDialog->close();
            _pFindDialog->deleteLater();
        }
        _pFindDialog = nullptr;

        if (_pMakeLinkDialog)
        {
            _pMakeLinkDialog->close();
            _pMakeLinkDialog->deleteLater();
        }
        _pMakeLinkDialog = nullptr;

        if(_pShowHelpDialog)
        {
            _pShowHelpDialog->close();
            _pShowHelpDialog->deleteLater();
        }
        _pShowHelpDialog = nullptr;
    }
}

// Слот устанавливаем светлую тему
void MainWindow::SlotSetupLightTheme()
{
    Settings::GetInstance().SetTheme(Theme::Light);

    qApp->setStyle("Fusion");
    QPalette* pLightPalette = new QPalette();      // палитра для светлой темы
    qApp->setPalette(*pLightPalette);
}

// Слот устанавливаем тёмную тему
void MainWindow::SlotSetupDarkTheme()
{
    Settings::GetInstance().SetTheme(Theme::Dark);

    qApp->setStyle("Fusion");
    QPalette* pDarkPalette = new QPalette();       // палитра для тёмной темы
    pDarkPalette->setColor(QPalette::Window, QColor(53, 53, 53));
    pDarkPalette->setColor(QPalette::WindowText, Qt::white);
    pDarkPalette->setColor(QPalette::Base, QColor(35, 35, 35));
    pDarkPalette->setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    pDarkPalette->setColor(QPalette::ToolTipBase, QColor(25, 25, 25));
    pDarkPalette->setColor(QPalette::ToolTipText, Qt::white);
    pDarkPalette->setColor(QPalette::Text, Qt::white);
    pDarkPalette->setColor(QPalette::Button, QColor(53, 53, 53));
    pDarkPalette->setColor(QPalette::ButtonText, Qt::white);
    pDarkPalette->setColor(QPalette::BrightText, Qt::red);
    pDarkPalette->setColor(QPalette::Link, QColor(42, 130, 218));
    pDarkPalette->setColor(QPalette::Highlight, QColor(42, 130, 218));
    pDarkPalette->setColor(QPalette::HighlightedText, QColor(35, 35, 35));
    pDarkPalette->setColor(QPalette::Active, QPalette::Button, QColor(53, 53, 53));
    pDarkPalette->setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    pDarkPalette->setColor(QPalette::Disabled, QPalette::WindowText, Qt::darkGray);
    pDarkPalette->setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    pDarkPalette->setColor(QPalette::Disabled, QPalette::Light, QColor(53, 53, 53));
    qApp->setPalette(*pDarkPalette);
}

// Слот добавления пути в список путей
void MainWindow::SlotOnOpen(QString path)
{
    _pListPath->append(path);
}

// Слот удаления пути из списка путей
void MainWindow::SlotDelPath(QString path)
{
    if(!path.isEmpty() && (_pListPath->indexOf(path) >= 0))
        _pListPath->remove(_pListPath->indexOf(path,0));
}

// Создаём объекты действий
void MainWindow::CreateActions()
{
    // Создание действия "Новый файл"
    _pNewAct = new QAction(tr("New File"), this);
    _pNewAct->setShortcut(QKeySequence::New);
    _pNewAct->setIcon(QPixmap(":/images/icons/filenew.png"));
    connect(_pNewAct, SIGNAL(triggered()), SLOT(SlotNewDoc()));

    // Создание действия "Открыть файл"
    _pOpenAct = new QAction(tr("Open file"), this);
    _pOpenAct->setShortcut(QKeySequence::Open);
    _pOpenAct->setIcon(QPixmap(":/images/icons/fileopen.png"));
    connect(_pOpenAct, SIGNAL(triggered()), SLOT(SlotLoad()));

    // Создание действия "Сохранить файл"
    _pSaveAct = new QAction(tr("Save file"), this);
    _pSaveAct->setShortcut(QKeySequence::Save);
    _pSaveAct->setIcon(QPixmap(":/images/icons/filesave.png"));
    connect(_pSaveAct, SIGNAL(triggered()), SLOT(SlotSave()));

    // Создание действия "Сохранить файл как"
    _pSaveAsAct = new QAction(tr("Save file as..."), this);
    _pSaveAsAct->setShortcut(QKeySequence("Ctrl+Shift+S"));
    _pSaveAsAct->setIcon(QPixmap(":/images/icons/filesaveas.png"));
    connect(_pSaveAsAct, SIGNAL(triggered()), SLOT(SlotSaveAs()));

    // Создание действия "Печать файла"
    _pPrintAct = new QAction(tr("Print"), this);
    _pPrintAct->setShortcut(QKeySequence::Print);
    _pPrintAct->setIcon(QPixmap(":/images/icons/fileprint.png"));
    connect(_pPrintAct, SIGNAL(triggered()), SLOT(SlotPrint()));

    // Создание действия "Предпросмотр перед печатю файла"
    _pPrintPreviewAct = new QAction(tr("Print preview"), this);
    _pPrintPreviewAct->setShortcut(QKeySequence("Ctrl+Shift+V"));
    _pPrintPreviewAct->setIcon(QPixmap(":/images/icons/frameprint.png"));
    connect(_pPrintPreviewAct, SIGNAL(triggered()), SLOT(SlotPrintPreview()));

    // Создание действия "Сохранить как *.odt"
    _pSaveAsOdt = new QAction(tr("Save as ODT"), this);
    _pSaveAsOdt->setShortcut(QKeySequence("Ctrl+Shift+O"));
    _pSaveAsOdt->setIcon(QPixmap(":/images/icons/ooo_gulls.png"));
    connect(_pSaveAsOdt, SIGNAL(triggered()), SLOT(SlotSaveAsOdt()));

    // Создание действия "Сохранить как PDF файл"
    _pPrintPDFAct = new QAction(tr("Save as PDF"), this);
    _pPrintPDFAct->setShortcut(QKeySequence("Ctrl+Shift+P"));
    _pPrintPDFAct->setIcon(QPixmap(":/images/icons/acroread.png"));
    connect(_pPrintPDFAct, SIGNAL(triggered()), SLOT(SlotPrintPDF()));

    // Создание действия "Назад"
    _pBackwardAct = new QAction(tr("Backward"), this);
    _pBackwardAct->setEnabled(false);
    _pBackwardAct->setShortcut(QKeySequence::Back);
    _pBackwardAct->setIcon(QPixmap(":/images/icons/back.png"));
    connect(_pBackwardAct, SIGNAL(triggered()), SLOT(SlotBackward()));

    // Создание действия "Домой"
    _pHomeAct = new QAction(tr("Home"), this);
    _pHomeAct->setIcon(QPixmap(":/images/icons/gohome.png"));
    connect(_pHomeAct, SIGNAL(triggered()), SLOT(SlotHome()));

    // Создание действия "Впрёд"
    _pForwardAct = new QAction(tr("Forward"), this);
    _pForwardAct->setEnabled(false);
    _pForwardAct->setShortcut(QKeySequence::Forward);
    _pForwardAct->setIcon(QPixmap(":/images/icons/forward.png"));
    connect(_pForwardAct, SIGNAL(triggered()), SLOT(SlotForward()));

    // Создание действия "Вырезать"
    _pCutAct = new QAction(tr("Cut"), this);
    _pCutAct->setShortcuts(QKeySequence::Cut);
    _pCutAct->setIcon(QPixmap(":/images/icons/editcut.png"));
    connect(_pCutAct, SIGNAL(triggered()), SLOT(SlotCut()));

    // Создание действия "Копировать"
    _pCopyAct = new QAction(tr("Copy"), this);
    _pCopyAct->setShortcuts(QKeySequence::Copy);
    _pCopyAct->setIcon(QPixmap(":/images/icons/editcopy.png"));
    connect(_pCopyAct, SIGNAL(triggered()), SLOT(SlotCopy()));

    // Создание действия "Вставить"
    _pPasteAct = new QAction(tr("Paste"), this);
    _pPasteAct->setShortcuts(QKeySequence::Paste);
    _pPasteAct->setIcon(QPixmap(":/images/icons/editpaste.png"));
    connect(_pPasteAct, SIGNAL(triggered()), SLOT(SlotPaste()));

    // Создание действия "Отменить"
    _pUndoAct = new QAction(tr("Undo"), this);
    _pUndoAct->setShortcuts(QKeySequence::Undo);
    _pUndoAct->setIcon(QPixmap(":/images/icons/undo.png"));

    // Создание действия "Вернуть"
    _pRedoAct = new QAction(tr("Redo"), this);
    _pRedoAct->setShortcuts(QKeySequence::Redo);
    _pRedoAct->setIcon(QPixmap(":/images/icons/redo.png"));

    // указатель на действие "Следующая вкладка"
    _pNextAct = new QAction(tr("Ne&xt"), this);
    _pNextAct->setShortcuts(QKeySequence::NextChild);

    // указатель на действие "Предыдующая вкладка"
    _pPreviousAct = new QAction(tr("Pre&vious"), this);
    _pPreviousAct->setShortcuts(QKeySequence::PreviousChild);

    // Создание действия "Поиск"
    _pFindAct = new QAction(tr("Find"), this);
    _pFindAct->setShortcut(QKeySequence::Find);
    _pFindAct->setIcon(QPixmap(":/images/icons/find.png"));
    connect(_pFindAct, SIGNAL(triggered()), SLOT(SlotFind()));

    // Создание действия добавить гиперссылку
    _pMakeLinkAct = new QAction(tr("Hyperlink"), this);
    _pMakeLinkAct->setShortcut(QKeySequence("CTRL+H"));
    _pMakeLinkAct->setIcon(QPixmap(":/images/icons/cache.png"));
    connect(_pMakeLinkAct, SIGNAL(triggered()), SLOT(SlotMakeHyperlink()));

    // Создание действия добавить изображение
    _pAddImageAct = new QAction(tr("Image"), this);
    _pAddImageAct->setShortcut(QKeySequence("Ctrl+M"));
    _pAddImageAct->setIcon(QPixmap(":/images/icons/thumbnail.png"));
    connect(_pAddImageAct, SIGNAL(triggered()), SLOT(SlotAddImage()));

    // Создание действия "О программе"
    _pAboutAct = new QAction(tr("About"), this);
    connect(_pAboutAct, SIGNAL(triggered()), SLOT(SlotAbout()));

    // Создание действия "О программе"
    _pHelpAct = new QAction(tr("&Help"), this);
    _pHelpAct->setShortcut(QKeySequence::HelpContents);
    _pHelpAct->setIcon(QPixmap(":/images/icons/help.png"));
    connect(_pHelpAct, SIGNAL(triggered()), SLOT(SlotHelp()));

    // Создание действия "Выход"
    _pQuitAct = new QAction(tr("&Quit"), this);
    _pQuitAct->setShortcut(QKeySequence("CTRL+Q"));
    connect(_pQuitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
}

// Создаём меню переключения языков
void MainWindow::SetupLanguageActions(QMenu *menu)
{
    QActionGroup* langGroup = new QActionGroup(this);
    langGroup->setExclusive(true);

    _pSetRussian = new QAction(tr("&Russian language"), this);
    _pSetRussian->setCheckable(true);
    langGroup->addAction(_pSetRussian);
    menu->addAction(_pSetRussian);

    _pSetEnglish = new QAction(tr("&English language"), this);
    _pSetEnglish->setCheckable(true);
    langGroup->addAction(_pSetEnglish);
    menu->addAction(_pSetEnglish);

    if(Settings::GetInstance().GetLanguage() == Language::Russian)
        _pSetRussian->setChecked(true);
    else
        _pSetEnglish->setChecked(true);

    connect(_pSetRussian, SIGNAL(triggered(bool)), SLOT(SlotSetupRussianLanguage()));
    connect(_pSetEnglish, SIGNAL(triggered(bool)), SLOT(SlotSetupEnglishLanguage()));
}

// Создаём меню переключения тем интерфейса
void MainWindow::SetupThemeActions(QMenu *menu)
{
    QActionGroup* themeGroup = new QActionGroup(this);
    themeGroup->setExclusive(true);

    _pSetLightTheme = new QAction(tr("&Light theme"), this);
    _pSetLightTheme->setCheckable(true);
    themeGroup->addAction(_pSetLightTheme);
    menu->addAction(_pSetLightTheme);

    _pSetDarkTheme = new QAction(tr("&Dark theme"), this);
    _pSetDarkTheme->setCheckable(true);
    themeGroup->addAction(_pSetDarkTheme);
    menu->addAction(_pSetDarkTheme);

    if(Settings::GetInstance().GetTheme() == Theme::Light)
        _pSetLightTheme->setChecked(true);
    else
        _pSetDarkTheme->setChecked(true);

    connect(_pSetLightTheme, SIGNAL(triggered(bool)), SLOT(SlotSetupLightTheme()));
    connect(_pSetDarkTheme, SIGNAL(triggered(bool)), SLOT(SlotSetupDarkTheme()));
}

// Формирование экшена для жирного шрифта
void MainWindow::SetupBoldActions(QToolBar* toolBar, QMenu* menu)
{
    _pActionTextBold = menu->addAction(tr("&Bold"));
    _pActionTextBold->setShortcut(QKeySequence::Bold);
    _pActionTextBold->setPriority(QAction::LowPriority);
    _pActionTextBold->setIcon(QPixmap(":/images/icons/text_bold.png"));
    QFont bold;
    bold.setBold(true);
    _pActionTextBold->setFont(bold);
    toolBar->addAction(_pActionTextBold);
    _pActionTextBold->setCheckable(true);
}

// Формирование экшена для курсивного шрифта
void MainWindow::SetupItalicActions(QToolBar* toolBar, QMenu* menu)
{
    _pActionTextItalic = menu->addAction(tr("&Italic"));
    _pActionTextItalic->setShortcut(QKeySequence::Italic);
    _pActionTextItalic->setPriority(QAction::LowPriority);
    _pActionTextItalic->setIcon(QPixmap(":/images/icons/text_italic.png"));
    QFont italic;
    italic.setItalic(true);
    _pActionTextItalic->setFont(italic);
    toolBar->addAction(_pActionTextItalic);
    _pActionTextItalic->setCheckable(true);
}

// Формирование экшена для подчеркнутого шрифта
void MainWindow::SetupUnderLineActions(QToolBar* toolBar, QMenu* menu)
{
    _pActionTextUnderline = menu->addAction(tr("&Underline"));
    _pActionTextUnderline->setShortcut(QKeySequence::Underline);
    _pActionTextUnderline->setPriority(QAction::LowPriority);
    _pActionTextUnderline->setIcon(QPixmap(":/images/icons/text_under.png"));
    QFont underline;
    underline.setUnderline(true);
    _pActionTextUnderline->setFont(underline);
    toolBar->addAction(_pActionTextUnderline);
    _pActionTextUnderline->setCheckable(true);
}

// Формирование экшена для изменения размера шрифта
void MainWindow::SetupSizeActions(QToolBar* toolBar)
{
    _pComboSize = new QComboBox(toolBar);
    _pComboSize->setObjectName("comboSize");
    _pComboSize->setFixedHeight(26);

    QFont chFont = _pComboSize->font();
    chFont.setPointSize(10);
    _pComboSize->setFont(chFont);

    toolBar->addWidget(_pComboSize);
    _pComboSize->setEditable(true);
    const QList<int> standardSizes = QFontDatabase::standardSizes();
    for (int size : standardSizes)
        _pComboSize->addItem(QString::number(size));

    _pComboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));
}

// Mетод создает панели и меню форматирования текста
void MainWindow::SetupFormatActions(QMenu* _pMenu)
{
    _pToolBarFormat = new QToolBar(tr("Format panel"), this);
    SetupBoldActions(_pToolBarFormat, _pMenu);
    SetupItalicActions(_pToolBarFormat, _pMenu);
    SetupUnderLineActions(_pToolBarFormat, _pMenu);
    _pMenu->addSeparator();
    _pToolBarFormat->addSeparator();
    SetupFontColorActions(_pToolBarFormat, _pMenu);
    SetupBackgroundColorActions( _pToolBarFormat, _pMenu);
    _pMenu->addSeparator();
    _pToolBarFormat->addSeparator();
    SetupJustifyActions(_pToolBarFormat, _pMenu);
    SetupIndentActions(_pToolBarFormat, _pMenu);

    _pToolBarFormat->addSeparator();
    _pToolBarFormat->addAction(_pMakeLinkAct);
    _pToolBarFormat->addAction(_pAddImageAct);
    _pToolBarFormat->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    return;
}

// Метод создает панели и меню конфигурирования шрифта
void MainWindow::SetupFontActions()
{
    _pFontToolbar = new QToolBar(tr("Font panel"), this);
    SetupFontStyle(_pFontToolbar);
    _pFontToolbar->addSeparator();
    SetupFontFamily(_pFontToolbar);
    _pFontToolbar->addSeparator();
    SetupSizeActions(_pFontToolbar);
    _pFontToolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    return;
}

// Отображение настроек измененного шрифта
void MainWindow::FontChanged(const QFont &f)
{
    _pComboFont->setCurrentIndex(_pComboFont->findText(QFontInfo(f).family()));
    _pActionTextBold->setChecked(f.bold());
    _pActionTextItalic->setChecked(f.italic());
    _pActionTextUnderline->setChecked(f.underline());
    _pComboSize->setCurrentIndex(_pComboSize->findText(QString::number(f.pointSize())));
}

//Mетод создает панель для изменения цвета шрифта
void MainWindow::SetupFontColorActions(QToolBar* toolBar, QMenu* menu)
{
    _pActionTextColor = new QAction();
    _pActionTextColor->setShortcut(QKeySequence("Ctrl+Shift+C"));
    ColorFontChanged(QColor(0,0,0));
    menu->addAction(_pActionTextColor);
    toolBar->addAction(_pActionTextColor);
}

// Получение указателя на активное окно редактирования, удаляет старые соединения, устанавливает новые
void MainWindow::SetupActiveDocument(QMdiSubWindow* window)
{
    DisonnectFromDocument();
    if(window)
    {
        _pCurrentDocument = dynamic_cast<DocumentWindow*>(window->widget());
        if(_pCurrentDocument)
        {
            CurrentCharFormatChanged(_pCurrentDocument->currentCharFormat());
            AlignmentChanged (_pCurrentDocument->alignment());
            ConnectToActiveDocument();

            QFileInfo fi(_pCurrentDocument->GetPathFileName());
            if (fi.isNativePath())
                QDir::setCurrent(fi.path());
        }
    }
}

// Устанавливает новые соединения с окном редактирования
void MainWindow::ConnectToActiveDocument()
{
    if (_pCurrentDocument)
    {
        connect(_pComboFont, &QComboBox::textActivated,
                _pCurrentDocument, &DocumentWindow::TextFamily);
        connect(_pActionTextBold, &QAction::triggered,
                _pCurrentDocument, &DocumentWindow ::TextBold);
        connect(_pActionTextItalic, &QAction::triggered,
                _pCurrentDocument, &DocumentWindow ::TextItalic);
        connect(_pActionTextUnderline, &QAction::triggered,
                _pCurrentDocument, &DocumentWindow ::TextUnderline);
        connect(_pCurrentDocument, &QTextEdit::currentCharFormatChanged,
                this, &MainWindow::CurrentCharFormatChanged);
        connect(_pComboSize, &QComboBox::textActivated,
                _pCurrentDocument, &DocumentWindow::TextSize);
        connect(_pActionTextColor, &QAction::triggered,
                _pCurrentDocument, &DocumentWindow::TextColor);
        connect(this, &MainWindow::Alignment,
                _pCurrentDocument, &QTextBrowser::setAlignment);
        connect(_pCurrentDocument, &QTextBrowser::cursorPositionChanged,
                this, &MainWindow::CursorPositionChanged);
        connect(_pActionIndentMoreAct, &QAction::triggered,
                _pCurrentDocument, &DocumentWindow::Indent);
        connect(_pActionIndentLessAct, &QAction::triggered,
                _pCurrentDocument, &DocumentWindow::Unindent);
        connect(_pActionBackgroundColor, &QAction::triggered,
                _pCurrentDocument, &DocumentWindow::BackgroundColor);
        connect(_pComboFontStyle, &QComboBox::activated,
                _pCurrentDocument, &DocumentWindow::TextStyle);

        connect(_pCurrentDocument, &QTextEdit::copyAvailable,
                _pCutAct, &QAction::setEnabled);
        connect(_pCurrentDocument, &QTextEdit::copyAvailable,
                _pCopyAct, &QAction::setEnabled);
        connect(_pCurrentDocument, &QTextEdit::textChanged,
                this, &MainWindow::SlotSaveEnable);
        connect(_pCurrentDocument, SIGNAL(backwardAvailable(bool)),
                _pBackwardAct, SLOT(setEnabled(bool)));
        connect(_pCurrentDocument, SIGNAL(forwardAvailable(bool)),
                _pForwardAct, SLOT(setEnabled(bool)));

        connect(_pUndoAct, &QAction::triggered,
                _pCurrentDocument, &DocumentWindow::undo);
        connect(_pRedoAct, &QAction::triggered,
                _pCurrentDocument, &DocumentWindow::redo);
        connect(_pCurrentDocument->document(), &QTextDocument::undoAvailable,
                _pUndoAct, &QAction::setEnabled);
        connect(_pCurrentDocument->document(), &QTextDocument::redoAvailable,
                _pRedoAct, &QAction::setEnabled);
    }
}

// Удаляет текущие соединения с  окном редактирования
void MainWindow::DisonnectFromDocument()
{
    if (!_pMdiArea->activeSubWindow())
        _pCurrentDocument = nullptr;

    if (_pCurrentDocument)
    {
        disconnect(_pComboFont, &QComboBox::textActivated,
                   _pCurrentDocument, &DocumentWindow::TextFamily);
        disconnect(_pActionTextBold, &QAction::triggered,
                   _pCurrentDocument, &DocumentWindow ::TextBold);
        disconnect(_pActionTextItalic, &QAction::triggered,
                   _pCurrentDocument, &DocumentWindow ::TextItalic);
        disconnect(_pActionTextUnderline, &QAction::triggered,
                   _pCurrentDocument, &DocumentWindow ::TextUnderline);
        disconnect(_pCurrentDocument, &QTextEdit::currentCharFormatChanged,
                   this, &MainWindow::CurrentCharFormatChanged);
        disconnect(_pActionTextColor, &QAction::triggered,
                   _pCurrentDocument, &DocumentWindow::TextColor);
        disconnect(this, &MainWindow::Alignment,
                   _pCurrentDocument, &DocumentWindow::setAlignment);
        disconnect(_pCurrentDocument, &QTextBrowser::cursorPositionChanged,
                this, &MainWindow::CursorPositionChanged);
        disconnect(_pCurrentDocument, &QTextEdit::copyAvailable,
                   _pCutAct, &QAction::setEnabled);
        disconnect(_pCurrentDocument, &QTextEdit::copyAvailable,
                   _pCopyAct, &QAction::setEnabled);
        disconnect(_pCurrentDocument, &QTextEdit::textChanged,
                   this, &MainWindow::SlotSaveEnable);
        disconnect(_pActionIndentMoreAct, &QAction::triggered,
                   _pCurrentDocument, &DocumentWindow::Indent);
        disconnect(_pActionIndentLessAct, &QAction::triggered,
                   _pCurrentDocument, &DocumentWindow::Unindent);
        disconnect(_pActionBackgroundColor, &QAction::triggered,
                   _pCurrentDocument, &DocumentWindow::BackgroundColor);
        disconnect(_pComboFontStyle, &QComboBox::activated,
                   _pCurrentDocument, &DocumentWindow::TextStyle);
        disconnect(_pCurrentDocument, SIGNAL(backwardAvailable(bool)),
                   _pBackwardAct, SLOT(setEnabled(bool)));
        disconnect(_pCurrentDocument, SIGNAL(forwardAvailable(bool)),
                   _pForwardAct, SLOT(setEnabled(bool)));

        disconnect(_pUndoAct, &QAction::triggered,
                   _pCurrentDocument, &DocumentWindow::undo);
        disconnect(_pRedoAct, &QAction::triggered,
                   _pCurrentDocument, &DocumentWindow::redo);
        disconnect(_pCurrentDocument->document(), &QTextDocument::undoAvailable,
                   _pUndoAct, &QAction::setEnabled);
        disconnect(_pCurrentDocument->document(), &QTextDocument::redoAvailable,
                   _pRedoAct, &QAction::setEnabled);
    }
}

// Устанавливает настройки, соответствующие формату шрифта
void MainWindow::CurrentCharFormatChanged(const QTextCharFormat &format)
{
    FontChanged(format.font());
    ColorFontChanged(format.foreground().color());
    QColor color;
    if (format.background().style()!=Qt::BrushStyle::NoBrush)
        color = format.background().color();
    else
        color = QColor(255,255,255);
    ColorBackgroundChanged(color);
}

// Устанавливает настройки, соответствующие цвету шрифта
void MainWindow::ColorFontChanged(const QColor &color)
{
    QPixmap pix = QPixmap(":/images/icons/fill_ch_p.png");
    auto mask = pix.createMaskFromColor(QColor(0,255,0), Qt::MaskOutColor);
    QPainter painter;
    painter.begin(&pix);
    painter.setPen(color);
    painter.drawPixmap(pix.rect(), mask, mask.rect());
    painter.end();
    _pActionTextColor->setIcon(pix);
}

//Mетод создает панель для изменения выравния шрифта
void MainWindow::SetupJustifyActions(QToolBar* toolBar, QMenu* menu)
{
    const QIcon leftIcon = QIcon::fromTheme("format-justify-left",
                                            QIcon(":/images/icons/text_left.png"));
    _pAlignLeftAct = new QAction(this);
    _pAlignLeftAct->setIcon(leftIcon);
    _pAlignLeftAct->setShortcut(Qt::CTRL | Qt::Key_L);
    _pAlignLeftAct->setCheckable(true);
    _pAlignLeftAct->setPriority(QAction::LowPriority);

    const QIcon centerIcon = QIcon::fromTheme("format-justify-center",
                                              QIcon(":/images/icons/text_center.png"));
    _pAlignCenterAct = new QAction(this);
    _pAlignCenterAct->setIcon(centerIcon);
    _pAlignCenterAct->setShortcut(Qt::CTRL | Qt::Key_E);
    _pAlignCenterAct->setCheckable(true);
    _pAlignCenterAct->setPriority(QAction::LowPriority);

    const QIcon rightIcon = QIcon::fromTheme("format-justify-right",
                                             QIcon(":/images/icons/text_right.png"));
    _pAlignRightAct = new QAction(this);
    _pAlignRightAct->setIcon(rightIcon);
    _pAlignRightAct->setShortcut(Qt::CTRL | Qt::Key_R);
    _pAlignRightAct->setCheckable(true);
    _pAlignRightAct->setPriority(QAction::LowPriority);

    const QIcon fillIcon = QIcon::fromTheme("format-justify-fill",
                                            QIcon(":/images/icons/text_block.png"));
    _pAlignJustifyAct = new QAction(this);
    _pAlignJustifyAct->setIcon(fillIcon);
    _pAlignJustifyAct->setShortcut(Qt::CTRL | Qt::Key_J);
    _pAlignJustifyAct->setCheckable(true);
    _pAlignJustifyAct->setPriority(QAction::LowPriority);

    _pAlignGroup = new QActionGroup(this);
     connect(_pAlignGroup, &QActionGroup::triggered, this, &MainWindow::TextAlign);
    _pAlignGroup->addAction(_pAlignLeftAct);
    _pAlignGroup->addAction(_pAlignCenterAct);
    _pAlignGroup->addAction(_pAlignRightAct);
    _pAlignGroup->addAction(_pAlignJustifyAct);

    toolBar->addActions(_pAlignGroup->actions());
    menu->addActions(_pAlignGroup->actions());
}

//Mетод создает панель для изменения отступов
void MainWindow::SetupIndentActions(QToolBar* toolBar, QMenu* menu)
{
    const QIcon indentMoreIcon = QIcon::fromTheme("format-indent-more",
                                                  QIcon(":/images/icons/indent.png"));
    _pActionIndentMoreAct = new QAction(this);
    _pActionIndentMoreAct->setIcon( indentMoreIcon);
    _pActionIndentMoreAct->setShortcut(Qt::CTRL | Qt::Key_BracketRight);
    _pActionIndentMoreAct->setPriority(QAction::LowPriority);

    const QIcon indentLessIcon = QIcon::fromTheme("format-indent-less",
                                                  QIcon(":/images/icons/unindent.png"));
    _pActionIndentLessAct = new QAction( indentLessIcon, tr("Unin&dent"), this);
    _pActionIndentLessAct->setIcon(indentLessIcon);
    _pActionIndentLessAct->setShortcut(Qt::CTRL | Qt::Key_BracketLeft);
    _pActionIndentLessAct->setPriority(QAction::LowPriority);

    toolBar->addAction(_pActionIndentMoreAct);
    toolBar->addAction(_pActionIndentLessAct);
    menu->addAction(_pActionIndentMoreAct);
    menu->addAction(_pActionIndentLessAct);
}

//Mетод формирует сигнал при активации элемента упрвалеия выравниванием
void MainWindow::TextAlign(QAction* AlignAction)
{
    Qt::Alignment alignment;
    if (AlignAction == _pAlignLeftAct)
    {
        alignment = (Qt::AlignLeft | Qt::AlignAbsolute);
    }
    else if (AlignAction == _pAlignCenterAct)
    {
        alignment = (Qt::AlignHCenter);
    }
    else if (AlignAction == _pAlignRightAct)
    {
        alignment = (Qt::AlignRight | Qt::AlignAbsolute);
    }
    else if (AlignAction == _pAlignJustifyAct)
    {
        alignment = (Qt::AlignJustify);
    }
    emit Alignment(alignment);
}

//Метод, вызываемый при изменении положения курсора для отображения установленного выравнивания
void MainWindow:: CursorPositionChanged()
{
    AlignmentChanged(_pCurrentDocument->alignment());
    QTextList *list = _pCurrentDocument->textCursor().currentList();
    if (list)
    {
        switch (list->format().style())
        {
        case QTextListFormat::ListDisc:
            _pComboFontStyle->setCurrentIndex(1);
            break;
        case QTextListFormat::ListCircle:
            _pComboFontStyle->setCurrentIndex(2);
            break;
        case QTextListFormat::ListSquare:
            _pComboFontStyle->setCurrentIndex(3);
            break;
        case QTextListFormat::ListDecimal:
            _pComboFontStyle->setCurrentIndex(6);
            break;
        case QTextListFormat::ListLowerAlpha:
            _pComboFontStyle->setCurrentIndex(7);
            break;
        case QTextListFormat::ListUpperAlpha:
            _pComboFontStyle->setCurrentIndex(8);
            break;
        case QTextListFormat::ListLowerRoman:
            _pComboFontStyle->setCurrentIndex(9);
            break;
        case QTextListFormat::ListUpperRoman:
            _pComboFontStyle->setCurrentIndex(10);
            break;
        default:
            _pComboFontStyle->setCurrentIndex(-1);
            break;
        }
    }
    else
    {
        int headingLevel = _pCurrentDocument->textCursor().blockFormat().headingLevel();
        _pComboFontStyle->setCurrentIndex(headingLevel ? headingLevel + 10 : 0);
    }
}

//Метод, отвечает за активацию элемента, соответствующего  установленному выравниванию
void MainWindow:: AlignmentChanged (Qt::Alignment alignment)
{
    if (alignment.testFlag(Qt::AlignLeft))
        _pAlignLeftAct->setChecked(true);
    else if (alignment.testFlag(Qt::AlignHCenter))
        _pAlignCenterAct->setChecked(true);
    else if (alignment.testFlag(Qt::AlignRight))
        _pAlignRightAct->setChecked(true);
    else if (alignment.testFlag(Qt::AlignJustify))
        _pAlignJustifyAct->setChecked(true);
}

//Mетод создает панель для изменения цвета фона
void MainWindow::SetupBackgroundColorActions(QToolBar* pToolBar, QMenu* pMenu)
{
    _pActionBackgroundColor = new QAction();
    _pActionBackgroundColor->setShortcut(QKeySequence("Ctrl+Shift+B"));
    ColorBackgroundChanged(QColor(255,255,255));
    pMenu->addAction(_pActionBackgroundColor);
    pToolBar->addAction(_pActionBackgroundColor);
}

//Устанавливает настройки, соответствующие цвету фонв
void MainWindow::ColorBackgroundChanged(const QColor &color)
{
    QPixmap pix = QPixmap(":/images/icons/fill_bg_p.png");
    auto mask = pix.createMaskFromColor(QColor(0,255,0), Qt::MaskOutColor);
    QPainter painter;
    painter.begin(&pix);
    painter.setPen(color);
    painter.drawPixmap(pix.rect(), mask, mask.rect());
    painter.end();
    _pActionBackgroundColor->setIcon(pix);
}

// Метод создает панель для изменения семейства шрифта
void MainWindow::SetupFontFamily(QToolBar* pFontToolBar)
{
    _pComboFont = new QFontComboBox(pFontToolBar);
    pFontToolBar->addWidget(_pComboFont);
    _pComboFont->setEditable(false);
    _pComboFont->setFixedHeight(26);
    QFont chFont = _pComboFont->font();
    chFont.setPointSize(10);
//    chFont.setBold(true);
    _pComboFont->setFont(chFont);
}

// Метод создает панель для изменения стиля шрифта
void MainWindow::SetupFontStyle(QToolBar* pFontToolbar)
{
    _pComboFontStyle = new QComboBox(pFontToolbar);
    pFontToolbar->addWidget(_pComboFontStyle);
    _pComboFontStyle->setFixedHeight(26);
    _pComboFontStyle->setFixedWidth(300);

    QFont chFont = _pComboFontStyle->font();
    chFont.setPointSize(10);
    _pComboFontStyle->setFont(chFont);

    _pComboFontStyle->addItems({tr("Standard"),
                                tr("Bullet List (Disc)"),
                                tr("Bullet List (Circle)"),
                                tr("Bullet List (Square)"),
                                tr("Task List (Unchecked)"),
                                tr("Task List (Checked)"),
                                tr("Ordered List (Decimal)"),
                                tr("Ordered List (Alpha lower)"),
                                tr("Ordered List (Alpha upper)"),
                                tr("Ordered List (Roman lower)"),
                                tr("Ordered List (Roman upper)"),
                                tr("Heading 1"),
                                tr("Heading 2"),
                                tr("Heading 3"),
                                tr("Heading 4"),
                                tr("Heading 5"),
                                tr("Heading 6")});
}
