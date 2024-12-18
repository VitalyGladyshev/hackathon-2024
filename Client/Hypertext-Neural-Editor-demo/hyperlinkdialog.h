/*********************************************************
* Команда 2
* Гипертекстовый редактор
*
* Файл заголовков класса окна диалога создания гиперссылки
**********************************************************/

#pragma once

#include <QDialog>

class MainWindow;

namespace Ui {
class HyperlinkDialog;
}

/*!
 * \brief The HyperlinkDialog Класс создания гиперссылки
 */
class HyperlinkDialog : public QDialog
{
    Q_OBJECT

public:
    /// Конструктор
    explicit HyperlinkDialog(QWidget *parent = nullptr);

    /// Деструктор
    ~HyperlinkDialog();

    /*!
     * \brief SetLineEditText Задать текст поля редактирования "Текст ссылки"
     * \param text Текст поля редактирования "Текст ссылки"
     */
    void SetLineEditText(QString text);

    /// Очистить поле текст ссылки
    void ClearText();

    /// Очистить поле "Цель ссылки"
    void ClearTarget();

private slots:
    /// Слот получить путь через диалог
    void GetPath();

    /// Слот добавить гиперссылку
    void SlotMakeHyperlink();

private:
    Ui::HyperlinkDialog *ui;        // Указатель на пользовательский интерфейс
    MainWindow* _mainWindow;        // указатель на главное окно
};
