#include "multitabedit.h"

MultiTabEdit::MultiTabEdit(QWidget *widget,QString path)
{
    tabWidget = new QTabWidget(widget);
    connect(tabWidget,&QTabWidget::currentChanged,this,&MultiTabEdit::tabChange);

    QString tabName;

    tabTextEdit *tmpTab = new tabTextEdit;
    tmpTab->tab = new QWidget;
    tmpTab->edit = new QPlainTextEdit(tmpTab->tab);

    tmpTab->icon.addFile("resources\\icons\\save.png");
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

    tabWidget->addTab(tmpTab->tab,tmpTab->icon,tabName);
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

    tmpTab->icon.addFile("resources\\icons\\save.png");
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

    tabWidget->addTab(tmpTab->tab,tmpTab->icon,tabName);
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

    if(curTab==end && curTab==start)
    {
        tabTextEdit *tmpTab = new tabTextEdit;
        tmpTab->tab = new QWidget;
        tmpTab->edit = new QPlainTextEdit(tmpTab->tab);

        tmpTab->icon.addFile("resources\\icons\\save.png");
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

        tabWidget->addTab(tmpTab->tab,tmpTab->icon,"neimenovana");
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
    editAnimation->setEndValue(endValue);
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
}

bool MultiTabEdit::getTextChanged(int index)
{
    return &getTab(index)->textChanged;
}

void MultiTabEdit::setTextChanged(bool change, int index)
{
    getTab(index)->textChanged = change;
}

void MultiTabEdit::setIcon(QIcon icon, int index)
{
    tabWidget->setTabIcon(index,icon);
}

int MultiTabEdit::coutTabs()
{
    return tabWidget->count();
}


