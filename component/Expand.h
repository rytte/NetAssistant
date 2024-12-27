#ifndef EXPAND_H
#define EXPAND_H

#include <QWidget>
#include <QTimer>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTabWidget>

class MainWindow;   //声明

class Expand : public QWidget
{
public:
    Expand(QWidget *parent = nullptr);

    void stopTimerClose();

public slots:
    void slot_StopTimerBtn();
    void slot_ReplaceBufferBtn();
    void slot_FileSelectBtn();
    void slot_FileReadBtn();

private:
    void uiBuild();

private:
    MainWindow* m_myParent;

    //定时器
    QLineEdit* m_timeEdit;
    QPushButton* m_stopTimerBtn;
    QTimer* m_stopTimer;
    double m_secTime;

    //倒计时定时器
    QTimer* m_countDownTimer;
    QLabel* m_countDownLabel;

    //文件
    QLineEdit* m_fileEdit;

    //备用缓冲区
    QTabWidget* m_tabWidget;
    QTextEdit* m_textEdit[3];
};

#endif // EXPAND_H
