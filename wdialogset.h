#ifndef WDIALOGSET_H
#define WDIALOGSET_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QDebug>

namespace Ui {
class WDialogSet;
}

class WDialogSet : public QDialog
{
    Q_OBJECT

public:
    explicit WDialogSet(QWidget *parent = 0);
    ~WDialogSet();


    void SetParamLabel(const QString &param, const QString &defValue);


signals:
    void SendData(QString newValue);

private:
    Ui::WDialogSet *ui;

    void accept();
};

#endif // WDIALOGSET_H
