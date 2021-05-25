#ifndef PACK_H
#define PACK_H

#include <QWidget>
#include "archiver.h"
#include "menu.h"

class pack;

namespace Ui { class pack; }

class pack : public QWidget {
    Q_OBJECT
    public:
        explicit pack(QWidget *parent = nullptr);
        ~pack();
    private:
        Ui::pack *ui;
    signals:

    private slots:
        void on_button1_clicked();
        void on_button2_clicked();
};
#endif
