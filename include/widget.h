#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QTimer>
#include <QPointF>
#include <QQueue>
#include <QAbstractAxis>
#include <include/hl_simplekalmanfilter.h>
#include "QElapsedTimer"
#include "include/notchfilter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void update_data(QList<QPointF> list);
public slots:
    void timeout();
    void gen_data();
private slots:
    void on_pushButton_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_checkBox_2_stateChanged(int arg1);

private:
    float filter(float y);
    Ui::Widget *ui;
    QChart *chart;
    QChartView *chartview;
    QLineSeries *series;
    QLineSeries *series_filter;
    QTimer *timer;
    QList<QPointF> buff;
    QList<QPointF> buff_filter;
    uint32_t x_size;
    uint32_t x;
    float min_y,max_y;
    HL_SimpleKalmanFilter *filter_alogrim;
    QElapsedTimer fps_timer;
    notchfilter *notch_filter;
};
#endif // WIDGET_H
