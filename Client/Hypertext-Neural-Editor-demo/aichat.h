#ifndef AICHAT_H
#define AICHAT_H

#include <QWidget>

namespace Ui {
class AIChat;
}

class QNetworkAccessManager;
class QNetworkReply;
class QMdiArea;
class MainWindow;

/*!
 * \brief Перечисление AIModels - список моделей
 */
enum class AIModels
{
    GigaChat = 0,
    YandexGPT,
    ChatGPT,
    Llama
};

/*!
 * \brief Структура AIModelData - параметры языковых моделей
 */
struct AIModelData
{
    AIModels modelType;
    QString strModelName;
    QString strOAuthPath;
    QString strToken;
    QString strBasePath;
    double temperature;
};

/*!
 * \brief Класс AIChat ИИ чат
 */
class AIChat : public QWidget
{
    Q_OBJECT

public:
    /// Конструктор
    explicit AIChat(QWidget *parent = nullptr);
    /// Деструктор
    ~AIChat();

    /*!
     * \brief SetPropertiesButtonText Задать текст клавиши "Настройки" для перевода
     * \param buttonText Текст надписи
     */
    void SetPropertiesButtonText(const QString buttonText);

    /*!
     * \brief SetSendButtonText Задать текст клавиши "Отправить" для перевода
     * \param buttonText Текст надписи
     */
    void SetSendButtonText(const QString buttonText);

    /*!
     * \brief GetToken Зпросить токен
     */
    // void GetToken();

    /*!
     * \brief SetTextBoldAndColor Задать цвет текста и сделать его жирным
     * \param color Цвет текста
     */
    void SetTextBoldAndColor(QColor color);

public slots:

    /*!
     * \brief SlotPropertiesButtonClicked Слот на клик клавиши "Настройки"
     */
    void SlotPropertiesButtonClicked();

    /*!
     * \brief SlotSendButtonClicked Слот на клик клавиши "Отправить"
     */
    void SlotSendButtonClicked();

    /*!
     * \brief SlotReceptionIsOver Слот на окончание передачи
     */
    void SlotReceptionFinished(QNetworkReply* );

    /*!
     * \brief SlotGetToken Слот получения токена
     */
    // void SlotGetToken(QNetworkReply* );

    /*!
     * \brief SlotClearPromptFormat Очистить форматирование промпта
     */
    void SlotClearPromptFormat();

    /*!
     * \brief SlotReadyRead Слот для вывода частей ответа
     */
    void SlotReadyRead();

private:
    Ui::AIChat *ui;             // Указатель на пользовательский интерфейс
    QString _currentLLM;        // Текущая языковая модель (LLM)
    QMap<QString, AIModelData*> _mapModelsProperties;   // Свойства LLM
    QNetworkAccessManager* _pNetworkInterface;          // Указатель на объект сетевого интерфейса
    MainWindow* _mainWindow;    // Указатель на главное окно
    bool _verbose;              // Флаг вывода текста сообщения в чат
    QNetworkReply* _pReply;     // Ответ модели для вывода по частям
};

#endif // AICHAT_H
