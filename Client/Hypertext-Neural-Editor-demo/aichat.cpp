#include "aichat.h"
#include "ui_aichat.h"
// #include "settings.h"
#include "mainwindow.h"
#include "documentwindow.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextList>

AIChat::AIChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AIChat),
    _currentLLM("GigaChat"),
    _verbose(true),
    _pReply(nullptr)
{
    ui->setupUi(this);
    _mainWindow = dynamic_cast<MainWindow*>(parent);

    _mapModelsProperties[_currentLLM] = new AIModelData;
    _mapModelsProperties[_currentLLM]->modelType = AIModels::GigaChat;
    _mapModelsProperties[_currentLLM]->strOAuthPath =
        "https://ngw.devices.sberbank.ru:9443/api/v2/oauth";
    _mapModelsProperties[_currentLLM]->strBasePath =
        "https://gigachat.devices.sberbank.ru/api/v1";
    _mapModelsProperties[_currentLLM]->strToken = "";
    _mapModelsProperties[_currentLLM]->strModelName = "GigaChat-Pro";   // "GigaChat";  // "GigaChat:latest";
    _mapModelsProperties[_currentLLM]->temperature = 0.87;

    _pNetworkInterface = new QNetworkAccessManager(this);

    connect(ui->pushButtonProperties, SIGNAL(clicked(bool)),
            this, SLOT(SlotPropertiesButtonClicked()));
    connect(ui->pushButtonSend, SIGNAL(clicked(bool)),
            this, SLOT(SlotSendButtonClicked()));
    connect(_pNetworkInterface, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(SlotReceptionFinished(QNetworkReply*)));
    connect(ui->textEditPrompt, SIGNAL(textChanged()),
            this, SLOT(SlotClearPromptFormat()));

    // ui->textEditChat->setFontPointSize(12);
    ui->textEditChat->append("Путеводитель по Китаю");
    SetTextBoldAndColor(Qt::darkCyan);  //darkGreen);
    ui->textEditChat->append("Добро пожаловать в путешествие по Китаю");
    ui->textEditChat->append("");
}

AIChat::~AIChat()
{
    delete ui;

    //!!!
    if (_mapModelsProperties[_currentLLM])
        delete _mapModelsProperties[_currentLLM];
}

// Задать текст клавиши "Настройки" для перевода
void AIChat::SetPropertiesButtonText(const QString buttonText)
{
    ui->pushButtonProperties->setText(buttonText);
}

// Задать текст клавиши "Отправить" для перевода
void AIChat::SetSendButtonText(const QString buttonText)
{
    ui->pushButtonSend->setText(buttonText);
}

// Зпросить токен
// void AIChat::GetToken()
// {
//     const QUrl authUrlAuth { _mapModelsProperties[_currentLLM]->strOAuthPath };
//     const QString authorization { "Bearer " + Settings::GetInstance().GetGigaChatAuthorization() };
//     const QString payload { "scope=GIGACHAT_API_PERS" };

//     QNetworkRequest request(authUrlAuth);
//     request.setRawHeader("RqUID", "6f0b1291-c7f3-43c6-bb2e-9f3efb2dc98e");
//     request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
//     request.setRawHeader("Authorization", authorization.toLocal8Bit());

//     _pNetworkInterface->post(request, payload.toLocal8Bit());
//     qDebug() << "Token request";

//     disconnect(_pNetworkInterface, SIGNAL(finished(QNetworkReply*)),
//             this, SLOT(SlotReceptionFinished(QNetworkReply*)));
//     connect(_pNetworkInterface, SIGNAL(finished(QNetworkReply*)),
//             this, SLOT(SlotGetToken(QNetworkReply*)));
// }

// Задать цвет текста и сделать его жирным
void AIChat::SetTextBoldAndColor(QColor color)
{
    QTextCursor cursor = ui->textEditChat->textCursor();
    cursor.movePosition(QTextCursor::End);
    if (cursor.hasSelection())
        cursor.clearSelection();
    cursor.select(QTextCursor::LineUnderCursor);
    QTextCharFormat format;
    format.setForeground(color);
    format.setFontWeight(QFont::Bold);
    format.setFontPointSize(12);
    cursor.mergeCharFormat(format);
}

// Слот на клик клавиши "Настройки"
void AIChat::SlotPropertiesButtonClicked()
{
    qDebug() << "PropertiesButtonClicked";
}

