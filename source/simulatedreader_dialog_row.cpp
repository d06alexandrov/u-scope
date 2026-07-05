#include "simulatedreader_dialog_row.h"

#include "ui_simulatedreader_dialog_row.h"

SimulatedReaderDialogRow::SimulatedReaderDialogRow(QWidget *parent)
    : QWidget{ parent }
    , ui(new Ui::SimulatedReaderDialogRow())
{
    ui->setupUi(this);

    connect(ui->modifyButton, &QPushButton::clicked, this,
            &SimulatedReaderDialogRow::editRequested);
    connect(ui->deleteButton, &QPushButton::clicked, this,
            &SimulatedReaderDialogRow::deleteRequested);
}

SimulatedReaderDialogRow::~SimulatedReaderDialogRow()
{
    delete ui;
}

void SimulatedReaderDialogRow::set_text(const QString &text)
{
    ui->label->setText(text);
}
