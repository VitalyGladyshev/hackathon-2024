/**************************************
* Команда 2
* Гипертекстовый редактор
*
* Код класса настроек
***************************************/

#include <QApplication>
#include <QFile>
#include <QDomDocument>

#include "settings.h"

Settings::Settings():
    _language(Language::Russian),
    _theme(Theme::Light),
    _gigaChatAuthorization(""),
    _yandexGPTAuthorization(""),
    _yandexGPTFolder("")
{
    if (!LoadSettings())
        SaveSettings();
}

// Задать значение настройки текущего языка интерфейса
void Settings::SetLanguage(const Language language)
{
    _language = language;

    SaveSettings();
}

// Задать значение настройки текущей темы интерфейса
void Settings::SetTheme(const Theme theme)
{
    _theme = theme;

    SaveSettings();
}

// Задать авторизационные данные к GigaChat
void Settings::SetGigaChatAuthorization(const QString strGigaChatAuthorization)
{
    _gigaChatAuthorization = strGigaChatAuthorization;

    SaveSettings();
}

// Задать авторизационные данные к YandexGPT2
void Settings::SetYandexGPTAuthorization(const QString strYandexGPTAuthorization)
{
    _yandexGPTAuthorization = strYandexGPTAuthorization;

    SaveSettings();
}

// Задать id каталога в Яндекс Облаке
void Settings::SetYandexGPTFolder(const QString strYandexGPTFolder)
{
    _yandexGPTFolder = strYandexGPTFolder;

    SaveSettings();
}

// Загрузка настроек
bool Settings::LoadSettings()
{
    QFile settingsFile(QApplication::applicationDirPath() + "/settings.xml");
    if(settingsFile.open(QIODevice::ReadOnly))
    {
        bool error = false;
        QDomDocument docSet;
        if (docSet.setContent(&settingsFile))
        {
            QDomElement domEl = docSet.documentElement();
            QDomNode domNode = domEl.firstChild();
            QDomElement domGetEl = domNode.toElement();

            if (!domGetEl.isNull())
            {
                int iLng = domGetEl.text().toInt();
                if (iLng >= 0 && iLng <= 1)
                    _language = static_cast<Language>(iLng);
                else
                {
                    error = true;
                    _language = Language::Russian;
                }
            }

            domNode = domNode.nextSibling();
            domGetEl = domNode.toElement();
            if (!domGetEl.isNull())
            {
                int iThm = domGetEl.text().toInt();
                if (iThm >= 0 && iThm <= 1)
                    _theme = static_cast<Theme>(iThm);
                else
                {
                    error = true;
                    _theme = Theme::Light;
                }
            }

            domNode = domNode.nextSibling();
            domGetEl = domNode.toElement();
            if (!domGetEl.isNull())
                _gigaChatAuthorization = domGetEl.text();

            domNode = domNode.nextSibling();
            domGetEl = domNode.toElement();
            if (!domGetEl.isNull())
                _yandexGPTAuthorization = domGetEl.text();

            domNode = domNode.nextSibling();
            domGetEl = domNode.toElement();
            if (!domGetEl.isNull())
                _yandexGPTFolder = domGetEl.text();
        }
        else
            error = true;
        settingsFile.close();

        if(error)
            qDebug() << "Read file structure error!";
        return !error;
    }
    else
    {
        qDebug() << "Read file error!";
        return false;
    }
}

// Сохранение настроек
bool Settings::SaveSettings()
{
    QFile settingsFile(QApplication::applicationDirPath() + "/settings.xml");

    if (settingsFile.open(QIODevice::WriteOnly))
    {
        QDomDocument doc("hypertext");
        QDomElement settings = doc.createElement("settings");
        doc.appendChild(settings);

        QDomElement language = doc.createElement("language");
        QDomText domTextLanguage = doc.createTextNode(QString("%1").arg(static_cast<int>(_language)));
        language.appendChild(domTextLanguage);
        settings.appendChild(language);

        QDomElement theme = doc.createElement("theme");
        QDomText domTextTheme = doc.createTextNode(QString("%1").arg(static_cast<int>(_theme)));
        theme.appendChild(domTextTheme);
        settings.appendChild(theme);

        QDomElement domElGigaChatAuthorization = doc.createElement("gigachat_authorization");
        QDomText domTextGigaChatAuthorization = doc.createTextNode(_gigaChatAuthorization);
        domElGigaChatAuthorization.appendChild(domTextGigaChatAuthorization);
        settings.appendChild(domElGigaChatAuthorization);

        QDomElement domElYandexGPTAuthorization = doc.createElement("yandex_authorization");
        QDomText domTextYandexGPTAuthorization = doc.createTextNode(_yandexGPTAuthorization);
        domElYandexGPTAuthorization.appendChild(domTextYandexGPTAuthorization);
        settings.appendChild(domElYandexGPTAuthorization);

        QDomElement domElYandexGPTFolder = doc.createElement("yandex_folder");
        QDomText domTextYandexGPTFolder = doc.createTextNode(_yandexGPTFolder);
        domElYandexGPTFolder.appendChild(domTextYandexGPTFolder);
        settings.appendChild(domElYandexGPTFolder);

        doc.appendChild(settings);
        QTextStream(&settingsFile) << doc.toString();
        settingsFile.close();
        return true;
    }
    else
    {
        qDebug() << "Save file error!";
        return false;
    }
}
