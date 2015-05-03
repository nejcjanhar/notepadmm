#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(QApplication::clipboard(), SIGNAL(dataChanged()),
                this, SLOT(clipboardChanged()));

    zoom=0;
    textChanged=false;
    searchBarVisible=false;
    replaceBarVisible=false;

    setWindowTitle("notepad-- [Neimenovana]");

    ui->actionZavrzi_spremembe->setEnabled(false);
    ui->actionRazveljavi->setEnabled(false);
    ui->actionPonovi->setEnabled(false);
    ui->actionIzbrisi->setEnabled(false);
    ui->actionIzrezi->setEnabled(false);
    ui->actionKopiraj->setEnabled(false);

    ui->toolButtonUndo->setEnabled(false);
    ui->toolButtonRedo->setEnabled(false);
    ui->toolButtonCut->setEnabled(false);
    ui->toolButtonCopy->setEnabled(false);

    if(!ui->plainTextEdit->canPaste())
    {
        ui->actionPrilepi->setEnabled(false);
        ui->toolButtonPaste->setEnabled(false);
    }
    else
    {
        ui->actionPrilepi->setEnabled(true);
        ui->toolButtonPaste->setEnabled(true);
    }

}

void MainWindow::clipboardChanged()
{
    if(!ui->plainTextEdit->canPaste())
    {
        ui->actionPrilepi->setEnabled(false);
        ui->toolButtonPaste->setEnabled(false);
    }
    else
    {
        ui->actionPrilepi->setEnabled(true);
        ui->toolButtonPaste->setEnabled(true);
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
    ui->line_5->move(QPoint(0,ui->statusWidget->pos().y()));
    ui->plainTextEdit->resize(QSize(event->size().width(),ui->searchWidget->pos().y()-40));
}


MainWindow::~MainWindow()
{
    delete ui;
    delete set;
}

void MainWindow::on_actionRazveljavi_triggered()
{
    ui->plainTextEdit->undo();
}

void MainWindow::on_actionPonovi_triggered()
{
    ui->plainTextEdit->redo();
}

void MainWindow::on_actionIzhod_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionShrani_triggered()
{

    if (filename.isEmpty())
        filename = QFileDialog::getSaveFileName(this,tr("Shrani datoteko"),"*","Tekstovne datoteke (*.txt);;Vse datoteke (*.*)");

    if(filename.isEmpty())
        return;

    save();
}

void MainWindow::on_actionShrani_kot_triggered()
{
    QString filenametmp;

    if(!filename.isEmpty())
        filenametmp = filename;

    filename = QFileDialog::getSaveFileName(this,tr("Shrani datoteko"), filename,"Tekstovne datoteke (*.txt);;Vse datoteke (*.*)");

    if(filename.isEmpty())
    {
        filename = filenametmp;
        return;
    }

    save();
}

void MainWindow::on_plainTextEdit_textChanged()
{
    ui->labelLenght->setText(QString::number(ui->plainTextEdit->toPlainText().length()));
    ui->labelLines->setText(QString::number(ui->plainTextEdit->blockCount()));

    if(!textChanged)
    {
        textChanged=true;
        ui->actionZavrzi_spremembe->setEnabled(true);
    }

}

bool MainWindow::save()
{
    file.setFileName(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    if(file.isWritable())
        file.write(ui->plainTextEdit->toPlainText().toUtf8().constData());
    else
        {
            QMessageBox::critical(this,"notepad--","Datoteka je odprta samo za branje");
            return false;
        }

    setWindowTitle("notepad-- [" + filename +"]");

    if(file.isOpen())
    file.close();
    textChanged=false;
    ui->actionZavrzi_spremembe->setEnabled(false);

    return true;
}

int MainWindow::fileChanged()
{
    QMessageBox msgBox;
    int result = -1;
    if(textChanged)
    {
        QFile tmp;
        if(!filename.isEmpty())
        {
            tmp.setFileName(filename);
            tmp.open(QIODevice::ReadOnly | QIODevice::Text);
            if(tmp.readAll()==ui->plainTextEdit->toPlainText())
            {
                tmp.close();
                return result;
            }
        }
        else
        {
            if(ui->plainTextEdit->toPlainText().isEmpty())
                return result;
        }


        msgBox.setText("<font color=""silver"" size=""5"">Ali želiti shraniti spremembe?</font>");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        result=msgBox.exec();
        if(result==QMessageBox::Save)
        {
            if (filename.isEmpty())
                filename = QFileDialog::getSaveFileName(this,tr("Shrani datoteko"),"*","Tekstovne datoteke (*.txt);;Vse datoteke (*.*)");
            if(!save())
                result=QMessageBox::Cancel;
        }
    }

    return result;
}

 void MainWindow::closeEvent(QCloseEvent *event)
 {
     if(fileChanged()==QMessageBox::Cancel)
         event->setAccepted(false);
 }

void MainWindow::on_actionOdpri_triggered()
{

    QString filenametmp;

    if(fileChanged()==QMessageBox::Cancel)
        return;

    if(!filename.isEmpty())
        filenametmp=filename;

    filename = QFileDialog::getOpenFileName(this,tr("Odpri datoteko"),"","Tekstovne datoteke (*.txt);;Vse datoteke (*.*)");

    if(filename.isEmpty())
    {
        filename=filenametmp;
        return;
    }

    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    if(!file.isOpen())
        return;

    ui->plainTextEdit->setPlainText(file.readAll());

    file.close();

    textChanged = false;
    ui->actionZavrzi_spremembe->setEnabled(false);

    setWindowTitle("notepad-- [" + filename + "]");
}

void MainWindow::on_actionZavrzi_spremembe_triggered()
{
    if(filename.isEmpty())
        ui->plainTextEdit->clear();
    else
    {
        file.setFileName(filename);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        ui->plainTextEdit->setPlainText(file.readAll());
        file.close();
    }

    textChanged = false;
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

        QPropertyAnimation *textanimation = new QPropertyAnimation(ui->plainTextEdit, "size");
        textanimation->setDuration(200);
        textanimation->setStartValue(ui->plainTextEdit->size());
        textanimation->setEndValue(QSize(width(), ui->statusWidget->pos().y()-75));
        textanimation->start(QAbstractAnimation::DeleteWhenStopped);

        QPropertyAnimation *switchanimation = new QPropertyAnimation(ui->toolButtonSwitch, "pos");
        switchanimation->setDuration(200);
        switchanimation->setStartValue(ui->toolButtonSwitch->pos());
        switchanimation->setEndValue(QPoint(ui->statusWidget->width()-30, ui->statusWidget->pos().y()-25));
        switchanimation->start(QAbstractAnimation::DeleteWhenStopped);

        searchBarVisible=true;
    }
    else
    {
        QPropertyAnimation *textanimation = new QPropertyAnimation(ui->plainTextEdit, "size");
        textanimation->setDuration(200);
        textanimation->setStartValue(ui->plainTextEdit->size());
        textanimation->setEndValue(QSize(width(),ui->statusWidget->pos().y()-40));
        textanimation->start(QAbstractAnimation::DeleteWhenStopped);

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

void MainWindow::on_plainTextEdit_undoAvailable(bool b)
{
    ui->actionRazveljavi->setEnabled(b);

    ui->toolButtonUndo->setEnabled(b);
}

void MainWindow::on_plainTextEdit_copyAvailable(bool b)
{
    ui->actionIzbrisi->setEnabled(b);
    ui->actionIzrezi->setEnabled(b);
    ui->actionKopiraj->setEnabled(b);

    ui->toolButtonCut->setEnabled(b);
    ui->toolButtonCopy->setEnabled(b);
}

void MainWindow::on_plainTextEdit_redoAvailable(bool b)
{
    ui->actionPonovi->setEnabled(b);

    ui->toolButtonRedo->setEnabled(b);
}

void MainWindow::on_actionIzberi_vse_triggered()
{
    ui->plainTextEdit->selectAll();
}

void MainWindow::on_actionIzbrisi_triggered()
{
    ui->plainTextEdit->textCursor().removeSelectedText();
}

void MainWindow::on_actionPrilepi_triggered()
{
    ui->plainTextEdit->paste();
}

void MainWindow::on_actionIzrezi_triggered()
{
    ui->plainTextEdit->cut();
}

void MainWindow::on_actionKopiraj_triggered()
{
    ui->plainTextEdit->copy();
}

void MainWindow::on_pushButtonSearch_clicked()
{
    bool result;
    int flags=0;

    ui->plainTextEdit->setFocus();

    if(ui->radioUp->isChecked())
        flags+=QTextDocument::FindBackward;

    if(ui->pushButton_case->isChecked())
        flags+=QTextDocument::FindCaseSensitively;

    if(ui->pushButton_fullword->isChecked())
        flags+=QTextDocument::FindWholeWords;

    result = ui->plainTextEdit->find(ui->lineEditSearch->text(),(QTextDocument::FindFlags)flags);

    if(!result)
        QMessageBox::information(this,"notepad--", "<font color=""silver"">Ne morem najti \"" + ui->lineEditSearch->text() + "\"</font>");
}

void MainWindow::on_pushButtonCount_clicked()
{
    int n=-1;
    int flags=0;

    QTextCursor tmp = ui->plainTextEdit->textCursor();
    ui->plainTextEdit->moveCursor(QTextCursor::Start);

    if(ui->pushButton_case->isChecked())
        flags+=QTextDocument::FindCaseSensitively;

    if(ui->pushButton_fullword->isChecked())
        flags+=QTextDocument::FindWholeWords;

    for(bool result=true;result;n++)
        result = ui->plainTextEdit->find(ui->lineEditSearch->text(),(QTextDocument::FindFlags)flags);

    ui->plainTextEdit->setTextCursor(tmp);
    ui->plainTextEdit->setFocus();

    QMessageBox::information(this,"notepad--", "<font color=""silver"">Najdeno <font color=""DodgerBlue"">" + QString::number(n) + "</font> krat</font>");
}

void MainWindow::on_toolButtonSwitch_clicked()
{
    if(!replaceBarVisible)
    {
        QPropertyAnimation *textanimation = new QPropertyAnimation(ui->plainTextEdit, "size");
        textanimation->setDuration(200);
        textanimation->setStartValue(ui->plainTextEdit->size());
        textanimation->setEndValue(QSize(width(), ui->statusWidget->pos().y()-100));
        textanimation->start(QAbstractAnimation::DeleteWhenStopped);

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
        QPropertyAnimation *textanimation = new QPropertyAnimation(ui->plainTextEdit, "size");
        textanimation->setDuration(200);
        textanimation->setStartValue(ui->plainTextEdit->size());
        textanimation->setEndValue(QSize(width(),ui->statusWidget->pos().y()-75));
        textanimation->start(QAbstractAnimation::DeleteWhenStopped);

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
   if(ui->plainTextEdit->textCursor().selectedText()==ui->lineEditSearch->text())
        ui->plainTextEdit->textCursor().insertText(ui->lineEditReplace->text());
}

void MainWindow::on_pushButtonReplaceAll_clicked()
{
    ui->plainTextEdit->setFocus();

    int n=-1;
    int flags=0;

    QTextCursor tmp = ui->plainTextEdit->textCursor();

    ui->plainTextEdit->moveCursor(QTextCursor::Start);

    if(ui->pushButton_case->isChecked())
        flags+=QTextDocument::FindCaseSensitively;

    if(ui->pushButton_fullword->isChecked())
        flags+=QTextDocument::FindWholeWords;

    for(bool result=true;result;n++)
    {
        result = ui->plainTextEdit->find(ui->lineEditSearch->text(),(QTextDocument::FindFlags)flags);
        if(result)
            ui->plainTextEdit->textCursor().insertText(ui->lineEditReplace->text());
    }

    ui->plainTextEdit->setTextCursor(tmp);
    ui->plainTextEdit->setFocus();

    QMessageBox::information(this,"notepad--", "<font color=""silver"">Zamenjano <font color=""DodgerBlue"">" + QString::number(n) + "</font> vnosov</font>");
}

void MainWindow::on_actionNastavitve_triggered()
{
    set->exec();
}

void MainWindow::on_actionNov_triggered()
{
    if(fileChanged()==QMessageBox::Cancel)
        return;

    filename.clear();

    setWindowTitle("notepad-- [Neimenovana]");

    ui->plainTextEdit->clear();
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

void MainWindow::on_plainTextEdit_cursorPositionChanged()
{
    QTextCursor cursor = ui->plainTextEdit->textCursor();

    ui->labelRow->setText(QString::number(cursor.blockNumber()+1));
    ui->labelColumn->setText(QString::number(cursor.columnNumber()+1));
}

void MainWindow::on_actionNatisni_triggered()
{
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Natisni Dokument"));
        if (dialog->exec() != QDialog::Accepted)
            return;

   ui->plainTextEdit->print(&printer);
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
    ui->plainTextEdit->zoomIn();
    zoom++;
}

void MainWindow::on_actionPomanjsaj_triggered()
{
    ui->plainTextEdit->zoomOut();
    zoom--;
}

void MainWindow::on_actionPrivzeta_povecava_triggered()
{
    ui->plainTextEdit->zoomOut(zoom);
    zoom=0;
}
