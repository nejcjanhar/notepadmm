#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_pushButtonCancel_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok);

    if(ok)
    qDebug() << font.family() << endl;
}

void Settings::on_pushButtonSave_clicked()
{
    QColor color = QColorDialog::getColor();

    if(color.isValid())
    qDebug() << color.name() << endl;
}
