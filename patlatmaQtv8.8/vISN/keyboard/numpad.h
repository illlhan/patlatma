#ifndef NUMPAD_H
#define NUMPAD_H

#include <QWidget>
#include <QtGui>
#include <QLineEdit>
#include <QPushButton>


namespace Ui {
class Numpad;
}

class Numpad : public QWidget
{
    Q_OBJECT
    
public:
    explicit Numpad(QWidget *parent = 0);
    void setLineEdit(QLineEdit * );
    ~Numpad();
    QPushButton *enterButton;

private slots:
    void keyboardHandler();
    void on_shift_clicked();
    void on_char_2_toggled(bool checked);
    void on_clear_clicked();
    void on_enterButton_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_returnPressed();

    void on_backButton_clicked();

private:
    Ui::Numpad *ui;
    QString outputText;
    QLineEdit *outputLineEdit;
    bool shift;
    Numpad *lineEditkeyboard;

};

#endif // NUMPAD_H
