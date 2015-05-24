#include "multitabedit.h"

MultiTabEdit::MultiTabEdit(QWidget *widget,QString path)
{
    saveFile.setFileName("last.dat");

    tabZoom=0;
    textColor.setNamedColor("silver");
    lineWrap=true;
    tabWidget = new QTabWidget(widget);

    connect(tabWidget,&QTabWidget::currentChanged,this,&MultiTabEdit::tabChange);

    QString tabName;

    tabTextEdit *tmpTab = new tabTextEdit;
    tmpTab->tab = new QWidget;
    tmpTab->edit = new QPlainTextEdit(tmpTab->tab);

    tmpTab->iconFileName="resources\\icons\\save.png";
    tmpTab->canCopy = false;
    tmpTab->redoAvaible = false;
    tmpTab->undoAvaible = false;
    tmpTab->textChanged = false;
    tmpTab->zoom=0;

    if(path.isEmpty())
    {
        tabName="neimenovana";
        tmpTab->filename = "";
    }
    else
    {
        QFileInfo info(path);
        if(info.exists())
        {
            QFile file;
            file.setFileName(path);
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            tmpTab->edit->setPlainText(file.readAll());
            tmpTab->edit->setReadOnly(!info.isWritable());
            tmpTab->filename = path;
            tabName=info.fileName();
            file.close();
        }
        else
        {
            delete tmpTab->edit;
            delete tmpTab->tab;
            delete tmpTab;
            return;
        }
    }

    tmpTab->prevTabEdit=tmpTab->nextTabEdit=NULL;
    start=end=tmpTab;

    tabWidget->addTab(tmpTab->tab,QIcon(tmpTab->iconFileName),tabName);
    tmpTab->index = tabWidget->indexOf(tmpTab->tab);

    tabWidget->setCurrentIndex(tmpTab->index);
    tmpTab->edit->setFocus();

    connect(tmpTab->edit,&QPlainTextEdit::textChanged,this,&MultiTabEdit::tabTextChange);
    connect(tmpTab->edit,&QPlainTextEdit::cursorPositionChanged,this,&MultiTabEdit::tabTextCurChange);
    connect(tmpTab->edit,&QPlainTextEdit::selectionChanged,this,&MultiTabEdit::tabTextCurChange);
    connect(tmpTab->edit,&QPlainTextEdit::copyAvailable,this,&MultiTabEdit::tabTextCopyAvaible);
    connect(tmpTab->edit,&QPlainTextEdit::undoAvailable,this,&MultiTabEdit::tabTextUndoAvaible);
    connect(tmpTab->edit,&QPlainTextEdit::redoAvailable,this,&MultiTabEdit::tabTextRedoAvaible);

    loadData();
}

MultiTabEdit::~MultiTabEdit()
{
    while(remTab(tabWidget->currentIndex()));
    delete tabWidget;
}

void MultiTabEdit::addTab(QString path)
{
    QString tabName;

    tabTextEdit *tmpTab = new tabTextEdit;
    tmpTab->tab = new QWidget;
    tmpTab->edit = new QPlainTextEdit(tmpTab->tab);

    tmpTab->iconFileName="resources\\icons\\save.png";
    tmpTab->canCopy = false;
    tmpTab->redoAvaible = false;
    tmpTab->undoAvaible = false;
    tmpTab->textChanged = false;
    tmpTab->zoom=0;

    if(path.isEmpty())
    {
        tabName="neimenovana";
        tmpTab->filename = "";
    }
    else
    {
        QFileInfo info(path);
        if(info.exists())
        {
            QFile file;
            file.setFileName(path);
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            tmpTab->edit->setPlainText(file.readAll());
            tmpTab->edit->setReadOnly(!info.isWritable());
            tmpTab->filename = path;
            tabName=info.fileName();
            file.close();
        }
        else
        {
            delete tmpTab->edit;
            delete tmpTab->tab;
            delete tmpTab;
            return;
        }
    }

    tmpTab->nextTabEdit=start;
    tmpTab->prevTabEdit=NULL;
    start->prevTabEdit=tmpTab;
    start=tmpTab;

    tabWidget->addTab(tmpTab->tab,QIcon(tmpTab->iconFileName),tabName);
    tmpTab->index = tabWidget->indexOf(tmpTab->tab);

    tabWidget->setCurrentIndex(tmpTab->index);
    tmpTab->edit->setFocus();

    connect(tmpTab->edit,&QPlainTextEdit::textChanged,this,&MultiTabEdit::tabTextChange);
    connect(tmpTab->edit,&QPlainTextEdit::cursorPositionChanged,this,&MultiTabEdit::tabTextCurChange);
    connect(tmpTab->edit,&QPlainTextEdit::selectionChanged,this,&MultiTabEdit::tabTextCurChange);
    connect(tmpTab->edit,&QPlainTextEdit::copyAvailable,this,&MultiTabEdit::tabTextCopyAvaible);
    connect(tmpTab->edit,&QPlainTextEdit::undoAvailable,this,&MultiTabEdit::tabTextUndoAvaible);
    connect(tmpTab->edit,&QPlainTextEdit::redoAvailable,this,&MultiTabEdit::tabTextRedoAvaible);
}

