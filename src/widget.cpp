#include "include/widget.h"
#include "ui_widget.h"
#include "QtGlobal"
#include "QDebug"


QTimer *timer_gen_data;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->x_size = 10000;
    this->filter_alogrim = new HL_SimpleKalmanFilter(1,2,1);
    this->notch_filter = new notchfilter(this->x_size,50,5);

    this->x = 0;
    this->series = new QLineSeries();
    this->series->setColor(QColor(Qt::GlobalColor::darkGreen));
    this->series_filter = new QLineSeries();
    this->series_filter->setColor(QColor(Qt::GlobalColor::red));
    for(uint16_t i = 0;i<this->x_size;i++){
        this->x = i;
        float y =  0;
        this->buff.append(QPointF(this->x,y));
        this->buff_filter.append(QPointF(this->x,this->filter(y)));
    }
    this->min_y = 0;
    this->max_y = 0;
    this->series->setUseOpenGL(true);
    this->series_filter->setUseOpenGL(true);

    this->series->setName("signal");
    this->series_filter->setName("signal filter");
    this->series->append(this->buff);
    this->series_filter->append(this->buff_filter);
    this->chart = new QChart();
    this->chart->addSeries(this->series);
    this->chart->addSeries(this->series_filter);
    this->chart->createDefaultAxes();
    this->chart->setTitle("Chart");
    this->chart->legend()->setVisible(true);
    this->chart->legend()->setAlignment(Qt::AlignBottom);
    this->chart->axisX()->setRange(0,this->x_size);
    this->chart->axisX()->setTitleText("time");
    this->chart->axisY()->setTitleText("value");
//    this->chart->setTheme(QChart::ChartTheme::ChartThemeBlueIcy);
    this->chartview = new QChartView(this->chart);
    this->chartview->setRenderHint(QPainter::Antialiasing);

    ui->horizontalFrame->addWidget(this->chartview);

    this->timer = new QTimer(this);

    connect(this->timer,SIGNAL(timeout()),this,SLOT(timeout()));
    this->timer->start();

    timer_gen_data = new QTimer(this);
    connect(timer_gen_data,SIGNAL(timeout()),this,SLOT(gen_data()));

    fps_timer.start();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::update_data(QList<QPointF> list)
{
    this->buff.append(list);
}

void Widget::timeout()
{
    static int frameCount = 0;
    static QString labelText = QStringLiteral("FPS: %1");
    if(this->buff.count() >= this->x_size){
        this->series->replace(this->buff);
        this->series_filter->replace(this->buff_filter);
        this->chart->axisX()->setRange(this->series->at(0).x(),this->series->at(this->x_size-1).x());
        this->chart->axisY()->setRange(this->min_y-3,this->max_y+3);
        for(uint32_t i = 0;i<this->x_size;i++){
            this->buff.remove(0);
            this->buff_filter.remove(0);
        }

        frameCount++;

        int elapsed = fps_timer.elapsed();
        if (elapsed >= 1000) {
            elapsed = fps_timer.restart();
            qreal fps = qreal(0.1 * int(10000.0 * (qreal(frameCount) / qreal(elapsed))));
            ui->label->setText(labelText.arg(QString::number(fps, 'f', 1)));
            ui->label->adjustSize();
            frameCount = 0;
        }
    }
}

void Widget::gen_data()
{
    this->min_y = 0;
    this->max_y = 0;
    for(uint32_t i = 0;i<this->x_size;i++){
        float y = 5*sin(this->x*3.14/180) + (rand() % ((2 + 1) - (-2)) + (-2));
        float y_fil = this->filter(y);
        this->x++;
        this->buff.append(QPointF(this->x,y));
        this->buff_filter.append(QPointF(this->x,y_fil));
        if(this->min_y > y) this->min_y = y;
        if(this->max_y < y) this->max_y = y;
    }
}

float Widget::filter(float y)
{

    double fil = this->filter_alogrim->update(y);

    return (float) this->notch_filter->step(fil);
}


void Widget::on_pushButton_clicked()
{
    if(ui->pushButton->text() == "start"){
        timer_gen_data->start();
        ui->pushButton->setText("stop");
    }
    else{
        timer_gen_data->stop();
        ui->pushButton->setText("start");
    }
}

