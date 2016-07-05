#ifndef DIALOGFIRSTUSE_H
#define DIALOGFIRSTUSE_H

#include <QDialog>

namespace Ui {
class DialogFirstUse;
}

class DialogFirstUse : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFirstUse(QWidget *parent = 0);
    ~DialogFirstUse();

private slots:
    void on_pushButton_accept_clicked();

private:
    Ui::DialogFirstUse *ui;
};

#endif // DIALOGFIRSTUSE_H