bool MultiTabEdit::remTab(int index)
{
    tabTextEdit *curTab;
    bool result = true;

    curTab=getTab(index);
    if(curTab==NULL)
        return false;

    if(curTab==end && curTab==start)
    {
        tabTextEdit *tmpTab = new tabTextEdit;
        tmpTab->tab = new QWidget;
        tmpTab->edit = new QPlainTextEdit(tmpTab->tab);

        tmpTab->iconFileName="resources\\icons\\save.png";
        tmpTab->canCopy = false;
        tmpTab->redoAvaible = false;
        tmpTab->undoAvaible = false;
        tmpTab->textChanged = false;
        tmpTab->zoom=0;

        tmpTab->filename = "";

        tmpTab->nextTabEdit=start;
        tmpTab->prevTabEdit=NULL;

        start->prevTabEdit=tmpTab;
        start=tmpTab;

        tabWidget->addTab(tmpTab->tab,QIcon(tmpTab->iconFileName),"neimenovana");
        tmpTab->index = tabWidget->indexOf(tmpTab->tab);

        tabWidget->setCurrentIndex(tmpTab->index);
        tmpTab->edit->setFocus();

        connect(tmpTab->edit,&QPlainTextEdit::textChanged,this,&MultiTabEdit::tabTextChange);
        connect(tmpTab->edit,&QPlainTextEdit::cursorPositionChanged,this,&MultiTabEdit::tabTextCurChange);
        connect(tmpTab->edit,&QPlainTextEdit::selectionChanged,this,&MultiTabEdit::tabTextCurChange);
        connect(tmpTab->edit,&QPlainTextEdit::copyAvailable,this,&MultiTabEdit::tabTextCopyAvaible);
        connect(tmpTab->edit,&QPlainTextEdit::undoAvailable,this,&MultiTabEdit::tabTextUndoAvaible);
        connect(tmpTab->edit,&QPlainTextEdit::redoAvailable,this,&MultiTabEdit::tabTextRedoAvaible);

        result = false;
    }
    if(curTab==end)
    {
        end=end->prevTabEdit;
        end->nextTabEdit=NULL;
    }
    else if(curTab==start)
    {
        start=start->nextTabEdit;
        start->prevTabEdit=NULL;
    }
    else
    {
        curTab->prevTabEdit->nextTabEdit=curTab->nextTabEdit;
        curTab->nextTabEdit->prevTabEdit=curTab->prevTabEdit;
    }

    for(tabTextEdit *tmpTab=start;tmpTab!=curTab->nextTabEdit;tmpTab=tmpTab->nextTabEdit)
        tmpTab->index--;


    tabWidget->removeTab(index);
    delete curTab->edit;
    delete curTab->tab;
    delete curTab;

    getEdit(getCurrentTabIndex())->setFocus();

    return result;
}

void MultiTabEdit::resize(QSize size)
{
    tabWidget->resize(size);

    if(start!=NULL)
        getTab(getCurrentTabIndex())->edit->resize(QSize(size.width()-10,size.height()-35));
}

void MultiTabEdit::move(QPoint point)
{
    tabWidget->move(point);
}

int MultiTabEdit::getCurrentTabIndex()
{
    return tabWidget->currentIndex();
}

bool MultiTabEdit::tabExists()
{
    if(start==NULL)
        return false;
    else
        return true;
}

tabTextEdit* MultiTabEdit::getTab(int index)
{
    QWidget *tmp = tabWidget->widget(index);
    tabTextEdit *curTab;

    for(curTab=start;curTab!=NULL && curTab->tab!=tmp;curTab=curTab->nextTabEdit);

    return curTab;
}

