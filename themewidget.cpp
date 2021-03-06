/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "themewidget.h"
#include "ui_themewidget.h"

#include <iostream>
#include <fstream>
#include <string>

#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCore/QRandomGenerator>
#include <QtCharts/QBarCategoryAxis>
#include <QtWidgets/QApplication>
#include <QtCharts/QValueAxis>

const double fatigue_coef[] = {0.06, 0.07, 0.09, 0.14, 0.19, 0.22, 0.23};

const double fitness_coef_factor = 1.4;
const double fitness_coef[] = {0.007,0.008,0.009,0.01,0.011,0.012,0.013,0.014,0.015,0.016,0.017,0.018,0.019,0.02,0.021,0.022,0.023,0.025,0.026,0.027,0.028,0.03,0.0315,0.032,0.032,0.033,0.034,0.035,0.036,0.037,0.037,0.037,0.036,0.035,0.034,0.0335,0.033,0.032,0.031};

class TrainingWeek {
public:
        int week_number;
        int year;
        int month;
        double sum_hour;
        double sum_tss;
        double sum_km;
        double sum_hour_objective;
        double sum_tss_objective;
        double sum_km_objective;
        QString category;
        QString comment;
};

class TrainingItem {
public:
    QString weather;
    QDate date;
    QString training;
    double hour;
    unsigned short int feeling;
    QString daily_objective;
    double TSS;
    double Km_per_day;
    double hour_objective;
    double TSS_objective;
    QString category;
    QString muscu;
    QString muscu_objective;
    double km_per_week_objective;
    double hour_per_week_objective;
    double TSS_per_week_objective;
};

std::vector<TrainingItem> loadTrainingsFromFile(std::string filename) {
    std::vector<TrainingItem> database;
    std::string line;
    std::ifstream myfile (filename);

    if (!myfile.is_open()) {
        std::cout<<"Error: Cannot load training data from "<<filename<<std::endl;
        return std::vector<TrainingItem>();
    }

    while (std::getline(myfile, line)) {
        TrainingItem current_item;
        std::string buffer;
        bool in_apo = false;
        int count_items = 0;
        int count_lines = 0;

        for (const char &c: line) {
            if (c == '"') {
                in_apo = !in_apo;
            } else if (c == ',' && !in_apo) {
                // save current item and start parsing next one
                if (count_items == 0) {
                    current_item.weather = QString::fromStdString(buffer);
                } else if (count_items == 1) {
                    current_item.date = QDate::fromString(QString::fromStdString(buffer));
                } else if (count_items == 2) {
                     current_item.training = QString::fromStdString(buffer);
                } else if (count_items == 3) {
                    try {
                        current_item.hour = std::stod(buffer);
                    } catch (...) {
                        std::cout<<"Error: "<<buffer<<"is not a double"<<std::endl;
                    }
                } else if (count_items == 4) {
                    current_item.feeling = (unsigned short int)std::stoi(buffer);
                } else if (count_items == 5) {
                    current_item.daily_objective = QString::fromStdString(buffer);
                } else if (count_items == 6) {
                    try {
                        current_item.TSS = std::stod(buffer);
                    } catch (...) {
                        std::cout<<"Error: "<<buffer<<"is not a double"<<std::endl;
                    }
                } else if (count_items == 7) {
                    try {
                        current_item.Km_per_day = std::stod(buffer);
                    } catch (...) {
                        std::cout<<"Error: "<<buffer<<"is not a double"<<std::endl;
                    }
                } else if (count_items == 8) {
                    current_item.hour_objective = std::stod(buffer);
                } else if (count_items == 9) {
                    current_item.TSS_objective = std::stod(buffer);
                } else if (count_items == 10) {
                    current_item.category = QString::fromStdString(buffer);
                } else if (count_items == 11) {
                    current_item.muscu = QString::fromStdString(buffer);
                } else if (count_items == 12) {
                    current_item.muscu_objective = QString::fromStdString(buffer);
                } else if (count_items == 13) {
                    current_item.km_per_week_objective = std::stod(buffer);
                } else if (count_items == 14) {
                    current_item.hour_per_week_objective = std::stod(buffer);
                } else if (count_items == 15) {
                    current_item.TSS_per_week_objective = std::stod(buffer);
                    database.push_back(current_item);
                } else {
                    std::cout<<"Error: Line "<<count_lines<<"Too many items"<<std::endl;
                }
                count_items++;
                buffer.clear();
            } else {
                buffer.push_back(c);
            }
        }
        count_lines++;
    }
    myfile.close();

    return database;
}