// Слот на клик клавиши "Отправить"
void AIChat::SlotSendButtonClicked()
{
    if (_verbose)
    {
        QTextCursor cursor = ui->textEditChat->textCursor();

        cursor.beginEditBlock();

        ui->textEditChat->append("Пользователь");
        SetTextBoldAndColor(Qt::darkBlue);
        QTextBlockFormat blockFmt = cursor.blockFormat();
        blockFmt.setIndent(blockFmt.indent() + 2);
        cursor.setBlockFormat(blockFmt);

        ui->textEditChat->append(ui->textEditPrompt->toPlainText());
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.setBlockFormat(blockFmt);

        ui->textEditChat->append("");
        cursor.select(QTextCursor::LineUnderCursor);
        blockFmt.setIndent(blockFmt.indent() - 2);
        cursor.setBlockFormat(blockFmt);

        cursor.endEditBlock();
    }
    _verbose = true;

    // auto doc = _mainWindow->GetActiveDocumentWindow();
    // QString CurrentPageText = doc->GetContext();

    // const QString token { QString("Bearer ") + _mapModelsProperties[_currentLLM]->strToken };

//    const QUrl authUrlModels { _mapModelsProperties[_currentLLM]->strBasePath + "/models" };
//    QNetworkRequest requestList(authUrlModels);
//    requestList.setRawHeader("Authorization", token.toLocal8Bit());
//    _pNetworkInterface->get(requestList);

    // const QUrl authUrlCompletion { _mapModelsProperties[_currentLLM]->strBasePath + "/chat/completions"};
    // QJsonObject dataForMessage
    // {
    //     {"model",               _mapModelsProperties[_currentLLM]->strModelName},
    //     {"temperature",         _mapModelsProperties[_currentLLM]->temperature},
    //     {"top_p",               0.47},
    //     {"n",                   1},
    //     {"max_tokens",          512},
    //     {"repetition_penalty",  1.07},
    //     {"stream",              false},
    //     {"update_interval",     0},
    // };
    // QJsonObject systemMessage
    // {
    //     {"role", "system"},
    //     {"content", "Отвечай как путеводитель по Китаю. Используй контекст"}
    // };
    QJsonObject userMessage
    {
//        {"role", "user"},
//        {"content", ui->textEditPrompt->toPlainText()} // +
            //" Используй контекст: " + CurrentPageText.simplified()}
        {"content", ui->textEditPrompt->toPlainText()}
    };
    // QJsonArray messages;
    // messages.append(systemMessage);
    // messages.append(userMessage);
    // dataForMessage["messages"] = messages;

    QJsonDocument jsonDocument(userMessage);    // dataForMessage);
    qDebug() << "Текст промпта: " << ui->textEditPrompt->toPlainText() /*+ " Используй контекст: "
            + CurrentPageText.simplified()*/ << "\n";    //jsonDocument.toJson(QJsonDocument::Compact);

    QNetworkRequest requestCompletion(QUrl("http://192.168.0.19:9090/chat"));  //"http://172.28.168.76:9090/chat"));   //(authUrlCompletion);
    requestCompletion.setRawHeader("Content-Type", "application/json");
    //requestCompletion.setRawHeader("Authorization", token.toLocal8Bit());

    _pReply = _pNetworkInterface->post(requestCompletion, jsonDocument.toJson(QJsonDocument::Compact));
    connect(_pReply, &QNetworkReply::readyRead, this, &AIChat::SlotReadyRead);

    ui->textEditChat->append("Путеводитель по Китаю");
    SetTextBoldAndColor(Qt::darkCyan);
    ui->textEditChat->append("");
    ui->textEditPrompt->clear();
}

// Слот на окончание передачи
void AIChat::SlotReceptionFinished(QNetworkReply* pReply)
{
    disconnect(_pReply, &QNetworkReply::readyRead, this, &AIChat::SlotReadyRead);

    auto replyData = pReply->readAll();

    QJsonDocument jsonResponse = QJsonDocument::fromJson(replyData);
    QJsonObject jsonObject = jsonResponse.object();

    if (pReply->error() != QNetworkReply::NoError)
    {
        qDebug() << "ReceptionFinished error!";

        qDebug() << pReply->rawHeaderList();
        qDebug() << pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qDebug() << pReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        qDebug() << pReply->errorString() << pReply->error();
        qDebug() << "Статус: " << jsonObject["status"].toInt();
        qDebug() << replyData;

        if (jsonObject["status"].toInt() == 401)
            // GetToken();
            qDebug() << "Error: 401";
        else if (jsonObject["status"].toInt() == 0)
        {
            QTime dieTime = QTime::currentTime().addMSecs(1000);
            while( QTime::currentTime() < dieTime )
            {
                QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
            }

            _verbose = false;
            // SlotSendButtonClicked();
        }
        else
        {
            ui->textEditChat->append("Путеводитель по Китаю");
            SetTextBoldAndColor(Qt::darkCyan);
            ui->textEditChat->append(QString(replyData));
            ui->textEditChat->append("");
        }
    }
    else
    {
        // ui->textEditChat->append("Путеводитель по Китаю");
        // SetTextBoldAndColor(Qt::darkCyan);

        auto answer = jsonObject["content"].toString();

        ui->textEditChat->append(answer);
        qDebug() << answer;

        // QJsonArray jsonArray = jsonObject["choices"].toArray();

        // for(auto value: jsonArray)
        // {
        //     QJsonObject objMessage = value.toObject();
        //     QJsonObject obj = objMessage["message"].toObject();
        //     ui->textEditChat->append(obj["content"].toString());
        // }
        ui->textEditChat->append("");

        // ui->textEditPrompt->clear();
    }

    pReply->deleteLater();
}

