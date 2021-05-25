#include "menu.h"
#include "ui_menu.h"

menu::menu(QWidget *parent) : QWidget(parent), ui(new Ui::menu) {
    ui->setupUi(this);
}

menu::~menu() {
    delete ui;
}

void menu::on_button1_clicked() {
    pack* packer = new pack();
    packer -> show();
    this -> close();
}
void menu::on_button2_clicked() {
    unpack* unpacker = new unpack();
    unpacker -> show();
    this -> close();
}
