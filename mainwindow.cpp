#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //Init lan check
    fileTypes.append(tr("Hardware"));
    fileTypes.append(tr("Geometry"));
    fileTypes.append(tr("Screen"));
    foreach (QString entry, fileTypes)
    {
        rbFileTypes.append(new QRadioButton(entry));
        ui->gbType->layout()->addWidget(rbFileTypes.last());
        //rbFileTypes.last()->setEnabled(false);
        connect(rbFileTypes.last(),SIGNAL(clicked()),this,SLOT(TypeChanged()));
    }


    GetConfModel();
    FindTeminals();
    file.type = 0;
    file.fillLabels = true;
    if(ui->lwTerm->count() > 0)
        on_lwTerm_itemDoubleClicked(ui->lwTerm->item(0));
    rbFileTypes.at(0)->setChecked(true);


    lbStatus = new QLabel(this);
    statusBar()->addPermanentWidget(lbStatus, 0);
    lbStatus->setMinimumWidth(120);
    //lbStatus->setVisible(false);

    pbProcess = new QProgressBar(this);
    pbProcess->setTextVisible(false);
    statusBar()->addPermanentWidget(pbProcess, 1);
    //pbProcess->setVisible(false);

    // window preset
    this->setWindowTitle(TITLE_MAIN);
    // dialog preset
    wDlgSet = new WDialogSet(this);
    connect(wDlgSet, SIGNAL(SendData(QString)), this, SLOT(SetStart(QString)));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::GetConfModel()
{
    //tcms.confPath = "c:/VC/conf";
#ifdef WIN32
    tcms.confPath = "c:/VoiceComm/conf";
#else
    tcms.confPath = "/data/etc/Mega/VoiceComm/conf";
#endif

    CCfgConf tcmsConf;
    tcmsConf.parseFile(tcms.confPath + "/tcms.conf");
    tcms.overallScreen = tcmsConf.getValue("", "overall_screen").toInt();
    tcms.screenRegistry = tcmsConf.getValue("", "screen_regestry").contains("1");
    rbFileTypes.at(1)->setEnabled(!tcms.screenRegistry);
    qDebug() << "&&&" << tcms.overallScreen << tcms.screenRegistry;
}

void MainWindow::FindTeminals()
{
    QString screensSuf = "screens";
    QDir conf(tcms.confPath);
    QStringList lstDirs = conf.entryList(QDir::Dirs |
                                         QDir::AllDirs |
                                         QDir::NoDotAndDotDot);

    qDebug() << "dir size " << lstDirs.size();
    CTerminal tmp;
    tmp.screen = 0;
    bool used;

    foreach(QString entry, lstDirs)
    {
        if(entry != screensSuf)
        {
            used = false;
            CCfgConf hardware;
            hardware.parseFile(tcms.confPath + "/" + entry + "/hardware.conf");
            //Host class select
            QString pr = entry.left(2);
            if(pr == "d_" || pr == "w_")
            {
                QString interface = GetSection( "Interface", 1);
                quint16 type = hardware.getValue(interface, "type").toInt();
                if(type == 301 || type == 303 || type == 304)
                {
                    tmp.name = hardware.getValue(interface, "name").mid(1);
                    tmp.path = tcms.confPath + "/" + entry;
                    tmp.host = entry;
                    tmp.screen = hardware.getValue(interface, "screen_regestry").toInt();
                    used = true;
                }
            }
            //find legacy screen number
            if(!tcms.screenRegistry && used)
            {
                if(tcms.overallScreen > 0)
                {
                    tmp.screen = tcms.overallScreen;
                }
                else
                {
                    //get screen number from geometry
                    CCfgConf geometry;
                    geometry.parseFile(tcms.confPath + "/" + entry + "/geometry.conf");
                    tmp.screen = geometry.getValue("", "screen").toInt();
                }
            }
            if(used)
            {
                //qDebug() << "item " << tmp.name << tmp.path << tmp.host << tmp.screen;
                term[tmp.name] = tmp;
                ui->lwTerm->addItem(tmp.name);
            }
        }
    }
}