// Слот получения токена
// void AIChat::SlotGetToken(QNetworkReply *pReply)
// {
//     qDebug() << "Token reply";

//     auto replyData = pReply->readAll();

//     if (pReply->error() != QNetworkReply::NoError)
//     {
//         qDebug() << "Token reply error!";

//         qDebug() << pReply->rawHeaderList();
//         qDebug() << pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
//         qDebug() << pReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
//         qDebug() << pReply->errorString() << pReply->error();
//         qDebug() << replyData;

//         ui->textEditChat->append("Token reply error!");
//         SetTextBoldAndColor(Qt::red);
//         ui->textEditChat->append("");
//     }
//     else
//     {
//         QJsonDocument jsonResponse = QJsonDocument::fromJson(replyData);
//         QJsonObject jsonObject = jsonResponse.object();

//         _mapModelsProperties[_currentLLM]->strToken = jsonObject["access_token"].toString();
//         qDebug() << "Token reply success!";

//         _verbose = false;
//         SlotSendButtonClicked();
//     }

//     pReply->deleteLater();

//     disconnect(_pNetworkInterface, SIGNAL(finished(QNetworkReply*)),
//                this, SLOT(SlotGetToken(QNetworkReply*)));
//     connect(_pNetworkInterface, SIGNAL(finished(QNetworkReply*)),
//             this, SLOT(SlotReceptionFinished(QNetworkReply*)));
// }

// Очистить форматирование промпта
void AIChat::SlotClearPromptFormat()
{
    disconnect(ui->textEditPrompt, SIGNAL(textChanged()),
            this, SLOT(SlotClearPromptFormat()));

    QTextCursor cursor = ui->textEditPrompt->textCursor();
    if (cursor.hasSelection())
        cursor.clearSelection();
    cursor.select(QTextCursor::Document);

    QTextCharFormat format;
    // cursor.movePosition(QTextCursor::End);
    cursor.setCharFormat(format);   //QTextCharFormat());

    connect(ui->textEditPrompt, SIGNAL(textChanged()),
            this, SLOT(SlotClearPromptFormat()));
}

// Слот для вывода частей ответа
void AIChat::SlotReadyRead()
{
    auto replyData = _pReply->readAll();
    // qDebug() << "SlotReadyRead";
    qDebug() << "ReplyData: " << replyData;

    QJsonDocument jsonResponse = QJsonDocument::fromJson(replyData);

    QJsonObject jsonObject = jsonResponse.object();

    if (jsonObject.isEmpty())
        qDebug() << "jsonObject is empty!";
    // else
    //     qDebug() << "jsonObject: " << jsonObject;

    if (_pReply->error() != QNetworkReply::NoError)
    {
        qDebug() << "SlotReadyRead ReceptionFinished error!";
        disconnect(_pReply, &QNetworkReply::readyRead, this, &AIChat::SlotReadyRead);
    }
    else
    {
        // QJsonArray jsonArray = jsonObject["choices"].toArray();

        // for(auto value: jsonArray)
        // {
        //     qDebug() << "value: " << value;
        //     QJsonObject objMessage = value.toObject();
        //     QJsonObject obj = objMessage["delta"].toObject();
        //     ui->textEditChat->moveCursor(QTextCursor::End);
        //     // ui->textEditChat->insertPlainText(QString(replyData));
        //     ui->textEditChat->insertPlainText(obj["content"].toString());
        // }
        if (jsonObject["index"].toInt())
        {
            qDebug() << "Индекс страницы: " << jsonObject["index"].toInt();
            int pageName = jsonObject["index"].toInt() + 1;

            QString fileName;
            if (pageName < 10)
                fileName = ":/pages/00" + QString::number(pageName) + ".html";
            else if (pageName < 100)
                fileName = ":/pages/0" + QString::number(pageName) + ".html";
            else
                fileName = ":/pages/" + QString::number(pageName) + ".html";

            QString newFileName = QDir(QDir::currentPath()).filePath(fileName);
            qDebug() << newFileName;

            _mainWindow->OpenFile(newFileName);
        }
        if (jsonObject["content"].isString())
        {
            ui->textEditChat->moveCursor(QTextCursor::End);
            ui->textEditChat->insertPlainText(jsonObject["content"].toString());
        }
    }
    // qDebug() << "\n";
}
