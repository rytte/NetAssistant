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
    enum {
        WIDGET_NUM = 14
    };

    enum WidgetID{
        COMMAND = 0,
        TEXT = 1,
        BACKGROUND_COLOR = 2,
        TEXT_COLOR = 3,
        TEXT_DIRECTION = 4,
        TEXT_SCROLL_INTERVAL = 5,
        TEXT_FONT_SIZE = 6,
        LIMIT_STYLE = 7,
        LIMIT_TIME = 8,
        LIMIT_TIME_START = 9,
        LIMIT_TIME_END = 10,
        MSG_PRIORITY = 11,
        VOLUME = 12,
        LIVE_ADDRESS = 13
    };

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
    QDateTimeEdit* m_limitTimeStartEdit;
    QDateTimeEdit* m_limitTimeEndEdit;
    QSpinBox* m_msgPriorityBox;
    QSpinBox* m_volumeBox;
    QLineEdit* m_liveAddressEdit;

    //ui
    QLabel* ui_label[WIDGET_NUM];
};

#endif // AGENTTEST_H
