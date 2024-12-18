/***********************************************************
* Команда 2
* Гипертекстовый редактор
*
* Файл заголовков класса главного окна текстового редактора
************************************************************/

#pragma once

#include <QMainWindow>
#include <QPrinter>
#include <QTranslator>

#include "filemanager.h"

class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class DocumentWindow;
class QFontComboBox;
class QTextCharFormat;
class QTextList;
class QComboBox;
class FindDialog;
class AIChat;
class HyperlinkDialog;
class Settings;
class HelpViewDialog;
class QActionGroup;

/*!
 * \brief Класс главного окна текстового редактора
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /// Конструктор
    MainWindow(QWidget *parent = nullptr);

    /*!
     * \brief GetMdi Геттер указателя на объект QMdiArea главного окна
     * \return Возвращает указатель на объект QMdiArea главного окна
     */
    QMdiArea* GetMdi() { return _pMdiArea; }

    /*!
     * \brief OpenFile Метод открытия файла в дочернем окне
     * \param fullFileName Полное имя файла: путь и имя
     * \return Признак успешного чтения файла
     */
    bool OpenFile(const QString& pathFileName);

    /*!
     * \brief CreateNewDocument Метод создаёт экземпляр дочернего MDI окна документа
     * \return Возвращает экземпляр дочернего MDI окна документа
     */
    DocumentWindow* CreateNewDocument();

    /*!
     * \brief GetActiveDocumentWindow Возвращает указатель на текущий активный документ
     * \return Указатель на текущий активный документ
     */
    DocumentWindow* GetActiveDocumentWindow();

    /*!
     * \brief getListPath Возвращает указатель на список путей
     */
    QList<QString>* getListPath(){return _pListPath;}

protected:
    /*!
     * \brief closeEvent Перегруженный метод закрытия виджета
     * \param event Указатель на объект с параметрами события
     */
    void closeEvent(QCloseEvent *event) override;

    /*!
     * \brief changeEvent Перегруженный метод обработки события смены
     * \param pEvent Указатель на объект с параметрами события
     */
    void changeEvent(QEvent* pEvent) override;

