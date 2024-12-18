/**********************************************
* Команда 2
* Гипертекстовый редактор
*
* Код класса окна диалога создания гиперссылки
***********************************************/

#include "mainwindow.h"
#include "documentwindow.h"
#include "hyperlinkdialog.h"
#include "ui_hyperlinkdialog.h"

#include <QFileDialog>
#include <QMessageBox>

HyperlinkDialog::HyperlinkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HyperlinkDialog)
{
    ui->setupUi(this);

    _mainWindow = dynamic_cast<MainWindow*>(parent);

    connect(ui->pushButtonOpenDialog, SIGNAL(clicked(bool)),
            this, SLOT(GetPath()));

    connect(ui->pushButtonHyperlink, SIGNAL(clicked(bool)),
            this, SLOT(SlotMakeHyperlink()));
}

HyperlinkDialog::~HyperlinkDialog()
{
    delete ui;
}

// Задать текст поля редактирования "Текст ссылки"
void HyperlinkDialog::SetLineEditText(QString text)
{
    ui->lineEditLinkText->setText(text);
}

// Очистить поле текст ссылки
void HyperlinkDialog::ClearText()
{
    ui->lineEditLinkText->clear();
}

// Очистить поле "Цель ссылки"
void HyperlinkDialog::ClearTarget()
{
    ui->lineEditLinkTarget->clear();
}

// Слот получить путь через диалог
void HyperlinkDialog::GetPath()
{
    QFileDialog fileDialog(this, tr("Choose target path"), QDir::currentPath());
    fileDialog.setOptions(QFileDialog::DontUseNativeDialog);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);

    QStringList mimeTypeFilters({"text/html"});
    fileDialog.setMimeTypeFilters(mimeTypeFilters);
    fileDialog.setDefaultSuffix("html");

    if (fileDialog.exec() != QDialog::Accepted)
        return;

    const QString pathFileName = fileDialog.selectedFiles().constFirst();

    if (pathFileName.isEmpty())
        return;

    ui->lineEditLinkTarget->setText(pathFileName);
}

// Слот добавить гиперссылку
void HyperlinkDialog::SlotMakeHyperlink()
{
    const QString linkText = ui->lineEditLinkText->text();
    QString linkTarget = ui->lineEditLinkTarget->text();

    bool succses = false;

    if(!linkText.isEmpty())
    {
        if(!linkTarget.isEmpty())
        {
            if(_mainWindow)
            {
                auto doc =_mainWindow->GetActiveDocumentWindow();
                if (doc)
                    doc->MakeHyperlink(linkText, ui->lineEditLinkTarget->text());
                succses = true;
            }
        }
        else
            QMessageBox::warning(this, tr("Make hyperlink"), tr("Enter a link target"));
    }
    else
        QMessageBox::warning(this, tr("Make hyperlink"), tr("Enter a link text"));

    if(succses)
        close();
}
