/****************************************
* Команда 2
* Гипертекстовый редактор
*
* Код класса диалога помощи
*****************************************/

#include "helpviewdialog.h"
#include "ui_helpviewdialog.h"

HelpViewDialog::HelpViewDialog(const QString& sFile, QStringList sPaths,
                               QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpViewDialog)
{
    ui->setupUi(this);

    ui->textBrowserHelp->setSearchPaths(sPaths);
    ui->textBrowserHelp->setSource(sFile);
    ui->textBrowserHelp->setOpenExternalLinks(true);
    ui->textBrowserHelp->setReadOnly(true);

    connect(ui->pushButtonBackward, SIGNAL(clicked(bool)),
            ui->textBrowserHelp, SLOT(backward()));
    connect(ui->pushButtonHome, SIGNAL(clicked(bool)),
            ui->textBrowserHelp, SLOT(home()));
    connect(ui->pushButtonForward, SIGNAL(clicked(bool)),
            ui->textBrowserHelp, SLOT(forward()));

    connect(ui->textBrowserHelp, SIGNAL(backwardAvailable(bool)),
            ui->pushButtonBackward, SLOT(setEnabled(bool)));
    connect(ui->textBrowserHelp, SIGNAL(forwardAvailable(bool)),
            ui->pushButtonForward, SLOT(setEnabled(bool)));
}

HelpViewDialog::~HelpViewDialog()
{
    delete ui;
}
