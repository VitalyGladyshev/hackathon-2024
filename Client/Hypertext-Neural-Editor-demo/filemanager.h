/***********************************************************
* Команда 2
* Гипертекстовый редактор
*
* Файл заголовков класса файлового менеджера
*
* Беляев Алексей
************************************************************/

#pragma once

#include <QFileSystemModel>
#include <QTreeView>
#include <QMouseEvent>

class MainWindow;

/*!
 * \brief Класс FileManager файловый менеджер
 */
class FileManager : public QTreeView
{
    Q_OBJECT
public:
    /// Конструктор
    explicit FileManager(QWidget *parent = nullptr);
    /*!
     * \brief mouseDoubleClickEvent Переопределенный метод двойного нажатия левой кнопки мыши открывает файл в Hypertext Neural Editor
     * \param event Указатель на объект с параметрами события манипулятора мышь
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    /*!
     * \brief SignalSetActive Сигнал сделать окно активным
     * \param str Путь и имя файла
     */
    void SignalSetActive(QString path);

private:
    QFileSystemModel* _fileSystem;  // указатель на модель системы
    MainWindow* _mainWindow;        // указатель на главное окно
};