void MainWindow::ShowFile()
{
    QString path = GetPath(file.name);
    qDebug() << "file path: " << path;
    ui->lbFile->setText(path);
    ui->lwFile->clear();
    CCfgConf sample;
    sample.parseFile(path);
    file.map = sample.GetMap();
    QMap<QString,QString>::iterator it = file.map.begin();
    for(;it != file.map.end(); ++it)
    {
       ui->lwFile->addItem(it.key() + " = " + it.value());
    }
    if(ui->lwFile->count() > 0)
    {
        ui->lwFile->setCurrentRow(0);
    }
}

QString MainWindow::GetPath(const QString &name)
{
    QString path = "";
    switch (file.type)
    {
    case 0:
        path = term[name].path + "/hardware.conf";
        break;
    case 1:
        path = term[name].path + "/geometry.conf";
        break;
    case 2:
    {
        if(tcms.screenRegistry)
        {
            path = tcms.confPath + "/screens/screen" + QString("%1").arg(term[name].screen, 3, 10, QChar('0')) + ".conf";
        }
        else // !tcms.screenRegistry
        {
            path = term[name].path + GetSection( "screen", term[name].screen) + ".conf";
        }
        break;
    }
    default:
        break;
    }
    return path;
}

QString MainWindow::GetSection(const QString &name, quint8 i)
{
    QString s;
    s = QString::number(i);
    if(i<100)
    {
        s = "00" + s;
        s = s.right(2);
    }
    s = "/" + name + s;
    return s;
}



void MainWindow::TypeChanged()
{
    for(int i=0; i < rbFileTypes.count(); i++)
    {
        if(rbFileTypes[i]->isChecked())
        {
            file.type = i;
            qDebug() << "TypeChanged" << file.type;
            ShowFile();
        }
    }
}

void MainWindow::ShowWarning(const QString &msg)
{
    QMessageBox dlg(QMessageBox::Warning, TITLE_MAIN, msg, QMessageBox::Ok);
    dlg.setWindowFlags(dlg.windowFlags() | Qt::WindowStaysOnTopHint);
    dlg.setWindowIcon(QPixmap(":/new/icon/LITENING.ICO"));
    if(dlg.exec() == QMessageBox::Ok)
        return;

}

void MainWindow::on_pbSet_clicked()
{
    //Set init
    if(ui->lwTerm->selectedItems().count() == 0)
    {
        ShowWarning(tr("No selected termonals!"));
        return;
    }
    wDlgSet->show();
    wDlgSet->SetParamLabel(file.selParam, file.selVal);
}

void MainWindow::SetStart(QString newValue)
{
    //Set start
    qDebug() << "newValue" << newValue;
    ClearSummary();
    lbStatus->setText(tr("Set"));
    bool needReView = false;

    CSummaryItem tmpItem;
    tmpItem.value = newValue;
    if(file.selStr)
        tmpItem.value = "$" + tmpItem.value;

    for(quint16 i=0; i<ui->lwTerm->count(); i++)
    {
        pbProcess->setValue((i+1) * 100 / ui->lwTerm->count());
        if(ui->lwTerm->item(i)->isSelected())
        {
            CCfgConf item;
            item.SetOutputEndLine(EL_WIN);
            tmpItem.name = ui->lwTerm->item(i)->text();
            QString path = GetPath(term[tmpItem.name].name);
            item.parseFile(path);
            QMap<QString, QString> itemMap = item.GetMap();
            itemMap[file.selKey] = tmpItem.value;
            item.SetMap(itemMap);
            item.writeFile(path);
            if(file.name == tmpItem.name)
                needReView = true;
        }
    }

    if(needReView)
    {
        qDebug() << "needReView";
        quint32 row = ui->lwFile->currentRow();
        qint32 sbv = ui->lwFile->verticalScrollBar()->value();

        file.fillLabels = false;
        ShowFile();
        ui->lwFile->setCurrentRow(row);
        quint32 tmpRow = row;
        while(ui->lwFile->verticalScrollBar()->value() < sbv)
        {
            ui->lwFile->setCurrentRow(++tmpRow);
        }
        file.fillLabels = true;
        ui->lwFile->setCurrentRow(row);
        if(file.selVal != newValue)
        {
            qDebug() << "add fillLabels";
            on_lwFile_currentRowChanged(ui->lwFile->currentRow());
        }
    }
    CheckStart();
}