int saveTrainingsToFile(std::string filename, const std::vector<TrainingItem> &trainings) {
    std::ofstream myfile;
    size_t line_count = 0;
    myfile.open(filename);
    if (!myfile.is_open()) {
        std::cout<<"Error: Cannot save training to "<<filename<<std::endl;
        return 1;
    }
    for (auto it = trainings.begin(); it != trainings.end(); it++) {
        myfile << "\"" << it->weather.toUtf8().constData() << "\",";
        myfile << "\"" << it->date.toString().toUtf8().constData() << "\",";
        myfile << "\"" << it->training.toUtf8().constData() << "\",";
        myfile << "\"" << it->hour << "\",";
        myfile << "\"" << it->feeling << "\",";
        myfile << "\"" << it->daily_objective.toUtf8().constData() << "\",";
        myfile << "\"" << it->TSS << "\",";
        myfile << "\"" << it->Km_per_day << "\",";
        myfile << "\"" << it->hour_objective << "\",";
        myfile << "\"" << it->TSS_objective << "\",";
        myfile << "\"" << it->category.toUtf8().constData() << "\",";
        myfile << "\"" << it->muscu.toUtf8().constData() << "\",";
        myfile << "\"" << it->muscu_objective.toUtf8().constData() << "\",";
        myfile << "\"" << it->km_per_week_objective << "\",";
        myfile << "\"" << it->hour_per_week_objective << "\",";
        myfile << "\"" << it->TSS_per_week_objective << "\","<<std::endl;
        line_count++;
    }
    std::cout<<"Training datas saved to file "<<filename<<" ("<<line_count<<" lines)"<<std::endl;
    myfile.close();
    return 0;
}

TrainingItem blankDay() {
    TrainingItem tmp;
    tmp.weather = QString("Clear");
    tmp.training = QString();
    tmp.date = QDate::fromString("01/01/2050","dd/MM/yyyy");
    tmp.hour = 0.0;
    tmp.feeling = 0;
    tmp.daily_objective = QString();
    tmp.TSS = 0;
    tmp.Km_per_day = 0;
    tmp.hour_objective = 0;
    tmp.TSS_objective = 0;
    tmp.category = QString();
    tmp.muscu = QString();
    tmp.muscu_objective = QString();
    tmp.km_per_week_objective = 0;
    tmp.hour_per_week_objective = 0;
    tmp.TSS_per_week_objective = 0;
    return tmp;
}

TrainingWeek blankWeek() {
    TrainingWeek tmp;
    tmp.week_number = 0;
    tmp.year = 0;
    tmp.month = 0;
    tmp.sum_hour = 0;
    tmp.sum_tss = 0;
    tmp.sum_km = 0;
    tmp.sum_hour_objective = 0;
    tmp.sum_tss_objective = 0;
    tmp.sum_km_objective = 0;
    QString category = QString();
    QString comment = QString();
    return tmp;
}

void debugPrintTraining(const std::vector<TrainingItem> &trainings)
{
    std::cout<<"Trainings contains: "<<trainings.size()<<" entry"<<std::endl;
    for (auto it = trainings.begin(); it!= trainings.end(); it++) {
        std::cout<<"Date: "<<it->date.toString().toStdString()<<std::endl;
        std::cout<<"Weather: "<<it->weather.toStdString()<<std::endl;
        std::cout<<"Training: "<<it->training.toStdString()<<std::endl;
    }
    std::cout<<"-----------------"<<std::endl;
}

