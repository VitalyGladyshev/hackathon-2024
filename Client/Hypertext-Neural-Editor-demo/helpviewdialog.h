/*************************************************
* Команда 2
* Гипертекстовый редактор
*
* Файл заголовков класса окна диалога помощи
**************************************************/

#pragma once

#include <QDialog>

namespace Ui {
class HelpViewDialog;
}

/*!
 * \brief The HelpViewDialog Класс диалога помощи
 */
class HelpViewDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     * \brief HelpViewDialog Конструктор
     * \param sFile Стартовый файл справки
     * \param sPath Путь к справке
     * \param parent Указатель на объект предок
     */
    explicit HelpViewDialog(const QString& sFile, QStringList sPaths = {":/"},
                            QWidget *parent = nullptr);

    /// Деструктор
    ~HelpViewDialog();

private:
    Ui::HelpViewDialog *ui;             // Указатель на пользовательский интерфейс
};