void MainWindow::CheckStart()
{
    //Check
    if(ui->lwTerm->selectedItems().count() == 0)
    {
        ShowWarning(tr("No selected termonals!"));
        return;
    }
    ClearSummary();
    lbStatus->setText(tr("Check"));

    QList<CSummaryItem> sList;
    CSummaryItem tmpItem;
    for(quint16 i=0; i<ui->lwTerm->count(); i++)
    {
        pbProcess->setValue((i+1) * 100 / ui->lwTerm->count());
        if(ui->lwTerm->item(i)->isSelected())
        {
            CCfgConf item;
            tmpItem.name = ui->lwTerm->item(i)->text();
            QString path = GetPath(term[tmpItem.name].name);
            item.parseFile(path);
            tmpItem.value = item.getValue(file.selKey, "");
            if(file.selStr)
                tmpItem.value = tmpItem.value.mid(1);
            sList.append(tmpItem);
        }
    }

    FillSummary(sList);
}

void MainWindow::on_pbCheck_clicked()
{
    //Check
    CheckStart();
}

void MainWindow::FillSummary(const QList<CSummaryItem> &sList)
{
    foreach (CSummaryItem item, sList)
    {
        quint8 row = ui->twSumm->rowCount();
        ui->twSumm ->setRowCount(row + 1);
        ui->twSumm->setItem(row, 0, new QTableWidgetItem(item.name));
        ui->twSumm->setItem(row, 1, new QTableWidgetItem(item.value));
        ui->twSumm->setRowHeight(row, 25);
    }
    bool identical = true;
    if(ui->twSumm->rowCount() > 1)
    {
        for(quint16 i=0; i<ui->twSumm->rowCount(); i++)
        {
            if(ui->twSumm->item(i, 1)->text() != ui->twSumm->item(0, 1)->text())
            {
                identical = false;
            }
        }
        identical? ui->lbResult->setText(tr("Identical")): ui->lbResult->setText(tr("Different")) ;
    }
    lbStatus->setText("");
    pbProcess->setValue(0);
}

void MainWindow::ClearSummary()
{
    if(ui->twSumm->rowCount() > 0)
    {
        //qDebug() << "clear 2" << ui->twSumm->rowCount();
        ui->twSumm->setRowCount(0);
        ui->twSumm->repaint();
        ui->lbResult->setText("");
    }
}

void MainWindow::on_lwFile_currentRowChanged(int currentRow)
{
    //currentRowChanged
    //qDebug() << "currentRow " << currentRow << "fillLabels" << file.fillLabels;
    if(currentRow < 0 || !file.fillLabels)
        return;
    qDebug() << "currentRow " << currentRow ;
    QStringList all = ui->lwFile->item(currentRow)->text().split(" = ");
    file.selKey = all.at(0);
    file.selVal = all.at(1);
    QStringList paramPath = all.at(0).split('\t');
    file.selParam = paramPath.at(1);
    file.selStr = file.selVal.startsWith('$');
    if(file.selStr)
    {
        file.selVal = file.selVal.mid(1);
        ui->lbType->setText(tr("string"));
    }
    else
    {
        ui->lbType->setText(tr("number"));
    }
    ui->lbSect->setText(paramPath.at(0));
    ui->lbParam->setText(file.selParam);
    ui->lbValue->setText(file.selVal);
}

void MainWindow::on_lwTerm_itemDoubleClicked(QListWidgetItem *item)
{
    //itemDoubleClicked
    ui->lbSelect->setText(QString("%1 [ %2 ]").arg(term[item->text()].host).arg(item->text()));
    file.name = item->text();
    qDebug() << "item " << item->text();
    ShowFile();
}

void MainWindow::on_pbAll_clicked()
{
    for(quint16 i=0; i<ui->lwTerm->count(); i++)
    {
        ui->lwTerm->item(i)->setSelected(true);
    }
}

void MainWindow::on_pbReset_clicked()
{
    for(quint16 i=0; i<ui->lwTerm->count(); i++)
    {
        ui->lwTerm->item(i)->setSelected(false);
    }
}