ThemeWidget::ThemeWidget(QWidget *parent) :
    QWidget(parent),
    m_listCount(3),
    m_valueMax(10),
    m_valueCount(7),
    m_ui(new Ui_ThemeWidgetForm)
{
    m_ui->setupUi(this);

    mTrainings = loadTrainingsFromFile("test_training.csv");
    debugPrintTraining(mTrainings);
    updateWeekSummary();
    updateFatigue();

    // Create charts
    QChartView *chartView;
    /*
    chartView = new QChartView(createLineChart());
    m_ui->GraphGrid->addWidget(chartView, 1, 2);
    m_charts << chartView;
    */

    chartView = new QChartView(createScatterChart());
    m_ui->GraphGrid->addWidget(chartView, 2, 2);
    m_charts << chartView;

    // Set the colors from the light theme as default ones
    QPalette pal = qApp->palette();
    pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    qApp->setPalette(pal);

    mTableHeader<<"Date"<<"Weather"<<"Description"<<"Target"<<"TSS"<<"TSS obj."<<"Km"<<"Km obj."<<"Feeling"<<"Duration"<<"Duration obj."<<"Musculation"<<"Muscu Target";

    m_ui->CalendarWidget->setRowCount(1);
    m_ui->CalendarWidget->setColumnCount(13);
    m_ui->CalendarWidget->setHorizontalHeaderLabels(mTableHeader);
    m_ui->CalendarWidget->verticalHeader()->setVisible(false);

    m_ui->WeekWidget->setRowCount(7);
    m_ui->WeekWidget->setColumnCount(13);
    m_ui->WeekWidget->setHorizontalHeaderLabels(mTableHeader);
    m_ui->WeekWidget->verticalHeader()->setVisible(false);

    QDate today = QDate::currentDate();
    m_ui->dateEdit->setDate(today);

    updateUI();
}

ThemeWidget::~ThemeWidget()
{
    delete m_ui;
}

/*
QChart *ThemeWidget::createAreaChart() const
{
    QChart *chart = new QChart();
    chart->setTitle("Area chart");

    // The lower series initialized to zero values
    QLineSeries *lowerSeries = 0;
    QString name("Series ");
    int nameIndex = 0;
    for (int i(0); i < m_dataTable.count(); i++) {
        QLineSeries *upperSeries = new QLineSeries(chart);
        for (int j(0); j < m_dataTable[i].count(); j++) {
            Data data = m_dataTable[i].at(j);
            if (lowerSeries) {
                const QVector<QPointF>& points = lowerSeries->pointsVector();
                upperSeries->append(QPointF(j, points[i].y() + data.first.y()));
            } else {
                upperSeries->append(QPointF(j, data.first.y()));
            }
        }
        QAreaSeries *area = new QAreaSeries(upperSeries, lowerSeries);
        area->setName(name + QString::number(nameIndex));
        nameIndex++;
        chart->addSeries(area);
        lowerSeries = upperSeries;
    }

    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, m_valueCount - 1);
    chart->axes(Qt::Vertical).first()->setRange(0, m_valueMax);
    // Add space to label to add space between labels and axis
    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");

    return chart;
}
*/
/*
QChart *ThemeWidget::createBarChart(int valueCount) const
{
    Q_UNUSED(valueCount);
    QChart *chart = new QChart();
    chart->setTitle("Bar chart");

    QStackedBarSeries *series = new QStackedBarSeries(chart);
    for (int i(0); i < m_dataTable.count(); i++) {
        QBarSet *set = new QBarSet("Bar set " + QString::number(i));
        for (const Data &data : m_dataTable[i])
            *set << data.first.y();
        series->append(set);
    }
    chart->addSeries(series);

    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).first()->setRange(0, m_valueMax * 2);
    // Add space to label to add space between labels and axis
    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");

    return chart;
}
*/
/*
QChart *ThemeWidget::createLineChart() const
{
    //![1]
    QChart *chart = new QChart();
    chart->setTitle("Line chart");
    //![1]

    //![2]
    QString name("Series ");
    int nameIndex = 0;
    for (const DataList &list : m_dataTable) {
        QLineSeries *series = new QLineSeries(chart);
        for (const Data &data : list)
            series->append(data.first);
        series->setName(name + QString::number(nameIndex));
        nameIndex++;
        chart->addSeries(series);
    }
    //![2]

    //![3]
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, m_valueMax);
    chart->axes(Qt::Vertical).first()->setRange(0, m_valueCount);
    //![3]
    //![4]
    // Add space to label to add space between labels and axis
    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");
    //![4]

    return chart;
}
*/
/*
QChart *ThemeWidget::createSplineChart() const
{
    QChart *chart = new QChart();
    chart->setTitle("Spline chart");
    QString name("Series ");
    int nameIndex = 0;
    for (const DataList &list : m_dataTable) {
        QSplineSeries *series = new QSplineSeries(chart);
        for (const Data &data : list)
            series->append(data.first);
        series->setName(name + QString::number(nameIndex));
        nameIndex++;
        chart->addSeries(series);
    }

    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, m_valueMax);
    chart->axes(Qt::Vertical).first()->setRange(0, m_valueCount);

    // Add space to label to add space between labels and axis
    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");
    return chart;
}
*/

