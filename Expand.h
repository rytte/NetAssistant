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

    QString getBufferText(int bufferIndex) {return m_textEdit[bufferIndex]->toPlainText();}
    QString getTimerText() {return m_timeEdit->text();};
    void setBufferText1(QString str) {m_textEdit[0]->setText(str);}
    void setBufferText2(QString str) {m_textEdit[1]->setText(str);}
    void setBufferText3(QString str) {m_textEdit[2]->setText(str);}
    void setTimerText(QString str) {m_timeEdit->setText(str);}
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
    QLineEdit* m_FileEdit;
    QPushButton* m_FileSelectBtn;
    QPushButton* m_FileReadBtn;

    //备用缓冲区
    QTabWidget* m_tabWidget;
    QPushButton* m_replaceBufferBtn;
    QTextEdit* m_textEdit[3];
};

#endif // EXPAND_H
