#ifndef AGENTTEST_H
#define AGENTTEST_H

#include <QWidget>
#include <iostream>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QLineEdit>
#include <QSpinBox>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QLabel>
#include <QVector>

class MainWindow;

class AgentTest: public QWidget
{
public:
    AgentTest(QWidget *parent = nullptr);



    //滚动时间间隔（四个等级：90ms及以上、60-89ms、30-59ms、0-29ms）
    enum SCROLL_INTERVAL{
        INTERVAL_1 = 90,
        INTERVAL_2 = 60,
        INTERVAL_3 = 40,
        INTERVAL_4 = 25
    };

public slots:
    void slot_BuildBtnSZ();
    void slot_BuildBtnFZ();
    void slot_ConvertJsonBtnSZ();
    void slot_CommandBoxChanged();
    void slot_LimitStyleBoxChanged();

private:
    void uiBuild();

private:
    MainWindow* m_myParent;
    QVector<QWidget*> m_widgetVec;

    QComboBox* m_commandBox;
    QComboBox* m_textBox;
    QComboBox* m_backgroundColorBox;
    QComboBox* m_textColorBox;
    QComboBox* m_textDirectionBox;
    QComboBox* m_textScrollIntervalBox;
    QLineEdit* m_textFontSizeEdit;
    QComboBox* m_limitStyleBox;
    QSpinBox* m_limitTimeBox;
    QDateTimeEdit* m_limitTimeStartBox;
    QDateTimeEdit* m_limitTimeEndBox;
    QSpinBox* m_volumeBox;
    QSpinBox* m_msgPriorityBox;

    //布局管理器
    QVBoxLayout* m_vBox;
    QHBoxLayout* m_hBox[14];

    //Json生成按钮
    QPushButton* m_buildBtnSZ;
    QPushButton* m_buildBtnFZ;
    QPushButton* m_jsonConvertSZ;
    QPushButton* m_udpSendBtn;

    //ui
    QLabel* ui_label[13];
};

#endif // AGENTTEST_H
