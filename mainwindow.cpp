#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList arg = QCoreApplication::arguments();
    statusBarVisible=true;

    tabEdit = new MultiTabEdit(this);
    loadSettings();

    connect(QApplication::clipboard(),&QClipboard::dataChanged,this,&MainWindow::clipboardChanged);
    connect(tabEdit,&MultiTabEdit::tabChange,this,&MainWindow::tabChange);
    connect(tabEdit,&MultiTabEdit::tabTextChange,this,&MainWindow::tabTextChange);
    connect(tabEdit,&MultiTabEdit::tabTextCurChange,this,&MainWindow::tabTextCurChange);
    connect(tabEdit,&MultiTabEdit::tabTextCopyAvaible,this,&MainWindow::tabTextCopyAvaible);
    connect(tabEdit,&MultiTabEdit::tabTextRedoAvaible,this,&MainWindow::tabTextRedoAvaible);
    connect(tabEdit,&MultiTabEdit::tabTextUndoAvaible,this,&MainWindow::tabTextUndoAvaible);

    tabEdit->tabChange(tabEdit->getCurrentTabIndex());
    tabEdit->move(QPoint(0,60));

    searchBarVisible=false;
    replaceBarVisible=false;

    if(arg.count() > 1)
        tabEdit->addTab(arg.at(1));
}

