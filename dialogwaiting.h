#ifndef DIALOGWAITING_H
#define DIALOGWAITING_H

#include <QDialog>

namespace Ui {
class DialogWaiting;
}

class DialogWaiting : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWaiting(QWidget *parent = 0);
    ~DialogWaiting();

private:
    Ui::DialogWaiting *ui;
};

#endif // DIALOGWAITING_H
