#include "pack.h"
#include "ui_pack.h"
#include <QMessageBox>

pack::pack(QWidget *parent) : QWidget(parent), ui(new Ui::pack) {
    ui -> setupUi(this);
}

pack::~pack() {
    delete ui;
}

void pack::on_button1_clicked() {
    QString path_from = ui -> textEdit -> toPlainText();
    QString path_to = ui -> textEdit_2 -> toPlainText();
    QString name_of_archive = ui -> textEdit_3 -> toPlainText();
    QString all_path_to = path_to + name_of_archive + ".arc";
    archiver my_archiver;
    QMessageBox message_box;
    try {
        my_archiver.pack(path_from.toStdString(), all_path_to.toStdString());
    }  catch (std::string s) {
        message_box.setText(QString::fromStdString(s));
        message_box.exec();
        my_archiver.free();
        return;
    }
    message_box.setText("Complete");
    message_box.exec();
}
void pack::on_button2_clicked() {
    menu* my_menu = new menu();
    my_menu -> show();
    this -> close();
}

