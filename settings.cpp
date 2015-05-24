#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    saveFile.setFileName("settings.dat");

    ui->pushButtonCancel->setFocusPolicy(Qt::NoFocus);
    ui->pushButtonSave->setFocusPolicy(Qt::NoFocus);
    ui->pushButtonFont->setFocusPolicy(Qt::NoFocus);
}

void Settings::showEvent(QShowEvent *)
{
    if(!saveFile.open(QIODevice::ReadOnly))
    {
        if(saveFile.exists())
            QMessageBox::critical(NULL,"Napaka pri branju","Datoteke settings.dat ni bilo mogoče odpreti za branje.");

        return;
    }
    else
    {
        QDataStream in(&saveFile);

        bool status,lineWrap;

        in >> textColor >> textFont >> status >> lineWrap;

        ui->checkBoxLineWrap->setChecked(lineWrap);
        ui->checkBoxStatusBar->setChecked(status);
        ui->toolButtonColor->setStyleSheet("background-color: " + textColor.name() + ";");
    }

    saveFile.close();
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_pushButtonSave_clicked()
{

    if(!saveFile.open(QIODevice::WriteOnly))
    {
        if(saveFile.exists())
            QMessageBox::critical(NULL,"Napaka pri shranjevanju","Datoteke settings.dat ni bilo mogoče odpreti za pisanje.");

        return;
    }
    QDataStream out(&saveFile);

    out << textColor << textFont << ui->checkBoxStatusBar->isChecked() << ui->checkBoxLineWrap->isChecked();

    saveFile.close();

    this->close();
}

void Settings::on_pushButtonFont_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok,textFont);

    if(ok)
        textFont = font;
}

void Settings::on_toolButtonColor_clicked()
{
    QColor color = QColorDialog::getColor(textColor);

    if(color.isValid())
    {
        textColor = color;
        ui->toolButtonColor->setStyleSheet("background-color: " + textColor.name() + ";");
    }
}

void Settings::on_pushButtonCancel_clicked()
{
    this->close();
}
