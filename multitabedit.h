#ifndef MULTITABEDIT_H
#define MULTITABEDIT_H

#include <QWidget>
#include <QString>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QPropertyAnimation>
#include <QFileInfo>

struct tabTextEdit
{
    QPlainTextEdit *edit;
    QWidget *tab;

    QString filename;
    QIcon icon;
    int zoom;
    bool textChanged;
    bool undoAvaible;
    bool redoAvaible;
    bool canCopy;
    int index;

    tabTextEdit *nextTabEdit;
    tabTextEdit *prevTabEdit;
};

class MultiTabEdit : public QWidget
{
Q_OBJECT
signals:
    void tabChange(int index);

    void tabTextChange();

    void tabTextCurChange();

    void tabTextUndoAvaible(bool b);

    void tabTextRedoAvaible(bool b);

    void tabTextCopyAvaible(bool b);


private:
    QTabWidget *tabWidget;
    tabTextEdit *start;
    tabTextEdit *end;

public:
    MultiTabEdit(QWidget *parent = 0,QString path="");

    ~MultiTabEdit();

    void addTab(QString filename="");

    bool remTab(int index);

    void resize(QSize size);

    void move (QPoint point);

    void setIcon(QIcon icon, int index);

    int getCurrentTabIndex();

    void animate(int duration,QSize endValue);

    bool tabExists();

    tabTextEdit* getTab(int index);

    bool getTextChanged(int index);

    void setTextChanged(bool change,int index);

    QPlainTextEdit* getEdit(int index);

    QString getFileName(int index);

    void setFileName(QString filename,int index);

    int coutTabs();
};

#endif // MULTITABEDIT_H
