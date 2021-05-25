#ifndef UNPACK_H
#define UNPACK_H

#include <QWidget>
#include "archiver.h"
#include "menu.h"

class unpack;

namespace Ui { class unpack; }

class unpack : public QWidget {
    Q_OBJECT
    public:
        explicit unpack(QWidget *parent = nullptr);
        ~unpack();
    private:
        Ui::unpack *ui;
    signals:

    private slots:
        void on_button1_clicked();
        void on_button2_clicked();
};
#endif
