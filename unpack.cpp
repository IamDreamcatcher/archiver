#include "unpack.h"
#include "ui_unpack.h"
#include <QMessageBox>
#include <QDebug>

unpack::unpack(QWidget *parent) : QWidget(parent), ui(new Ui::unpack) {
    ui -> setupUi(this);
}

unpack::~unpack() {
    delete ui;
}

void unpack::on_button1_clicked() {
    QString path_from = ui -> textEdit -> toPlainText();
    QString path_to = ui -> textEdit_2 -> toPlainText() + "/\\";
    archiver my_archiver;
    QMessageBox message_box;
    try {
        my_archiver.un_pack(path_from.toStdString(), path_to.toStdString());
    }  catch (std::string s) {
        message_box.setText(QString::fromStdString(s));
        message_box.exec();
        my_archiver.free();
        return;
    }
    message_box.setText("Complete");
    message_box.exec();
}
void unpack::on_button2_clicked() {
    menu* my_menu = new menu();
    my_menu -> show();
    this -> close();
}