void MultiTabEdit::animate(int duration,QSize endValue)
{
    if(start!=NULL)
    {
    QPlainTextEdit *edit = getTab(getCurrentTabIndex())->edit;
    QPropertyAnimation *editAnimation = new QPropertyAnimation(edit, "size");
    editAnimation->setDuration(duration);
    editAnimation->setStartValue(edit->size());
    editAnimation->setEndValue(QSize(endValue.width()-10,endValue.height()-35));
    editAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    QPropertyAnimation *widgetAnimation = new QPropertyAnimation(tabWidget, "size");
    widgetAnimation->setDuration(duration);
    widgetAnimation->setStartValue(tabWidget->size());
    widgetAnimation->setEndValue(endValue);
    widgetAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

QPlainTextEdit* MultiTabEdit::getEdit(int index)
{
    return getTab(index)->edit;
}

QString MultiTabEdit::getFileName(int index)
{
    return getTab(index)->filename;
}

void MultiTabEdit::setFileName(QString filename, int index)
{
    getTab(index)->filename = filename;
    if(!filename.isEmpty())
    {
        QFileInfo info(filename);
        tabWidget->setTabText(index,info.fileName());
    }
}

bool MultiTabEdit::getTextChanged(int index)
{
    return getTab(index)->textChanged;
}

void MultiTabEdit::setTextChanged(bool change, int index)
{
    getTab(index)->textChanged = change;
}

void MultiTabEdit::setIcon(QString iconpath, int index)
{
    tabWidget->setTabIcon(index,QIcon(iconpath));
    getTab(index)->iconFileName=iconpath;
}

int MultiTabEdit::countTabs()
{
    return tabWidget->count();
}

int MultiTabEdit::getZoom()
{
    return tabZoom;
}

void MultiTabEdit::setZoom(int n)
{
    tabZoom=n;
}

void MultiTabEdit::saveData()
{
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        if(saveFile.exists())
        {
            int result=QMessageBox::critical(NULL,"Napaka pri shranjevanju","Datoteke last.dat ni bilo mogoče odpreti za pisanje.",QMessageBox::Retry | QMessageBox::Ok);
            if(result==QMessageBox::Retry)
                saveData();
        }
        return;
    }
    QDataStream out(&saveFile);

    out<<tabWidget->currentIndex();
    out<<tabZoom;
    for(tabTextEdit *tmpTab=end;tmpTab!=NULL;tmpTab=tmpTab->prevTabEdit)
        out<< tmpTab->filename << tmpTab->iconFileName << tmpTab->edit->toPlainText()
          << tmpTab->edit->textCursor().selectionStart() << tmpTab->edit->textCursor().selectionEnd() << tmpTab->textChanged;

    saveFile.close();
}

void MultiTabEdit::loadData()
{
    if(!saveFile.open(QIODevice::ReadOnly))
    {
        if(saveFile.exists())
        {
            int result=QMessageBox::critical(NULL,"notepad--","Datoteke last.dat ni bilo mogoče odpreti za branje.",QMessageBox::Retry | QMessageBox::Ok);
            if(result==QMessageBox::Retry)
                loadData();
        }
        return;
    }
    QDataStream in(&saveFile);

    int index;
    int curIndex;
    int cursStart;
    int cursEnd;
    QString filename;
    QString iconFileName;
    QString text;
    QTextCursor curs;
    bool textChanged;
    QFileInfo tmpfileinfo;

    in >> curIndex;
    in >> tabZoom;
    while(!in.atEnd())
    {
        in >> filename;
        in >> iconFileName;
        in >> text;
        in >> cursStart;
        in >> cursEnd;
        in >> textChanged;

        if(!filename.isEmpty())
        {
            tmpfileinfo.setFile(filename);
            if(!tmpfileinfo.exists())
                continue;
        }

        addTab(filename);
        index = tabWidget->currentIndex();
        setIcon(iconFileName,index);
        getTab(index)->edit->setPlainText(text);
        getTab(index)->edit->zoomOut(tabZoom);
        getTab(index)->zoom=tabZoom;
        curs = getTab(index)->edit->textCursor();
        curs.setPosition(cursStart);
        curs.setPosition(cursEnd,QTextCursor::KeepAnchor);
        getTab(index)->edit->setTextCursor(curs);
        getTab(index)->textChanged = textChanged;
    }

    remTab(0);
    tabWidget->setCurrentIndex(curIndex);
    saveFile.close();
}

void MultiTabEdit::changeIndex(int index)
{
    tabWidget->setCurrentIndex(index);
}

void MultiTabEdit::setTextColor(QColor color)
{
   textColor = color;

   getEdit(getCurrentTabIndex())->setStyleSheet("color: " + color.name() + ";");
}

void MultiTabEdit::setTextFont(QFont font)
{
    textFont = font;
    getEdit(getCurrentTabIndex())->setFont(font);
}

void MultiTabEdit::setLineWrap(bool b)
{
    lineWrap = b;
    getEdit(getCurrentTabIndex())->setLineWrapMode((QPlainTextEdit::LineWrapMode)b);
}

QColor MultiTabEdit::getTextColor()
{
    return textColor;
}

QFont MultiTabEdit::getTextFont()
{
    return  textFont;
}

bool MultiTabEdit::getLineWrap()
{
    return lineWrap;
}


