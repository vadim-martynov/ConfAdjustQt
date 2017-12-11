#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDir>
#include <QListWidgetItem>
#include <QRadioButton>
#include <QLabel>
#include <QProgressBar>
#include <QLayout>
#include <QScrollBar>
#include <QMessageBox>

#include "xmllibrary.h"
#include "wdialogset.h"



#define TITLE_MAIN          "ConfAdjustQt v.2.0.2"
/*
* 2.0.1 betta
* 2.0.2 force Win CrLf in xmllibrary
*
* release 2017-12-06
*/




struct CTerminal
{
    QString path;
    QString host;
    QString name;
    quint16 screen;
};

struct CTcmsData
{
    quint8 overallScreen;
    bool screenRegistry;
    QString confPath;
};

struct CCurrentFile
{
    quint8 type;
    QString name;
    QMap<QString, QString> map;
    QString selKey;
    QString selVal;
    QString selParam;
    bool selStr;
    bool fillLabels;
};

struct CSummaryItem
{
    QString name;
    QString value;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pbSet_clicked();
    void on_pbCheck_clicked();
    void on_lwFile_currentRowChanged(int currentRow);
    void on_lwTerm_itemDoubleClicked(QListWidgetItem *item);
    void on_pbAll_clicked();
    void on_pbReset_clicked();

    void TypeChanged();
    void SetStart(QString newValue);

private:
    Ui::MainWindow *ui;

    WDialogSet *wDlgSet;

    QMap<QString, CTerminal> term;
    QStringList fileTypes;
    QList<QRadioButton *> rbFileTypes;

    QLabel *lbStatus;
    QProgressBar *pbProcess;

    CTcmsData tcms;
    CCurrentFile file;

    void GetConfModel();
    void FindTeminals();
    QString GetSection(const QString &name, quint8 i);
    void ShowFile();
    QString GetPath(const QString &name);
    void CheckStart();
    void ClearSummary();
    void FillSummary(const QList<CSummaryItem> &sList);
    void ShowWarning(const QString &msg);
};

#endif // MAINWINDOW_H
