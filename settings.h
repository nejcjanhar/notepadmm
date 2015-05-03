#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QDebug>
#include <QFile>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

private slots:
    void on_pushButtonSave_clicked();

    void on_pushButtonCancel_clicked();

private:
    Ui::Settings *ui;

    QFile saveFile;
};

#endif // SETTINGS_H
