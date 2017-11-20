#include "wdialogset.h"
#include "ui_wdialogset.h"

WDialogSet::WDialogSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WDialogSet)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setModal(true);
}

WDialogSet::~WDialogSet()
{
    delete ui;
}

void WDialogSet::accept()
{
    emit SendData(ui->lineEdit->text());
    this->done(this->Accepted);
}


void WDialogSet::SetParamLabel(const QString &param, const QString &defValue)
{
    ui->label->setText(param);
    ui->lineEdit->setText(defValue);
    ui->lineEdit->selectAll();
    ui->lineEdit->setFocus();
}