QChart *ThemeWidget::createScatterChart() const
{
    // scatter chart
    QChart *chart = new QChart();
    chart->setTitle("Distance per week");

    QScatterSeries *serie = new QScatterSeries(chart);
    serie->setName(QString("Km"));
    for (const TrainingWeek &tmp : mWeeks) {
        serie->append(QPoint(tmp.week_number, tmp.sum_km)); // TODO handle many years
        std::cout<<"Week "<<tmp.week_number<<": "<<tmp.sum_km<<"km"<<std::endl;
    }
    chart->addSeries(serie);

    chart->createDefaultAxes();
    //chart->axes(Qt::Horizontal).first()->setRange(0, m_valueMax);
    //chart->axes(Qt::Vertical).first()->setRange(0, m_valueCount);
    // Add space to label to add space between labels and axis
    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");
    return chart;
}

QColor computeColor(double done, double todo) {
    int red = 255*(1-done/todo);
    int green = 255*done/todo;
    red = (red<0)?0: red;
    red = (red>255)?255: red;
    green = (green<0)?0: green;
    green = (green>255)?255: green;
    return QColor(red, green, 0, 127);
}

void ThemeWidget::updateCalendar() {
    m_ui->CalendarWidget->setRowCount(mTrainings.size());
    size_t item_count = 0;
    m_ui->CalendarWidget->clearContents();
    QTableWidgetItem *test;
    for (auto it = mTrainings.begin(); it!= mTrainings.end(); it++) {
        m_ui->CalendarWidget->setItem(item_count, 0, new QTableWidgetItem(it->date.toString()));
        m_ui->CalendarWidget->setItem(item_count, 1, new QTableWidgetItem(it->weather));
        m_ui->CalendarWidget->setItem(item_count, 2, new QTableWidgetItem(it->training));
        m_ui->CalendarWidget->setItem(item_count, 3, new QTableWidgetItem(it->daily_objective));
        test = new QTableWidgetItem(QString::number(it->TSS));
        test->setBackgroundColor(computeColor(it->TSS, it->TSS_objective));
        m_ui->CalendarWidget->setItem(item_count, 4, test);
        m_ui->CalendarWidget->setItem(item_count, 5, new QTableWidgetItem(QString::number(it->TSS_objective)));
        test =  new QTableWidgetItem(QString::number(it->Km_per_day));
        test->setBackgroundColor(computeColor(it->Km_per_day, it->km_per_week_objective));
        m_ui->CalendarWidget->setItem(item_count, 6, test);
        m_ui->CalendarWidget->setItem(item_count, 7, new QTableWidgetItem(QString::number(it->km_per_week_objective)));
        test =  new QTableWidgetItem(QString::number(it->feeling));
        test->setBackgroundColor(computeColor(it->feeling, 6));
        m_ui->CalendarWidget->setItem(item_count, 8, test);
        test =  new QTableWidgetItem(QString::number(it->hour));
        test->setBackgroundColor(computeColor(it->hour, it->hour_objective));
        m_ui->CalendarWidget->setItem(item_count, 9, test);
        m_ui->CalendarWidget->setItem(item_count, 10, new QTableWidgetItem(QString::number(it->hour_objective)));
        m_ui->CalendarWidget->setItem(item_count, 11, new QTableWidgetItem(it->muscu));
        m_ui->CalendarWidget->setItem(item_count, 12, new QTableWidgetItem(it->muscu_objective));
        item_count++;
    }
}