private:

    /*!
     * \brief SetInterfaceLabels Устанавливаем все надписи интерфейса
     */
    void SetInterfaceLabels();

    /*!
     * \brief FontChanged метод формирует отобажение конфигурации шрифта в toolbar главного окна в соответствии с принятым шрифтом
     * \param f шрифт
     */
    void FontChanged(const QFont &f);

    /*!
     * \brief SetupFormatActions метод создает панели и меню форматирования текста
     * \param menu Указатель на меню
     */
    void SetupFormatActions(QMenu* menu);

    /*!
     * \brief SetupFontActions метод создает панели и меню конфигурирования шрифта
     */
    void SetupFontActions();

    /*!
     * \brief CreateActions Создаём объекты действий
     */
    void CreateActions();

    /*!
     * \brief SetupLanguageActions Создаём меню переключения языков
     * \param menu Меню настроек
     */
    void SetupLanguageActions(QMenu* menu);

    /*!
     * \brief SetupThemeActions Создаём меню переключения тем интерфейса
     * \param menu Меню настроек
     */
    void SetupThemeActions(QMenu* menu);

    /*!
     * \brief SetupBoldActions метод создает панели и меню конфигурирования включения/выключения жирного шрифта
     * \param toolBar указатель на тулбар
     * \param menu указатель на меню
     */
    void SetupBoldActions(QToolBar* toolBar, QMenu* menu);

    /*!
     * \brief SetupBoldActions метод создает панели и меню конфигурирования включения/выключения курсивного шрифта
     * \param toolBar указатель на тулбар
     * \param menu указатель на меню
     */
    void SetupItalicActions(QToolBar* toolBar, QMenu* menu);

    /*!
     * \brief SetupBoldActions метод создает панели и меню конфигурирования включения/выключения подчеркнутого шрифта
     * \param toolBar указатель на тулбар
     * \param menu указатель на меню
     */
    void SetupUnderLineActions(QToolBar* toolBar, QMenu* menu);

    /*!
     * \brief SetupSizeActions метод создает панель изменения размера шрифта
     * \param toolBar ссылка на тулбар
     */
    void SetupSizeActions(QToolBar* toolBar);

    /*!
     * \brief SetupFontColorActions метод создает панель изменения цвета шрифта
     * \param toolBar ссылка на тулбар
     * \param menu ссылка на меню
     */
    void SetupFontColorActions(QToolBar* toolBar,QMenu* menu);

    /*!
     * \brief SetupActiveDocument метод удаляет не актуальные соединения сигнал/слот, получает указатель на активное окно класса DocumentWindow и формирует актуальные соединения сигнал/слот
     * \param window указатель на активное окно класса QMdiSubWindow
     */
    void SetupActiveDocument(QMdiSubWindow* window);

    /*!
     * \brief ConnectToActiveDocument метод создания соединений панели и меню конигурирования шрифтов с активным окном класса DocumentWindow
     */
    void ConnectToActiveDocument();

    /*!
     * \brief ConnectToActiveDocument метод удаления соединений панели и меню конигурирования шрифтов с уже неактивным окном класса DocumentWindow
     */
    void DisonnectFromDocument();

    /*!
     * \brief CurrentCharFormatChanged метод формирует отобажение формата текста в toolbar главного окна в соответствии с форматированием
     * \param format ссылка на форматирование
     */
    void CurrentCharFormatChanged(const QTextCharFormat &format);

    /*!
     * \brief ColorFontChanged метод отображения на тулбаре актуального цвета шрифта
     * \param color ссылка на объект QColor
     */
    void ColorFontChanged(const QColor &color);

    /*!
     * \brief SetupJustifyActions метод создает на панель и в меню элементы для упрваения выравниванием текста
     * \param toolBar ссылка на тулбар
     * \param menu сылка на меню
     */
    void SetupJustifyActions(QToolBar* toolBar, QMenu* menu);

    /*!
     * \brief SetupIndentActions метод создает на панель и в меню элементы для упрваения отсутпами текста
     * \param toolBar ссылка на тулбар
     * \param menu сылка на меню
     */
    void SetupIndentActions(QToolBar* toolBar, QMenu* menu);

    /*!
     * \brief TextAlign метод формирует сигнал о смене выравнивания при активации одного из элементов
     * \param alignAction ссылка на активированный элемент
     */
    void TextAlign(QAction *alignAction);

    /*!
     * \brief CursorPositionChanged метод устанавливает соответсвующие тексту параметры выравнивания при смене положения курсора
     */
    void CursorPositionChanged();

    /*!
     * \brief AlignmentChanged метод устанавливает соответсвующие тексту параметры выравнивания
     * \param alignment флаг Qt::Alignment
     */
    void AlignmentChanged(Qt::Alignment alignment);

    /*!
     * \brief SetupBackgroundColorActions метод создает панель изменения цвета фона
     * \param toolBar ссылка на тулбар
     * \param menu ссылка на меню
     */
    void SetupBackgroundColorActions(QToolBar* toolBar, QMenu* menu);

    /*!
     * \brief ColorBackgroundChanged метод отображения на тулбаре актуального цвета фона
     * \param color ссылка на объект QColor
     */
    void ColorBackgroundChanged(const QColor &color);

    /*!
     * \brief SetupFontFamily метод создает панель выбора шрифта
     * \param toolBar указатель тулбар
     */
    void SetupFontFamily(QToolBar* toolBar);

    /*!
     * \brief SetupFontStyle метод создает панель выбора стиля шрифта
     * \param toolBar указатель на тулбар
     */
    void SetupFontStyle(QToolBar* toolBar);

public slots:
    /*!
     * \brief SlotStatusBarMessage Слот - вывод сообщения в статусбаре главного окна
     */
    void SlotStatusBarMessage(QString);

    /*!
     * \brief SlotOnOpen Слот добавления пути в список путей
     * \param path Путь в формате строки
     */
    void SlotOnOpen(QString path);

    /*!
     * \brief SlotDelPath Слот удаления пути из списка путей
     * \param path Путь в формате строки
     */
    void SlotDelPath(QString path);

