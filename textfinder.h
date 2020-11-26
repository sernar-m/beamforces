#ifndef TEXTFINDER_H
#define TEXTFINDER_H

#include <QWidget>
#include <QVector>
#include <QChart>
#include <QtCharts/QLineSeries>
#include <force.h>
//QT_BEGIN_NAMESPACE
using namespace QtCharts;
namespace Ui { class TextFinder; }
//QT_END_NAMESPACE

class TextFinder : public QWidget
{
    Q_OBJECT

public:
    TextFinder(QWidget *parent = nullptr);
    ~TextFinder();
private slots:

    void on_pushButton_clicked();//addforce

    void on_pushButton_2_clicked();//set bar dimensions
    void resizeEvent(QResizeEvent *event);// public :resizeEvent


    void on_radioButton_Mz_pressed();
    void on_radioButton_Fy_pressed();

private:
    Ui::TextFinder *ui;
    int ForceCounter;   // Counts number of Forces
    int maxForce;       // Greatest Force having a
    QGraphicsScene *scene;
    int lengthBar;
    int distanceXa;
    int distanceXb;
    QPen pen;
    QColor color;
    Qt::BrushStyle Bstyle;
    void paintTheBeam();
    void paintTheChart();
    void PaintAFestlager(int x , int y, int trianglesize);
    void PaintALoslager(int x , int y, int trianglesize);
    void PaintArrow(int x , int y, int thickness, double direction , QBrush bru);
    void paintLagerForces();
    void setdefaultvalues();
    void calculateLagerkraefte();
    void calculateTensionFlow();
    force calculatePoints(force A, force B1);
    static bool isclosertoX(force A, force B); // returns true if A is closer to X
    QVector <force> forceList;
    QVector <force> forceRes;
    QString toString_lastforceList();
    QChart *chart;
    QLineSeries *series;

    bool momentsOrForces;

    int beamlength;
    int offset_y;
    int offset_x;

    double F_Festlager;
    double F_Loslager;
};
#endif // TEXTFINDER_H