void ThemeWidget::updateMyWeek() {
    QDate today = QDate::currentDate();

    m_ui->dateEdit_2->setDate(today);
    m_ui->todayLabel->setText(today.toString());

    // Search training of the week
    size_t count = 0;
    m_ui->WeekWidget->clearContents();
    double sum_km_done = 0;
    double sum_km_objective = 0;
    double sum_hour_done = 0;
    double sum_hour_objective = 0;
    double sum_tss_done = 0;
    double sum_tss_objective = 0;
    QTableWidgetItem *test;
    for (auto it = mTrainings.begin(); it!= mTrainings.end(); it++) {
        if (today.weekNumber() == it->date.weekNumber()) {
            sum_km_done += it->Km_per_day;
            sum_km_objective += it->km_per_week_objective;
            sum_hour_done += it->hour;
            sum_hour_objective += it->hour_objective;
            sum_tss_done += it->TSS;
            sum_tss_objective += it->TSS_objective;
            m_ui->WeekWidget->setItem(count, 0, new QTableWidgetItem(it->date.toString()));
            m_ui->WeekWidget->setItem(count, 1, new QTableWidgetItem(it->weather));
            m_ui->WeekWidget->setItem(count, 2, new QTableWidgetItem(it->training));
            m_ui->WeekWidget->setItem(count, 3, new QTableWidgetItem(it->daily_objective));
            test = new QTableWidgetItem(QString::number(it->TSS));
            test->setBackgroundColor(computeColor(it->TSS, it->TSS_objective));
            m_ui->WeekWidget->setItem(count, 4, test);
            m_ui->WeekWidget->setItem(count, 5, new QTableWidgetItem(QString::number(it->TSS_objective)));
            test = new QTableWidgetItem(QString::number(it->Km_per_day));
            test->setBackgroundColor(computeColor(it->Km_per_day, it->km_per_week_objective));
            m_ui->WeekWidget->setItem(count, 6, test);
            m_ui->WeekWidget->setItem(count, 7, new QTableWidgetItem(QString::number(it->km_per_week_objective)));
            test =  new QTableWidgetItem(QString::number(it->feeling));
            test->setBackgroundColor(computeColor(it->feeling, 6));
            m_ui->WeekWidget->setItem(count, 8, test);
            test = new QTableWidgetItem(QString::number(it->hour));
            test->setBackgroundColor(computeColor(it->hour, it->hour_objective));
            m_ui->WeekWidget->setItem(count, 9, test);
            m_ui->WeekWidget->setItem(count, 10, new QTableWidgetItem(QString::number(it->hour_objective)));
            m_ui->WeekWidget->setItem(count, 11, new QTableWidgetItem(it->muscu));
            m_ui->WeekWidget->setItem(count, 12, new QTableWidgetItem(it->muscu_objective));
            count++;
            if (it->date == today) {
                m_ui->label_12->setText(it->training);
                m_ui->label_30->setText(it->daily_objective);

                m_ui->label_13->setText(it->muscu_objective);
                m_ui->label_32->setText(it->muscu);

                m_ui->label_17->setText(QString::number(it->hour));
                m_ui->label_14->setText(QString::number(it->hour_objective));

                m_ui->label_18->setText(QString::number(it->Km_per_day));
                m_ui->label_15->setText(QString::number(it->km_per_week_objective));

                m_ui->label_19->setText(QString::number(it->TSS));
                m_ui->label_16->setText(QString::number(it->TSS_objective));

                m_ui->label_5->setText(it->weather);

                if (it->hour_objective > 0)
                    m_ui->progressBar->setValue(100*it->hour/it->hour_objective);
                else
                     m_ui->progressBar->setValue(100);

                if (it->km_per_week_objective > 0)
                    m_ui->progressBar_2->setValue(100*it->Km_per_day/it->km_per_week_objective);
                else
                    m_ui->progressBar_2->setValue(100);

                if (it->TSS_objective > 0)
                    m_ui->progressBar_3->setValue(100*it->TSS/it->TSS_objective);
                else
                    m_ui->progressBar_3->setValue(100);
            }
        }
    }

    m_ui->progressBar_6->setValue(100*sum_km_done/sum_km_objective);
    m_ui->label_36->setText(QString::number(sum_km_done));
    m_ui->label_38->setText(QString::number(sum_km_objective));

    m_ui->label_39->setText(QString::number(sum_hour_done));// hour done
    m_ui->label_41->setText(QString::number(sum_hour_objective)); // hour todo
    m_ui->progressBar_5->setValue(100*sum_hour_done/sum_hour_objective);
    m_ui->label_42->setText(QString::number(sum_tss_done)); // TSS done
    m_ui->label_44->setText(QString::number(sum_tss_objective)); // TSS todo
    m_ui->progressBar_4->setValue(100*sum_tss_done/sum_tss_objective);
}