private slots:

    /// Слот создания нового документа
    void SlotNewDoc();

    /// Слот загрузки документа
    void SlotLoad();

    /// Слот сохранения документа
    void SlotSave();

    /// Слот активации действия сохранения документа
    void SlotSaveEnable();

    /// Слот сохранить документ как
    void SlotSaveAs();

    /// Слот вызова окна "О программе"
    void SlotAbout();

    /// Слот меню "Окна"
    void SlotWindows();

    /// Слот навигация назад
    void SlotBackward();

    /// Слот навигация Домой
    void SlotHome();

    /// Слот навигация вперёд
    void SlotForward();

    /// Слот вырезать текст
    void SlotCut();

    /// Слот копировать текст
    void SlotCopy();

    /// Слот вставить текст
    void SlotPaste();

    /// Слот печать документа
    void SlotPrint();

    /// Слот предварительного просмотра перед печатью документа
    void SlotPrintPreview();

    /*!
     * \brief SlotPrintPreviewDraw  Слот отображения для предварительного просмотра
     * \param printer Указатель на объект QPrinter
     */
    void SlotPrintPreviewDraw(QPrinter* printer);

    /// Слот сохранить документ в ODT
    void SlotSaveAsOdt();

    /// Слот сохранить документ в PDF
    void SlotPrintPDF();

    /// Слот поиск в тексте
    void SlotFind();

    /// Слот добавление гиперссылки
    void SlotMakeHyperlink();

    /// Слот добавить изображение
    void SlotAddImage();

    /// Слот показать справку
    void SlotHelp();

    /// Слот сделать активными/не активными эементы интерфеса, если документ открыт
    void SlotUpdateMenus();

    /*!
     * \brief SlotSetActiveSubWindow Слот делает дочернее MDI окно активным
     * \param QObject* Указатель на виджет дочернего MDI окна документа
     */
    void SlotSetActiveSubWindow(QObject*);

    /*!
     * \brief SlotSetActiveSubWindowByPath Слот делает дочернее MDI окно активным по имени
     * \param path Имя и путь к файлу в дочернем окне
     */
    void SlotSetActiveSubWindowByPath(QString path);

    /// Слот устанавливаем русский язык
    void SlotSetupRussianLanguage();

    /// Слот устанавливаем английский язык
    void SlotSetupEnglishLanguage();

    /// Слот устанавливаем светлую тему
    void SlotSetupLightTheme();

    /// Слот устанавливаем тёмную тему
    void SlotSetupDarkTheme();

signals:
    /*!
     * \brief Alignment Сигнал при выравнивании
     * \param alignment Флаг вида выравнивания
     */
    void Alignment(Qt::Alignment alignment);