void MainWindow::clipboardChanged()
{
    if(tabEdit->tabExists())
    {
        if(curPlainTextEdit->canPaste())
        {
            ui->actionPrilepi->setEnabled(true);
            ui->toolButtonPaste->setEnabled(true);
        }
        else
        {
            ui->actionPrilepi->setEnabled(false);
            ui->toolButtonPaste->setEnabled(false);
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    if(statusBarVisible)
        ui->statusWidget->move(QPoint(0,event->size().height()-40));
    else
        ui->statusWidget->move(QPoint(0,event->size().height()-20));

    ui->statusWidget->resize(QSize(event->size().width(),20));
    ui->toolbarWidget->resize(QSize(event->size().width(),30));
    ui->toolButtonSwitch->move(QPoint(ui->statusWidget->width()-30,ui->statusWidget->pos().y()-25));

    if(!searchBarVisible)
    {
        ui->replaceWidget->move(QPoint(0,ui->statusWidget->pos().y()));
        ui->searchWidget->move(QPoint(0,ui->replaceWidget->pos().y()));
        ui->toolButtonSwitch->move(QPoint(event->size().width()-30,event->size().height()));
        tabEdit->resize(QSize(event->size().width()-5,ui->searchWidget->pos().y()-50));
    }
    else if(!replaceBarVisible)
    {
        ui->replaceWidget->move(0,ui->statusWidget->pos().y());
        ui->searchWidget->move(0,ui->replaceWidget->pos().y()-35);
        tabEdit->resize(QSize(event->size().width()-5,ui->searchWidget->pos().y()-40));
    }
    else
    {
        ui->replaceWidget->move(0,ui->statusWidget->pos().y()-30);
        ui->searchWidget->move(0,ui->replaceWidget->pos().y()-30);
        tabEdit->resize(QSize(event->size().width()-5,ui->searchWidget->pos().y()-40));
    }

    ui->statusLine->resize(QSize(event->size().width(),2));
    ui->statusLine->move(QPoint(0,ui->statusWidget->pos().y()-1));
}


MainWindow::~MainWindow()
{
    delete ui;
    delete set;
    disconnect(tabEdit,&MultiTabEdit::tabChange,this,&MainWindow::tabChange);
    delete tabEdit;
}

void MainWindow::on_actionRazveljavi_triggered()
{
    if(tabEdit->tabExists())
        curPlainTextEdit->undo();
}

void MainWindow::on_actionPonovi_triggered()
{
    if(tabEdit->tabExists())
        curPlainTextEdit->redo();
}

void MainWindow::on_actionIzhod_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionShrani_triggered()
{
    curFilename = tabEdit->getFileName(curIndex);
    if (curFilename.isEmpty())
        curFilename = QFileDialog::getSaveFileName(this,tr("Shrani datoteko"),"*",tr("Tekstovne datoteke (*.txt);;Vse datoteke (*.*)"));

    if(curFilename.isEmpty())
        return;

    save();
}

void MainWindow::on_actionShrani_kot_triggered()
{
    curFilename=tabEdit->getFileName(curIndex);
    curFilename = QFileDialog::getSaveFileName(this,tr("Shrani datoteko"), curFilename,tr("Tekstovne datoteke (*.txt);;Vse datoteke (*.*)"));

    if(curFilename.isEmpty())
        return;

    save();
}

bool MainWindow::save(QString filename,QString text,int index)
{
    QFile file;

    if(filename.isEmpty())
    {
        file.setFileName(curFilename);
        tabEdit->setFileName(curFilename,curIndex);
    }
    else
    {
        file.setFileName(filename);
        tabEdit->setFileName(filename,index);
    }



    file.open(QIODevice::WriteOnly | QIODevice::Text);
    if(file.isWritable())
        if(text.isEmpty())
            file.write(curPlainTextEdit->toPlainText().toUtf8().constData());
        else file.write(text.toUtf8().constData());
    else
        {
            QMessageBox::critical(this,"notepad--","Datoteka je odprta samo za branje");
            return false;
        }


    if(file.isOpen())
    file.close();

    ui->actionZavrzi_spremembe->setEnabled(false);
    if(index==-2)
    {
        tabEdit->setTextChanged(false,curIndex);
        tabEdit->setIcon("resources\\icons\\save.png",curIndex);
        setWindowTitle("notepad-- [" + curFilename +"]");
    }
    else
    {
        tabEdit->setTextChanged(false,index);
        tabEdit->setIcon("resources\\icons\\save.png",index);
    }

    return true;
}

int MainWindow::tabChanged(QString filename, QString text,int index)
{
    if(tabEdit->getTab(index)==NULL)return -1;

    filename = tabEdit->getFileName(index);

    QMessageBox msgBox;
    int result = -1;
    if(tabEdit->getTextChanged(index))
    {
        QFile tmp;
        if(!filename.isEmpty())
        {
            tmp.setFileName(filename);
            tmp.open(QIODevice::ReadOnly | QIODevice::Text);
            if(tmp.readAll()== text)
            {
                tmp.close();
                return result;
            }
        }
        else
        {
            if(text.isEmpty())
                return result;
        }

        int tmpIndex=curIndex;
        tabEdit->changeIndex(index);
        if(filename.isEmpty())
            msgBox.setText("<font color=""silver"" size=""5"">Shrani \"neimenovana\"?</font>");
        else
            msgBox.setText("<font color=""silver"" size=""5"">Shrani \"" + filename +"\"?</font>");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        result=msgBox.exec();
        if(result==QMessageBox::Save)
        {
            if (filename.isEmpty())
            {
                filename = QFileDialog::getSaveFileName(this,tr("Shrani datoteko"),"*",tr("Tekstovne datoteke (*.txt);;Vse datoteke (*.*)"));
                if(!filename.isEmpty())
                    tabEdit->setFileName(filename,index);
            }
            if(!save())
                result=QMessageBox::Cancel;
        }
        tabEdit->changeIndex(tmpIndex);
    }
    return result;
}

 void MainWindow::closeEvent(QCloseEvent *event)
 {
    tabEdit->saveData();
 }

void MainWindow::on_actionOdpri_triggered()
{
    QString filename;
    filename = QFileDialog::getOpenFileName(this,tr("Odpri datoteko"),"","Tekstovne datoteke (*.txt);;Vse datoteke (*.*)");

    if(filename.isEmpty())
        return;

    tabEdit->addTab(filename);

    ui->actionZavrzi_spremembe->setEnabled(false);

    setWindowTitle("notepad-- [" + filename + "]");
}

void MainWindow::on_actionZavrzi_spremembe_triggered()
{
    QFile file;
    if(curFilename.isEmpty())
        curPlainTextEdit->clear();
    else
    {
        file.setFileName(curFilename);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        curPlainTextEdit->setPlainText(file.readAll());
        file.close();
    }

    tabEdit->setIcon("resources\\icons\\save.png",curIndex);
    tabEdit->setTextChanged(false,curIndex);
    ui->actionZavrzi_spremembe->setEnabled(false);
}

void MainWindow::on_actionNajdi_triggered()
{
    if(!searchBarVisible)
    {
        QPropertyAnimation *searchanimation = new QPropertyAnimation(ui->searchWidget, "pos");
        searchanimation->setDuration(200);
        searchanimation->setStartValue(ui->searchWidget->pos());
        searchanimation->setEndValue(QPoint(0, ui->statusWidget->pos().y()-35));
        searchanimation->start(QAbstractAnimation::DeleteWhenStopped);

        tabEdit->animate(200,QSize(width()-5, ui->statusWidget->pos().y()-75));

        QPropertyAnimation *switchanimation = new QPropertyAnimation(ui->toolButtonSwitch, "pos");
        switchanimation->setDuration(200);
        switchanimation->setStartValue(ui->toolButtonSwitch->pos());
        switchanimation->setEndValue(QPoint(ui->statusWidget->width()-30, ui->statusWidget->pos().y()-25));
        switchanimation->start(QAbstractAnimation::DeleteWhenStopped);

        searchBarVisible=true;
    }
    else
    {
        QPropertyAnimation *searchanimation = new QPropertyAnimation(ui->searchWidget, "pos");
        searchanimation->setDuration(200);
        searchanimation->setStartValue(ui->searchWidget->pos());
        searchanimation->setEndValue(QPoint(0,ui->statusWidget->pos().y()));
        searchanimation->start(QAbstractAnimation::DeleteWhenStopped);

        tabEdit->animate(200,QSize(width()-5,ui->statusWidget->pos().y()-50));

        QPropertyAnimation *switchanimation = new QPropertyAnimation(ui->toolButtonSwitch, "pos");
        switchanimation->setDuration(200);
        switchanimation->setStartValue(ui->toolButtonSwitch->pos());
        switchanimation->setEndValue(QPoint(width()-30, ui->statusWidget->pos().y()));
        switchanimation->start(QAbstractAnimation::DeleteWhenStopped);

        QPropertyAnimation *replaceanimation = new QPropertyAnimation(ui->replaceWidget, "pos");
        replaceanimation->setDuration(200);
        replaceanimation->setStartValue(ui->replaceWidget->pos());
        replaceanimation->setEndValue(QPoint(0, ui->statusWidget->pos().y()));
        replaceanimation->start(QAbstractAnimation::DeleteWhenStopped);

        ui->toolButtonSwitch->setArrowType(Qt::ArrowType::UpArrow);
        replaceBarVisible=false;
        searchBarVisible=false;
    }
}

void MainWindow::on_actionIzberi_vse_triggered()
{
    curPlainTextEdit->selectAll();
}

void MainWindow::on_actionIzbrisi_triggered()
{
    curPlainTextEdit->textCursor().removeSelectedText();
}

void MainWindow::on_actionPrilepi_triggered()
{
    curPlainTextEdit->paste();
}

void MainWindow::on_actionIzrezi_triggered()
{
    curPlainTextEdit->cut();
}

void MainWindow::on_actionKopiraj_triggered()
{
    curPlainTextEdit->copy();
}

void MainWindow::on_pushButtonSearch_clicked()
{
    bool result;
    int flags=0;

    curPlainTextEdit->setFocus();

    if(ui->radioUp->isChecked())
        flags+=QTextDocument::FindBackward;

    if(ui->pushButton_case->isChecked())
        flags+=QTextDocument::FindCaseSensitively;

    if(ui->pushButton_fullword->isChecked())
        flags+=QTextDocument::FindWholeWords;

    result = curPlainTextEdit->find(ui->lineEditSearch->text(),(QTextDocument::FindFlags)flags);

    if(!result)
        QMessageBox::information(this,"notepad--", "<font color=""silver"">Ne morem najti \"" + ui->lineEditSearch->text() + "\"</font>");
}

void MainWindow::on_pushButtonCount_clicked()
{
    int n=-1;
    int flags=0;

    QTextCursor tmp = curPlainTextEdit->textCursor();
    curPlainTextEdit->moveCursor(QTextCursor::Start);

    if(ui->pushButton_case->isChecked())
        flags+=QTextDocument::FindCaseSensitively;

    if(ui->pushButton_fullword->isChecked())
        flags+=QTextDocument::FindWholeWords;

    for(bool result=true;result;n++)
        result = curPlainTextEdit->find(ui->lineEditSearch->text(),(QTextDocument::FindFlags)flags);

    curPlainTextEdit->setTextCursor(tmp);
    curPlainTextEdit->setFocus();

    QMessageBox::information(this,"notepad--", "<font color=""silver"">Najdeno <font color=""DodgerBlue"">" + QString::number(n) + "</font> krat</font>");
}

void MainWindow::on_toolButtonSwitch_clicked()
{
    if(!replaceBarVisible)
    {
        tabEdit->animate(200,QSize(width()-5, ui->statusWidget->pos().y()-100));

        QPropertyAnimation *searchanimation = new QPropertyAnimation(ui->searchWidget, "pos");
        searchanimation->setDuration(200);
        searchanimation->setStartValue(ui->searchWidget->pos());
        searchanimation->setEndValue(QPoint(0, ui->statusWidget->pos().y()-60));
        searchanimation->start(QAbstractAnimation::DeleteWhenStopped);

        QPropertyAnimation *replaceanimation = new QPropertyAnimation(ui->replaceWidget, "pos");
        replaceanimation->setDuration(200);
        replaceanimation->setStartValue(ui->replaceWidget->pos());
        replaceanimation->setEndValue(QPoint(0, ui->statusWidget->pos().y()-30));
        replaceanimation->start(QAbstractAnimation::DeleteWhenStopped);

        ui->toolButtonSwitch->setArrowType(Qt::ArrowType::DownArrow);
        replaceBarVisible=true;
    }
    else
    {
        tabEdit->animate(200,QSize(width()-5,ui->statusWidget->pos().y()-75));

        QPropertyAnimation *searchanimation = new QPropertyAnimation(ui->searchWidget, "pos");
        searchanimation->setDuration(200);
        searchanimation->setStartValue(ui->searchWidget->pos());
        searchanimation->setEndValue(QPoint(0,ui->statusWidget->pos().y()-35));
        searchanimation->start(QAbstractAnimation::DeleteWhenStopped);

        QPropertyAnimation *replaceanimation = new QPropertyAnimation(ui->replaceWidget, "pos");
        replaceanimation->setDuration(200);
        replaceanimation->setStartValue(ui->replaceWidget->pos());
        replaceanimation->setEndValue(QPoint(0, ui->statusWidget->pos().y()));
        replaceanimation->start(QAbstractAnimation::DeleteWhenStopped);

        ui->toolButtonSwitch->setArrowType(Qt::ArrowType::UpArrow);
        replaceBarVisible=false;
    }
}

void MainWindow::on_pushButtonReplace_clicked()
{
   if(curPlainTextEdit->textCursor().selectedText()==ui->lineEditSearch->text())
        curPlainTextEdit->textCursor().insertText(ui->lineEditReplace->text());
}

void MainWindow::on_pushButtonReplaceAll_clicked()
{
    curPlainTextEdit->setFocus();

    int n=-1;
    int flags=0;

    QTextCursor tmp = curPlainTextEdit->textCursor();

    curPlainTextEdit->moveCursor(QTextCursor::Start);

    if(ui->pushButton_case->isChecked())
        flags+=QTextDocument::FindCaseSensitively;

    if(ui->pushButton_fullword->isChecked())
        flags+=QTextDocument::FindWholeWords;

    for(bool result=true;result;n++)
    {
        result = curPlainTextEdit->find(ui->lineEditSearch->text(),(QTextDocument::FindFlags)flags);
        if(result)
            curPlainTextEdit->textCursor().insertText(ui->lineEditReplace->text());
    }

    curPlainTextEdit->setTextCursor(tmp);
    curPlainTextEdit->setFocus();

    QMessageBox::information(this,"notepad--", "<font color=""silver"">Zamenjano <font color=""DodgerBlue"">" + QString::number(n) + "</font> vnosov</font>");
}

void MainWindow::on_actionNastavitve_triggered()
{
    set->exec();
    loadSettings();

}

void MainWindow::loadSettings()
{
    QFile setFile("settings.dat");

    if(setFile.exists())
    {
        if(!setFile.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(NULL,"Napaka pri branju","Datoteke settings.dat ni bilo mogoče odpreti za branje.");
            return;
        }
        else
        {
            QDataStream in(&setFile);
            QColor textColor;
            QFont textFont;
            bool status,lineWrap;

            in >> textColor >> textFont >> status >> lineWrap;

            tabEdit->setTextColor(textColor);
            tabEdit->setTextFont(textFont);
            tabEdit->setLineWrap(lineWrap);
            statusBarVisible = status;

            this->resizeEvent(&QResizeEvent(this->size(),this->size()));
        }

    setFile.close();
    }
}

void MainWindow::on_actionNov_triggered()
{    
    tabEdit->addTab();
    setWindowTitle("notepad-- [neimenovana]");
}

void MainWindow::on_toolButtonNew_clicked()
{
    ui->actionNov->trigger();
}

void MainWindow::on_toolButtonOpen_clicked()
{
    ui->actionOdpri->trigger();
}

void MainWindow::on_toolButtonSave_clicked()
{
    ui->actionShrani->trigger();
}

void MainWindow::on_toolButtonPrint_clicked()
{
    ui->actionNatisni->trigger();
}

void MainWindow::on_toolButtonCut_clicked()
{
    ui->actionIzrezi->trigger();
}

void MainWindow::on_toolButtonCopy_clicked()
{
    ui->actionKopiraj->trigger();
}

void MainWindow::on_toolButtonPaste_clicked()
{
    ui->actionPrilepi->trigger();
}

void MainWindow::on_toolButtonUndo_clicked()
{
    ui->actionRazveljavi->trigger();
}

void MainWindow::on_toolButtonRedo_clicked()
{
    ui->actionPonovi->trigger();
}

void MainWindow::on_toolButtonSearch_clicked()
{
    ui->actionNajdi->trigger();
}

void MainWindow::on_toolButtonSettings_clicked()
{
    ui->actionNastavitve->trigger();
}

void MainWindow::on_actionNatisni_triggered()
{
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Natisni Dokument"));
        if (dialog->exec() != QDialog::Accepted)
            return;

   curPlainTextEdit->print(&printer);
}

void MainWindow::on_actionNastavitev_strani_triggered()
{
    QPageSetupDialog *dialog = new QPageSetupDialog(&printer, this);
    dialog->setWindowTitle(tr("Nastavitev Strani"));
    dialog->exec();
}

void MainWindow::on_toolButtonZoomIn_clicked()
{
    ui->actionPovecaj->trigger();
}

void MainWindow::on_toolButtonZoomOut_clicked()
{
    ui->actionPomanjsaj->trigger();
}

void MainWindow::on_actionPovecaj_triggered()
{
    curPlainTextEdit->zoomIn();
    tabEdit->setZoom(tabEdit->getZoom()+1);
    curTab->zoom=tabEdit->getZoom();
}

void MainWindow::on_actionPomanjsaj_triggered()
{
    curPlainTextEdit->zoomOut();
    tabEdit->setZoom(tabEdit->getZoom()-1);
    curTab->zoom=tabEdit->getZoom();
}

void MainWindow::on_actionPrivzeta_povecava_triggered()
{
    curPlainTextEdit->zoomOut(tabEdit->getZoom());
    curTab->zoom=tabEdit->getZoom();
    tabEdit->setZoom(0);
}

void MainWindow::tabChange(int index)
{
    if(index!=-1)
    {
        tabEdit->resize(QSize(width()-5,ui->searchWidget->pos().y()-50));
        curIndex = index;
        curPlainTextEdit = tabEdit->getEdit(index);

        curTab = tabEdit->getTab(index);

        curFilename = tabEdit->getFileName(index);
        if(curFilename.isEmpty())
            ui->actionOdpri_v_mapi->setEnabled(false);
        else ui->actionOdpri_v_mapi->setEnabled(true);

        ui->labelLenght->setText(QString::number(curPlainTextEdit->toPlainText().length()));
        ui->labelLines->setText(QString::number(curPlainTextEdit->blockCount()));
        ui->labelRow->setText(QString::number(curPlainTextEdit->textCursor().blockNumber()+1));
        ui->labelColumn->setText(QString::number(curPlainTextEdit->textCursor().columnNumber()));

        ui->actionRazveljavi->setEnabled(curTab->undoAvaible);
        ui->actionPonovi->setEnabled(curTab->redoAvaible);
        ui->actionIzbrisi->setEnabled(curTab->canCopy);
        ui->actionIzrezi->setEnabled(curTab->canCopy);
        ui->actionKopiraj->setEnabled(curTab->canCopy);

        ui->toolButtonUndo->setEnabled(curTab->undoAvaible);
        ui->toolButtonRedo->setEnabled(curTab->redoAvaible);
        ui->toolButtonCut->setEnabled(curTab->canCopy);
        ui->toolButtonCopy->setEnabled(curTab->canCopy);

       if(curPlainTextEdit->canPaste())
       {
                ui->actionPrilepi->setEnabled(true);
                ui->toolButtonPaste->setEnabled(true);
       }
       else
       {
            ui->actionPrilepi->setEnabled(false);
            ui->toolButtonPaste->setEnabled(false);
       }

        if(tabEdit->getFileName(index).isEmpty())
            setWindowTitle("notepad-- [neimenovana]");
        else
            setWindowTitle("notepad-- [" + tabEdit->getFileName(index) + "]");

        if(tabEdit->getTextChanged(index))
            ui->actionZavrzi_spremembe->setEnabled(true);
        else
            ui->actionZavrzi_spremembe->setEnabled(false);

        if(curTab->zoom!=tabEdit->getZoom())
        {
            curPlainTextEdit->zoomOut(curTab->zoom);
            curPlainTextEdit->zoomIn(tabEdit->getZoom());
            curTab->zoom=tabEdit->getZoom();
        }

        curPlainTextEdit->setFont(tabEdit->getTextFont());
        curPlainTextEdit->setStyleSheet("color: " + tabEdit->getTextColor().name() + ";");
        curPlainTextEdit->setLineWrapMode((QPlainTextEdit::LineWrapMode)tabEdit->getLineWrap());
    }
}

void MainWindow::tabTextChange()
{
    if(tabEdit->getTextChanged(curIndex)==false)
    {
        tabEdit->setTextChanged(true,curIndex);
        tabEdit->setIcon("resources\\icons\\unsaved.png",curIndex);
        ui->actionZavrzi_spremembe->setEnabled(true);
    }
}

void MainWindow::tabTextCurChange()
{
    ui->labelLenght->setText(QString::number(curPlainTextEdit->toPlainText().length()));
    ui->labelLines->setText(QString::number(curPlainTextEdit->blockCount()));
    ui->labelRow->setText(QString::number(curPlainTextEdit->textCursor().blockNumber()+1));
    ui->labelColumn->setText(QString::number(curPlainTextEdit->textCursor().columnNumber()));
}

void MainWindow::tabTextCopyAvaible(bool b)
{
    curTab->canCopy = b;

    ui->toolButtonCopy->setEnabled(b);
    ui->actionKopiraj->setEnabled(b);
}

void MainWindow::tabTextRedoAvaible(bool b)
{
    curTab->redoAvaible = b;

    ui->toolButtonRedo->setEnabled(b);
    ui->actionPonovi->setEnabled(b);
}

void MainWindow::tabTextUndoAvaible(bool b)
{
    curTab->undoAvaible = b;

    ui->toolButtonUndo->setEnabled(b);
    ui->actionRazveljavi->setEnabled(b);
}


void MainWindow::on_actionZapri_triggered()
{
    if(tabChanged(curFilename,curPlainTextEdit->toPlainText(),curIndex)!=QMessageBox::Cancel)
        tabEdit->remTab(curIndex);
}

void MainWindow::on_toolButtonClose_clicked()
{
    ui->actionZapri->trigger();
}

void MainWindow::on_toolButtonSaveAll_clicked()
{
    ui->actionShrani_vse->trigger();
}

void MainWindow::on_toolButtonCloseAll_clicked()
{
    ui->actionZapri_vse->trigger();
}

void MainWindow::on_actionShrani_vse_triggered()
{
    QString filename;
    for(int i=0;i<tabEdit->countTabs();i++)
    {
        filename = tabEdit->getFileName(i);
        tabEdit->changeIndex(i);
        if(filename.isEmpty())
        {
            filename = QFileDialog::getSaveFileName(this,tr("Shrani datoteko"),"*",tr("Tekstovne datoteke (*.txt);;Vse datoteke (*.*)"));
            if(!filename.isEmpty())
                tabEdit->setFileName(filename,i);
            else continue;
        }
        save(filename,tabEdit->getEdit(i)->toPlainText(),i);
    }
}

void MainWindow::on_actionZapri_vse_triggered()
{
    while(true)
    {
     if(tabChanged(tabEdit->getFileName(0),tabEdit->getEdit(0)->toPlainText(),0)==QMessageBox::Cancel)
         continue;

     if(!tabEdit->remTab(0))break;
    }
}

void MainWindow::on_actionOdpri_v_mapi_triggered()
{
    QString argv = QLatin1String("/select,");
    QString explorer = QLatin1String("explorer.exe");
    argv+=QDir::toNativeSeparators(curFilename);
    QProcess::startDetached(explorer+" "+argv);
}

void MainWindow::on_actionZapri_vse_razen_tega_triggered()
{
    ui->actionZapri_vse_na_levo->trigger();
    ui->actionZapri_vse_na_desno->trigger();
}

void MainWindow::on_actionZapri_vse_na_levo_triggered()
{
    int i=0;
    while(curIndex!=i)
    {
        if(tabChanged(tabEdit->getFileName(i),tabEdit->getEdit(i)->toPlainText(),i)==QMessageBox::Cancel)
        {
            i++;
            continue;
        }
        tabEdit->remTab(i);
    }
}

void MainWindow::on_actionZapri_vse_na_desno_triggered()
{
    int i=1;
    while(true)
    {
        if(tabEdit->getTab(curIndex+i)==NULL)break;
        if(tabChanged(tabEdit->getFileName(curIndex+i),tabEdit->getEdit(curIndex+i)->toPlainText(),curIndex+i)==QMessageBox::Cancel)
        {
            i++;
            continue;
        }
        tabEdit->remTab(curIndex+i);
    }
}

void MainWindow::on_actionEnkripteraj_triggered()
{
    SimpleCrypt crypt;
    QFile file;
    QString key;
    QByteArray encrypted;

    key = QInputDialog::getText(this,"notepad--","Vnesi ključ za šifriranje",QLineEdit::Password);
    if(!key.isEmpty())
    {
        file.setFileName(QFileDialog::getSaveFileName(this,tr("Shrani šifrirano datoteko"),"*",tr("Binarne datoteke (*.dat);;Vse datoteke (*.*)")));
        crypt.setKey(key.toLong());
        encrypted = crypt.encryptToByteArray(curPlainTextEdit->toPlainText().toUtf8());

        file.open(QIODevice::WriteOnly);
        if(file.isWritable())
        {
            file.write(encrypted);
            tabEdit->remTab(curIndex);
        }
        else
            QMessageBox::critical(this,"notepad--","Datoteka je odprta samo za branje");

        file.close();
    }

}

void MainWindow::on_actionDekripteraj_triggered()
{
    SimpleCrypt crypt;
    QFile file;
    QString key;
    QString decrypted;
    QByteArray fileText;

    file.setFileName(QFileDialog::getOpenFileName(this,tr("Odpri šifrirano datoteko"),"*",tr("Binarne datoteke (*.dat);;Vse datoteke (*.*)")));
    if(file.exists())
    {
        key = QInputDialog::getText(this,"notepad--","Vnesi ključ",QLineEdit::Password);
        if(!key.isEmpty())
        {
            crypt.setKey(key.toLong());
            file.open(QIODevice::ReadOnly);
            if(file.isReadable())
            {
                fileText = file.readAll();

                decrypted = crypt.decryptToString(fileText);
                if(crypt.lastError() == crypt.ErrorNoError)
                {
                    tabEdit->addTab();
                    curPlainTextEdit->setPlainText(decrypted);
                }
                else if (crypt.lastError() == crypt.ErrorIntegrityFailed)
                    QMessageBox::critical(this,"notepad--","Kluč je napačen.");
                else if(crypt.lastError() == crypt.ErrorUnknownVersion)
                    QMessageBox::warning(this,"notepad--","Datoteka ni šifrirana.");
            }
            else
                QMessageBox::critical(this,"notepad--","Napaka pri odpiranju datoteke");

            file.close();
        }
    }
}