void ThemeWidget::updateUI()
{
    updateWeekSummary();
    updateCalendar();
    updateMyWeek();
}

void ThemeWidget::saveTrainingPlan()
{
    std::cout<<"Add item in training plans"<<std::endl;

    std::vector<TrainingItem>::iterator dayfound = mTrainings.end();
    std::vector<TrainingItem>::iterator inserthere = mTrainings.end();

    // Search for an other training on the same day
    for (auto it = mTrainings.begin(); it!= mTrainings.end(); it++) {
        if (it->date == m_ui->dateEdit->date()) {
            // TODO: ask if we want to override
            dayfound = it;
            break;
        }
    }
    if (dayfound == mTrainings.end()) {
        for (auto it = mTrainings.begin(); it!= mTrainings.end() && it->date.daysTo(m_ui->dateEdit->date()) > 0; it++)
            inserthere = it+1;
        TrainingItem tmp = blankDay();
        dayfound = mTrainings.insert(inserthere, tmp);
    }

    std::cout<<"Setting new day"<<std::endl;
    dayfound->date = m_ui->dateEdit->date();
    dayfound->weather = m_ui->comboBox->currentText();
    dayfound->daily_objective = m_ui->textEdit->toPlainText();
    dayfound->TSS_objective = m_ui->spinBox->value();
    dayfound->km_per_week_objective = m_ui->spinBox_2->value();
    dayfound->hour_objective = m_ui->doubleSpinBox->value();

    orderVector();
}

void ThemeWidget::orderVector()
{
   int i=0;
    while(i>=0 && i<mTrainings.size()-1) {
        if (mTrainings[i].date.daysTo(mTrainings[i+1].date) < 0) {
            std::cout<<"Training order error: "<<mTrainings[i].date.toString().toStdString()<<" is after "<<mTrainings[i+1].date.toString().toStdString()<<std::endl;
            TrainingItem tmp = mTrainings[i+1];
            mTrainings[i+1] = mTrainings[i];
            mTrainings[i] = tmp;
            i-=2;
            if (i<0)
                i=0;
        } else {
            i++;
        }
    }
    saveToFile();
    updateUI();
}

