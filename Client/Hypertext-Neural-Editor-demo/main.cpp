/****************************************
* Команда 2
* Гипертекстовый редактор
*
* Точка входа приложения - функция main
*****************************************/

#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QScreen>
#include <QCommandLineParser>
//#include <QCommandLineOption>

int main(int argc, char *argv[])
{
    QApplication appl(argc, argv);
    QCoreApplication::setApplicationName("Hypertext Neural Editor");
    QCoreApplication::setOrganizationName("LLM Technologies");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    // Средства интернационализации
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale: uiLanguages)
    {
        const QString baseName = "Hypertext-Neural-Editor_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName))
        {
            appl.installTranslator(&translator);
            break;
        }
    }

    // Инициализация парсера параметров командной строки
    QCommandLineParser parser;
    parser.setApplicationDescription("China Travel Guide");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file to open.");
    parser.process(appl);

    MainWindow hypertextEditor;  // Создание экземпляра класса главного окна

    // Открываем файлы, переданные в командной строке
    const QStringList posArgs = parser.positionalArguments();
    for (const QString &fileName : posArgs)
        hypertextEditor.OpenFile(fileName);

    // Задание оптимальных размеров окна
    const QRect availableGeometry = hypertextEditor.screen()->availableGeometry();
    hypertextEditor.resize(availableGeometry.width() / 2,
                      (availableGeometry.height() * 2) / 3);
    hypertextEditor.move((availableGeometry.width() - hypertextEditor.width()) / 2,
            (availableGeometry.height() - hypertextEditor.height()) / 2);

    hypertextEditor.show();
    return appl.exec();
}