private:
    int _iUnnamedIndex;                     // индекс для создания имён безымянных файлов

    QMdiArea* _pMdiArea;                    // указатель на MDI виджет
    QSignalMapper* _pSignalMapper;          // указатель на мапер сигналов
    FileManager* _pFileManager;             // указатель на FileManager - файловый менеджер
    QDockWidget* _pDocWidget;               // указатель на DocWidget файлового менеджера
    AIChat* _pAIChat;                       // указатель на AIChat - ИИ-чат
    QDockWidget* _pDocWidgetAI;             // указатель на DocWidget чата с LLM
    DocumentWindow* _pCurrentDocument;      // указатель на активный виджет класса DocumentWindow
    FindDialog* _pFindDialog;               // указатель на FindDialog - диалог поиска
    HyperlinkDialog* _pMakeLinkDialog;      // указатель на HyperlinkDialog - диалог добавления ссылки
    HelpViewDialog* _pShowHelpDialog;       // указатель на HelpViewDialog - диалог помощи
    QList<QString>* _pListPath;             // указатель на лист путей открытых файлов
    QList<QMdiSubWindow*> _pListSubWindow;  // Список путей открытых окон
    Settings* _pSettings;                   // указатель на объект настроек
    QTranslator _translator;                // объект для интернационализации

    QAction* _pActionTextBold;              // включение жирного шрифта
    QAction* _pActionTextUnderline;         // включение жирного подчеркнутого шрифта
    QAction* _pActionTextItalic;            // включение жирного курсивного шрифта
    QAction* _pActionTextColor;             // цвет шрифта
    QAction* _pActionBackgroundColor;       // цвет фона
    QFontComboBox* _pComboFont;             // выбор семейства шрифта
    QComboBox* _pComboSize;                 // выбор размер шрифта
    QComboBox* _pComboFontStyle;            // выбор стиля шрифта
    QAction* _pNewAct;                      // указатель на действие "Новый файл"
    QAction* _pOpenAct;                     // указатель на действие "Открыть файл"
    QAction* _pSaveAct;                     // указатель на действие "Сохранить"
    QAction* _pSaveAsAct;                   // указатель на действие "Сохранить как"
    QAction* _pBackwardAct;                 // указатель на действие "Назад"
    QAction* _pHomeAct;                     // указатель на действие "Домой"
    QAction* _pForwardAct;                  // указатель на действие "Вперёд"
    QAction* _pCutAct;                      // указатель на действие "Вырезать"
    QAction* _pCopyAct;                     // указатель на действие "Копировать"
    QAction* _pPasteAct;                    // указатель на действие "Вставить"
    QAction* _pUndoAct;                     // указатель на действие "Отменить"
    QAction* _pRedoAct;                     // указатель на действие "Вернуть"
    QAction* _pFindAct;                     // указатель на действие "Поиск"
    QAction* _pPrintAct;                    // указатель на действие "Печать файла"
    QAction* _pPrintPreviewAct;             // указатель на действие "Предпросмотр перед печатю файла"
    QAction* _pPrintPDFAct;                 // указатель на действие "Сохранить как PDF файл"
    QAction* _pCloseAct;                    // указатель на действие закрыть дочернее окно
    QAction* _pCloseAllAct;                 // указатель на действие закрыть всё дочерние окна
    QAction* _pNextAct;                     // указатель на действие "Следующая вкладка"
    QAction* _pPreviousAct;                 // указатель на действие "Предыдующая вкладка"
    QAction* _pSaveAsOdt;                   // указатель на действие "Сохранить как *.odt"
    QAction* _pMakeLinkAct;                 // указатель на действие добавить гиперссылку
    QAction* _pAddImageAct;                 // указатель на действие добавить изображение
    QAction* _pAboutAct;                    // указатель на действие "О программе"
    QAction* _pHelpAct;                     // указатель на действие "Помощь"
    QAction* _pQuitAct;                     // указатель не действие "Выход"
    QAction* _pSetRussian;                  // указатель не действие "Русский язык"
    QAction* _pSetEnglish;                  // указатель не действие "Английский язык"
    QAction* _pSetLightTheme;               // указатель не действие "Светлая тема"
    QAction* _pSetDarkTheme;                // указатель не действие "Тёмная тема"

    QAction* _pAlignLeftAct;                // указатель не действие "Выравнивание по левому краю"
    QAction* _pAlignCenterAct;              // указатель не действие "Выравнивание по центру"
    QAction* _pAlignRightAct;               // указатель не действие "Выравнивание по правому краю"
    QAction* _pAlignJustifyAct;             // указатель не действие "Выравнивание по ширине"
    QAction* _pActionIndentMoreAct;         // указатель не действие "Увеличение отступа"
    QAction* _pActionIndentLessAct;         // указатель не действие "Уменьшение отступа"

    QActionGroup *_pAlignGroup;             // указатель на группу действий форматирования

    QMenu* _pMenuWindows;                   // указатель на виджет меню
    QMenu* _pmnuFile;                       // указатель на виджет меню "Файл"
    QMenu* _pMenuEdit;                      // указатель на виджет меню "Правка" главного окна
    QMenu* _pMenuFormat;                    // указатель на виджет меню "Форматирование" главного окна
    QMenu* _pMenuSettings;                  // указатель на виджет меню "Настройки" главного окна
    QMenu* _pMenuHelp;                      // указатель на виджет меню "Помощь" главного окна

    QToolBar* _pToolBar;                    // указатель на Toolbar
    QToolBar* _pToolBarNavigation;          // указатель на Toolbar "Навигация"
    QToolBar* _pEditToolBar;                // указатель на Toolbar "Правка"
    QToolBar* _pHelpToolBar;                // указатель на Toolbar "Справка"
    QToolBar* _pFontToolbar;                // указатель на Toolbar "Шрифт"
    QToolBar* _pToolBarFormat;              // указатель на Toolbar "Форматирование"
};
