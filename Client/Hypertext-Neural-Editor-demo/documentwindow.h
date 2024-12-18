/*************************************************
* Команда 2
* Гипертекстовый редактор
*
* Файл заголовков класса документа в шаблоне MDI
**************************************************/

#pragma once

#include <QTextBrowser>

class MainWindow;

/*!
 * \brief Класс документа в MDI шаблоне
 */
class DocumentWindow : public QTextBrowser
{
    Q_OBJECT

public:
    /// Конструктор
    DocumentWindow(QWidget* pParent = nullptr);

    /*!
     * \brief OpenFile Метод загрузки файла и чтения из него текста
     * \param fullFileName Полное имя файла: путь и имя
     * \return Признак успешного чтения файла
     */
    bool OpenFile(const QString& pathFileName);

    /*!
     * \brief GetPathFileName Геттер полного имени файла: путь и имя
     * \return Полное имя файла: путь и имя
     */
    QString GetPathFileName() const { return _pathFileName; }

    /*!
     * \brief Load Метод загрузки документа
     * \return Признак успешного чтения
     */
    static QString Load();

    /// Метод сохранение документа
    bool Save();

    /// Метод сохранить документ как
    bool SaveAs();

    /// Метод сохранение файла
    bool SaveFile(const QString&);

    /// Метод сохранить документ как *.odt
    void SaveAsOdt(const QString fileName);

    /*!
     * \brief TextBold установка жирного шрифта
     * \param checked включение/выключение жирного шрифта
     */
    void TextBold(bool checked);

    /*!
     * \brief TextBold установка подчеркнутого шрифта
     * \param checked включение/выключение подчеркнутого шрифта
     */
    void TextUnderline(bool checked);

    /*!
     * \brief TextBold установка курсивного шрифта
     * \param checked включение/выключение курсивного шрифта
     */
    void TextItalic(bool checked);

    /*!
     * \brief TextSize устанавливает размер шрифта
     * \param size размер шрифта в формате QString
     */
    void TextSize(const QString &size);

    /*!
     * \brief TextFamily устанавливает шрифт
     * \param f название шрифта
     */
    void TextFamily(const QString &f);

    /*!
     *\brief TextColor выбор цвета и присваивание его формату шрифта
     */
    void TextColor();

    /*!
     * \brief MergeFormatOnWordOrSelection установка формата текста в выделение или слово активного документа
     * \param format ссылка на форматирование
     */
    void MergeFormatOnWordOrSelection(const QTextCharFormat &format);

    /*!
     * \brief Find Поиск в тексте
     * \param searchRequest Поисковый запрос
     * \param wholeText Флаг поиска "Слово целиком"
     * \param caseSensitive Флаг поиска "С учётом регистра"
     * \param backward Флаг поиска "В обратном направлении"
     */
    void Find(QString searchRequest, bool wholeText, bool caseSensitive, bool backward);

    /*!
     * \brief GetSelectedText Возвращает выделенный текст
     * \return Выделенный текст
     */
    QString GetSelectedText();

    /*!
     * \brief GetContext Возвращает текст документа как plainText для использования в качестве контекста
     * \return Текст документа как plainText
     */
    QString GetContext();

    /*!
     * \brief MakeHyperlink Создание гипертекстовой ссылки
     * \param linkText Текст ссылки
     * \param linkTarget Целевой путь перехода
     */
    void MakeHyperlink(const QString linkText, QString linkTarget);

    /// Добавить изображение
    void AddImage();

    /*!
     * \brief TextAlign метод устанавливает выравнивание текста
     * \param alignment флаг Qt::Alignment
     */
    void TextAlign(Qt::Alignment alignment);

    /*!
     * \brief Indent метод увеличения отсутпа
     */
    void Indent();

    /*!
     * \brief Unindent метод уменьшения отсутпа
     */
    void Unindent();

    /*!
     * \brief ModifyIndentation метод изменение отсутпа
     * \param amount значение изменения отступа (положительное число -увеличение, отрицательно число - уменьшение)
     */
    void ModifyIndentation(int amount);

    /*!
     * \brief BackgroundColor установка цвета фона
     */
    void BackgroundColor();

    /*!
     * \brief TextStyle устанавливает стиль текста
     * \param styleIndex индекс стиля
     */
    void TextStyle(int styleIndex);

public slots:

    /// Установить расширение по умолчанию
    void SlotSetDefaultSuffix();

signals:
    /*!
     * \brief SignalStatusBarMessage Сигнал - выод сообщения в статусбаре главного окна
     */
    void SignalStatusBarMessage(QString);

    /*!
     * \brief IsOpen Сигнал открытия окна
     * \param str Путь открываемого окна
     */
    void IsOpen(QString str);

    /*!
     * \brief IsClose Сигнал закрытия окна
     * \param str Путь закрываемого окна
     */
    void IsClose(QString str);

protected:

    /*!
     * \brief closeEvent Перегруженный метод закрытия виджета
     * \param event Указатель на объект с параметрами события
     */
    void closeEvent(QCloseEvent *event) override;

    /*!
     * \brief keyPressEvent Перегруженный метод нажатия клавиши
     * \param e Указатель на объект клавиши
     */
    void keyPressEvent(QKeyEvent *e) override;

private:

    QString _pathFileName;              // полное имя файла: путь и имя
    bool isChanged = false;             // Флаг изменения файла
};
