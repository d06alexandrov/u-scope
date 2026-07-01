#include "simulatedreader_dialog_row.h"

#include "ui_simulatedreader_dialog_row.h"

SimulatedReaderDialogRow::SimulatedReaderDialogRow(QWidget *parent)
    : QWidget{ parent }
    , ui(new Ui::SimulatedReaderDialogRow())
{
    ui->setupUi(this);
}

void SimulatedReaderDialogRow::set_text(const QString &text)
{
    ui->label->setText(text);
}
