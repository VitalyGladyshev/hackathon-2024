/****************************************
* Команда 2
* Гипертекстовый редактор
*
* Код класса диалога поиска
*****************************************/

#include <QMessageBox>

#include "finddialog.h"
#include "ui_finddialog.h"
#include "mainwindow.h"
#include "documentwindow.h"

// Конструктор
FindDialog::FindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindDialog)
{
    ui->setupUi(this);

    _mainWindow = dynamic_cast<MainWindow*>(parent);

    connect(ui->pushFindButton, SIGNAL(clicked(bool)), this, SLOT(SlotButtonFind()));
}

// Деструктор
FindDialog::~FindDialog()
{
    delete ui;
}

// Задать метку чекбокса "Текст полностью"
void FindDialog::SetWTCheckBoxLabel(QString label)
{
    ui->checkBoxWholeText->setText(label);
}

// Возвращает поисковый запрос
QString FindDialog::GetSearchRequest()
{
    return ui->lineEdit->text();
}

// Проверка флага "Слова целиком"
bool FindDialog::GetWTCheckBoxStatus()
{
    return ui->checkBoxWholeText->isChecked();
}

// Проверка флага "С учётом регистра"
bool FindDialog::GetCSCheckBoxStatus()
{
    return ui->checkBoxCaseSensitive->isChecked();
}

// Проверка флага "В обратном направлении"
bool FindDialog::GetBackwardCheckBoxStatus()
{
    return ui->checkBoxBackward->isChecked();
}

// Очистка строки поиска
void FindDialog::ClearRequest()
{
    ui->lineEdit->clear();
}

// Задать текст поля редактирования "Поиск"
void FindDialog::SetLineEditText(QString text)
{
    ui->lineEdit->setText(text);
}

// Слот - нажатие клавиши "Поиск"
void FindDialog::SlotButtonFind()
{
    if(!GetSearchRequest().isEmpty())
    {
        if(_mainWindow)
        {
            auto doc =_mainWindow->GetActiveDocumentWindow();
            if (doc)
                doc->Find(GetSearchRequest(),
                          GetWTCheckBoxStatus(),
                          GetCSCheckBoxStatus(),
                          GetBackwardCheckBoxStatus());
        }
    }
    else
        QMessageBox::warning(this, tr("Find text"), tr("Enter a search query"));
}
