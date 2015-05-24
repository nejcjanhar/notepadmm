#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    ui->pushButtonCancel->setFocusPolicy(Qt::NoFocus);
    ui->pushButtonSave->setFocusPolicy(Qt::NoFocus);
    ui->pushButtonFont->setFocusPolicy(Qt::NoFocus);

    //load function

    ui->toolButtonColor->setStyleSheet("background-color: " + textColor.name());
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_pushButtonSave_clicked()
{
    //save function
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
        ui->toolButtonColor->setStyleSheet("background-color: " + textColor.name());
    }
}