void ThemeWidget::saveWorkout()
{
    std::cout<<"Activity saved into training plans"<<std::endl;

    std::vector<TrainingItem>::iterator dayfound = mTrainings.end();
    std::vector<TrainingItem>::iterator inserthere = mTrainings.end();

    // Search for an other training on the same day
    for (auto it = mTrainings.begin(); it!= mTrainings.end(); it++) {
        if (it->date == m_ui->dateEdit_2->date()) {
            dayfound = it;
            break;
        }
    }
    if (dayfound == mTrainings.end()) {
        for (auto it = mTrainings.begin(); it!= mTrainings.end() && it->date.daysTo(m_ui->dateEdit_2->date()) > 0; it++)
            inserthere = it+1;
        TrainingItem tmp = blankDay();
        dayfound = mTrainings.insert(inserthere, tmp);
    }

    std::cout<<"Add training"<<std::endl;
    dayfound->date = m_ui->dateEdit_2->date(); // make sure of the date

    dayfound->weather = m_ui->WeatherCombo->currentText();

    dayfound->feeling = m_ui->FeelingSpinBox->value(); // Overwrite feeling

    if (m_ui->checkBox->isChecked()) {
        dayfound->training = m_ui->CommentLineEdit->text();
        dayfound->TSS = m_ui->TSSSpinBox->value();
        dayfound->Km_per_day = m_ui->KmDoubleSpinBox->value();
        dayfound->hour = m_ui->DurationDoubleSpinBox->value();
        dayfound->muscu = m_ui->MuscuLineEdit->text();
    } else {
        if(dayfound->training.size() > 0)
            dayfound->training.append("; ");

        dayfound->training.append(m_ui->CommentLineEdit->text());

        dayfound->TSS += m_ui->TSSSpinBox->value();

        dayfound->Km_per_day += m_ui->KmDoubleSpinBox->value();

        dayfound->hour += m_ui->DurationDoubleSpinBox->value();

        if(dayfound->muscu.size() > 0)
            dayfound->muscu.append("; ");
        dayfound->muscu.append(m_ui->MuscuLineEdit->text());
    }
    orderVector();
}

void ThemeWidget::saveToFile()
{
    saveTrainingsToFile("test_training.csv", mTrainings);
}

void ThemeWidget::updateWeekSummary()
{
    mWeeks.clear();
    TrainingWeek tmp = blankWeek();
    for (auto it = mTrainings.begin(); it!= mTrainings.end(); it++) {
        if (tmp.year == it->date.year() && tmp.week_number == it->date.weekNumber()) {
            tmp.sum_hour += it->hour;
            tmp.sum_tss += it->TSS;
            tmp.sum_km += it->Km_per_day;
            tmp.sum_hour_objective += it->hour_objective;
            tmp.sum_tss_objective += it->TSS_objective;
            tmp.sum_km_objective += it->km_per_week_objective;
            if (!it->category.isEmpty())
                tmp.category = it->category;
        } else {
            if (tmp.sum_hour != 0 || tmp.sum_km != 0 || tmp.sum_tss != 0)
                mWeeks.push_back(tmp);
            tmp = blankWeek();
            tmp.week_number = it->date.weekNumber();
            tmp.year = it->date.year();
            tmp.month = it->date.month();
            tmp.sum_hour += it->hour;
            tmp.sum_tss += it->TSS;
            tmp.sum_km += it->Km_per_day;
            tmp.sum_hour_objective += it->hour_objective;
            tmp.sum_tss_objective += it->TSS_objective;
            tmp.sum_km_objective += it->km_per_week_objective;
            if (!it->category.isEmpty())
                tmp.category = it->category;
        }
    }
}

void ThemeWidget::updateFatigue() {
    mFatigue.clear();
    if (mTrainings.size() <= sizeof(fatigue_coef)/sizeof(double)) {
        std::cout<<"Too few data to compute Fatigue ("<<sizeof(fatigue_coef)/sizeof(double)<<" required, "<<mTrainings.size()<<" available)"<<std::endl;
        return;
    }
    size_t count = 0;
    QDate current_date;
    double current_fatigue;
    for (auto it = mTrainings.begin(); it!= mTrainings.end(); it++) {
        if (count > sizeof(fatigue_coef)/sizeof(double)) {
            current_date = it->date;
            current_fatigue = 0;
            std::vector<TrainingItem>::iterator it2 = it-sizeof(fatigue_coef)/sizeof(double);
            for(int i =sizeof(fatigue_coef)/sizeof(double)-1; i>=0; i--) {
                current_fatigue += fatigue_coef[i]*it2->TSS;
                it2++;
            }
            mFatigue.push_back(std::pair<QDate, double>(current_date,current_fatigue));
            std::cout<<"Fatigue for "<<current_date.toString().toStdString()<<": "<<current_fatigue<<std::endl;
        }
        count++;
    }
}

void ThemeWidget::updateFitness() {

}

void ThemeWidget::updateForm() {

}
