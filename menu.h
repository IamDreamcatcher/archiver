#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include "pack.h"
#include "unpack.h"

class menu;

namespace Ui { class menu; }

class menu : public QWidget {
    Q_OBJECT
    public:
        explicit menu(QWidget *parent = nullptr);
        ~menu();
    private:
        Ui::menu *ui;
    signals:

    private slots:
        void on_button1_clicked();
        void on_button2_clicked();
};
#endif
