#ifndef MULTITABEDIT_H
#define MULTITABEDIT_H

#include <QWidget>
#include <QString>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QPropertyAnimation>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>

struct tabTextEdit
{
    QPlainTextEdit *edit;
    QWidget *tab;

    QString filename;
    QString iconFileName;
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
    int tabZoom;
    QFile saveFile;

public:
    MultiTabEdit(QWidget *parent = 0,QString path="");

    ~MultiTabEdit();

    void addTab(QString filename="");

    bool remTab(int index);

    void resize(QSize size);

    void move (QPoint point);

    void setIcon(QString iconpath, int index);

    int getCurrentTabIndex();

    void animate(int duration,QSize endValue);

    bool tabExists();

    tabTextEdit* getTab(int index);

    void changeIndex(int index);

    bool getTextChanged(int index);

    void setTextChanged(bool change,int index);

    QPlainTextEdit* getEdit(int index);

    QString getFileName(int index);

    void setFileName(QString filename,int index);

    int countTabs();

    void setZoom(int n);
    int getZoom();

    void loadData();
    void saveData();
};

#endif // MULTITABEDIT_H
