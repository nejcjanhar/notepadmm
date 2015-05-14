#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    zoom=0;
    closingTabs = false;

    tabEdit = new MultiTabEdit(this);

    connect(QApplication::clipboard(), SIGNAL(dataChanged()),this, SLOT(clipboardChanged()));
    connect(tabEdit,&MultiTabEdit::tabChange,this,&MainWindow::tabChange);
    connect(tabEdit,&MultiTabEdit::tabTextChange,this,&MainWindow::tabTextChange);
    connect(tabEdit,&MultiTabEdit::tabTextCurChange,this,&MainWindow::tabTextCurChange);
    connect(tabEdit,&MultiTabEdit::tabTextCopyAvaible,this,&MainWindow::tabTextCopyAvaible);
    connect(tabEdit,&MultiTabEdit::tabTextRedoAvaible,this,&MainWindow::tabTextRedoAvaible);
    connect(tabEdit,&MultiTabEdit::tabTextUndoAvaible,this,&MainWindow::tabTextUndoAvaible);

    tabChange(0);
    tabEdit->move(QPoint(0,60));

    searchBarVisible=false;
    replaceBarVisible=false;


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
    }
    else
    {
        ui->actionPrilepi->setEnabled(false);
        ui->toolButtonPaste->setEnabled(false);
    }
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    ui->statusWidget->move(QPoint(0,event->size().height()-40));
    ui->statusWidget->resize(QSize(event->size().width(),20));
    ui->toolbarWidget->resize(QSize(event->size().width(),30));
    ui->toolButtonSwitch->move(QPoint(ui->statusWidget->width()-30,ui->statusWidget->pos().y()-25));

    if(!searchBarVisible)
    {
        ui->replaceWidget->move(QPoint(0,ui->statusWidget->pos().y()));
        ui->searchWidget->move(QPoint(0,ui->replaceWidget->pos().y()));
        ui->toolButtonSwitch->move(QPoint(event->size().width()-30,event->size().height()));
    }
    else if(!replaceBarVisible)
    {
        ui->replaceWidget->move(0,ui->statusWidget->pos().y());
        ui->searchWidget->move(0,ui->replaceWidget->pos().y()-35);
    }
    else
    {
        ui->replaceWidget->move(0,ui->statusWidget->pos().y()-30);
        ui->searchWidget->move(0,ui->replaceWidget->pos().y()-30);
    }

    ui->line_5->resize(QSize(event->size().width(),2));
    ui->line_5->move(QPoint(0,ui->statusWidget->pos().y()-1));
    tabEdit->resize(QSize(event->size().width()-5,ui->searchWidget->pos().y()-50));
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
        curFilename = QFileDialog::getSaveFileName(this,tr("Shrani datoteko"),"*","Tekstovne datoteke (*.txt);;Vse datoteke (*.*)");

    if(curFilename.isEmpty())
        return;

    save();
}

void MainWindow::on_actionShrani_kot_triggered()
{
    curFilename = QFileDialog::getSaveFileName(this,tr("Shrani datoteko"), curFilename,"Tekstovne datoteke (*.txt);;Vse datoteke (*.*)");

    if(curFilename.isEmpty())
        return;

    save();
}

bool MainWindow::save(QString filename,QString text,int index)
{
    QFile file;

    if(filename.isEmpty())
        file.setFileName(curFilename);
    else
        file.setFileName(filename);

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
        tabEdit->setIcon(QIcon("resources\\icons\\save.png"),curIndex);
        setWindowTitle("notepad-- [" + curFilename +"]");
    }
    else
    {
        tabEdit->setTextChanged(false,index);
        tabEdit->setIcon(QIcon("resources\\icons\\save.png"),index);
    }

    return true;
}

int MainWindow::fileChanged(QString filename, QString text,int index)
{
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


        msgBox.setText("<font color=""silver"" size=""5"">Ali želiti shraniti spremembe?</font>");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        result=msgBox.exec();
        if(result==QMessageBox::Save)
        {
            if (filename.isEmpty())
            {
                filename = QFileDialog::getSaveFileName(this,tr("Shrani datoteko"),"*","Tekstovne datoteke (*.txt);;Vse datoteke (*.*)");
                if(!filename.isEmpty())
                    tabEdit->setFileName(filename,index);
            }
            if(!save())
                result=QMessageBox::Cancel;
        }
    }

    return result;
}

 void MainWindow::closeEvent(QCloseEvent *event)
 {

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

    tabEdit->setIcon(QIcon("resources\\icons\\save.png"),curIndex);
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

        tabEdit->animate(200,QSize(width(), ui->statusWidget->pos().y()-75));

        QPropertyAnimation *switchanimation = new QPropertyAnimation(ui->toolButtonSwitch, "pos");
        switchanimation->setDuration(200);
        switchanimation->setStartValue(ui->toolButtonSwitch->pos());
        switchanimation->setEndValue(QPoint(ui->statusWidget->width()-30, ui->statusWidget->pos().y()-25));
        switchanimation->start(QAbstractAnimation::DeleteWhenStopped);

        searchBarVisible=true;
    }
    else
    {
        tabEdit->animate(200,QSize(width(),ui->statusWidget->pos().y()-40));

        QPropertyAnimation *searchanimation = new QPropertyAnimation(ui->searchWidget, "pos");
        searchanimation->setDuration(200);
        searchanimation->setStartValue(ui->searchWidget->pos());
        searchanimation->setEndValue(QPoint(0,ui->statusWidget->pos().y()));
        searchanimation->start(QAbstractAnimation::DeleteWhenStopped);

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
        tabEdit->animate(200,QSize(width(), ui->statusWidget->pos().y()-100));

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
        tabEdit->animate(200,QSize(width(),ui->statusWidget->pos().y()-75));

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
    zoom++;
    curTab->zoom=zoom;
}

void MainWindow::on_actionPomanjsaj_triggered()
{
    curPlainTextEdit->zoomOut();
    zoom--;
    curTab->zoom=zoom;
}

void MainWindow::on_actionPrivzeta_povecava_triggered()
{
    curPlainTextEdit->zoomOut(zoom);
    zoom=0;
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

        if(curTab->zoom!=zoom)
        {
            curPlainTextEdit->zoomOut(curTab->zoom);
            curPlainTextEdit->zoomIn(zoom);
            curTab->zoom=zoom;
        }
    }
}

void MainWindow::tabTextChange()
{
    tabEdit->setTextChanged(true,curIndex);
    tabEdit->setIcon(QIcon("resources\\icons\\unsaved.png"),curIndex);

    ui->actionZavrzi_spremembe->setEnabled(true);
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
    for(int i=0;i<tabEdit->coutTabs();i++)
    {
        filename = tabEdit->getFileName(i);
        if(filename.isEmpty())
        {
            filename = QFileDialog::getSaveFileName(this,tr("Shrani datoteko"),"*","Tekstovne datoteke (*.txt);;Vse datoteke (*.*)");
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
     if(fileChanged(tabEdit->getFileName(0),tabEdit->getEdit(0)->toPlainText(),0)==QMessageBox::Cancel)
         continue;

     if(!tabEdit->remTab(0))break;
    }
}
