#include <QFile>
#include <QTextStream>
#include <string>
#include <QTextEdit>
#include <QtWidgets/QApplication>
#include <QGraphicsScene>
#include <QtWidgets/QMainWindow>
#include <QValueAxis>
#include <math.h>

#include <QtCharts/QLineSeries>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QChart>
#include <QVector>

#include <iostream>

#include "textfinder.h"
#include "ui_textfinder.h"
#include "force.h"

QT_CHARTS_USE_NAMESPACE
using namespace std;

TextFinder::TextFinder(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TextFinder)
{
    ui->setupUi(this);
    setdefaultvalues();
    chart = new QChart();

    ui->lineEdit_barLength->setValidator( new QIntValidator(-Q_INFINITY, Q_INFINITY, this) ); // Filtra todas las entradas a solo double
    ui->lineEdit_Xa->setValidator( new QIntValidator(-Q_INFINITY, Q_INFINITY, this) ); // Filtra todas las entradas a solo double
    ui->lineEdit_Xb->setValidator( new QIntValidator(-Q_INFINITY, Q_INFINITY, this) ); // Filtra todas las entradas a solo double

    ui->lineEdit_Fx->setValidator( new QDoubleValidator(-Q_INFINITY, Q_INFINITY, 1, this) ); // Filtra todas las entradas a solo double
    ui->lineEdit_Fy->setValidator( new QDoubleValidator(-Q_INFINITY, Q_INFINITY, 1, this) ); // Filtra todas las entradas a solo double
    ui->lineEdit_Fz->setValidator( new QDoubleValidator(-Q_INFINITY, Q_INFINITY, 1, this) ); // Filtra todas las entradas a solo double
    ui->lineEdit_posX->setValidator( new QIntValidator(0, this->lengthBar, this) ); // Filtra todas las entradas a solo double

    //Poner el color de fondo "invisible" y de solo lectura
    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->setFrameStyle(QFrame::NoFrame);
    QPalette palette;//Poner el color de fondo "invisible" y de solo lectura
    color = QColor(239,239,239);

    palette.setColor(QPalette::Base,color);
    ui->plainTextEdit->setPalette(palette);

    forceList = QVector<force>(0);
    forceRes = forceList;
    series = new QLineSeries();


    this->scene = new QGraphicsScene;

    this->Bstyle = Qt::SolidPattern;
    this->scene->setBackgroundBrush(QBrush(color, Bstyle));

    this->ui->graphicsView->setMinimumSize(QSize(500, 50)); //(x-waagerecht , y-senkrecht)
    this->ui->graphicsView->setMaximumSize(QSize(1900, 5000));

    this->ui->graphicsView->setScene(scene);
    pen = QPen();  // creates a default pen
    pen.setBrush(Qt::gray);
    pen.setWidth(5);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::MiterJoin);

    this->ui->graphicsView_chart->setVisible(false);

}
void TextFinder::setdefaultvalues(){
    this->lengthBar    = 1000;  //Value in mm
    this->ui->lineEdit_barLength->setText(QString::number(this->lengthBar));

    this->distanceXa   = 300;
    this->ui->lineEdit_Xa->setText(QString::number(this->distanceXa));

    this->distanceXb   = 700;
    this->ui->lineEdit_Xb->setText(QString::number(this->distanceXb));

    this->ui->lineEdit_Fx->setText(QString::number(10));
    this->ui->lineEdit_Fy->setText(QString::number(10));
    this->ui->lineEdit_Fz->setText(QString::number(10));
    this->ui->lineEdit_posX->setText(QString::number(500));

    this->ForceCounter = 0;
    this->maxForce=0;
    this->F_Festlager=0;
    this->F_Loslager=0;
    momentsOrForces =true;

    offset_y     = 60;
    offset_x     = 7;

}
TextFinder::~TextFinder() // destructor
{
    delete ui;
}
void TextFinder::paintTheBeam()
{
    scene = new QGraphicsScene;


    int lenght_x0line =55;  //Length of Line

    this->scene->setBackgroundBrush(QBrush(color, Bstyle));
    scene->addText("\n\n\n\n\n\n    x");
    if(this->F_Festlager!=0 || this->F_Loslager!=0)scene->addText("Fa="+QString::number(this->F_Festlager,'f',1)+" [N], Fb= "+QString::number(this->F_Loslager,'f',1)+" [N]");
    PaintAFestlager(offset_x+(this->distanceXa*beamlength/this->lengthBar),offset_y+pen.width()-1,15);
    PaintALoslager(offset_x+((this->distanceXb*beamlength) / (this->lengthBar)),offset_y+pen.width()-1,15);
    scene->addLine(offset_x,offset_y,offset_x+beamlength,offset_y,pen);                                      //THE BEAM

    scene->addLine(offset_x,offset_y+pen.width(),offset_x,offset_y+lenght_x0line);                               //Little Arrow X=0 Line
    scene->addLine(offset_x,offset_y+lenght_x0line,offset_x+10,offset_y+lenght_x0line);                          //Little Arrow
    scene->addLine(offset_x+10,offset_y+lenght_x0line,offset_x+7,offset_y+lenght_x0line-3);                      //Little Arrow
    scene->addLine(offset_x+10,offset_y+lenght_x0line,offset_x+7,offset_y+lenght_x0line+3);                      //Little Arrow

    if(this->forceList.size()>0){                           //Esto Pinta las fuerzas en la barra(solo las verticales)
            for(int i=0;i<this->forceList.size();i++){
                if(forceList[i].getForce_y()!=0){
                    if(forceList[i].getForce_y()>this->maxForce)    this->maxForce=forceList[i].getForce_y();
                     //scene->addText("\nThis MaxForce: "+QString::number(this->maxForce));
                    int comodin=offset_x+(qRound(forceList[i].getPos_x())*beamlength)/this->lengthBar;
                    PaintArrow(comodin, offset_y, 10, qRound(forceList[i].getForce_y()), Qt::black );
                }
           }
    }
    paintLagerForces();
//    scene->addText("\n"+QString::number(this->forceList.size()));
//    scene->addLine(offset_x+beamlength,0,offset_x+beamlength,100+1);
    this->ui->graphicsView->setScene(scene);
}
void TextFinder::paintLagerForces(){
    if(this->F_Festlager!=0)    PaintArrow(offset_x+(this->distanceXa*beamlength/this->lengthBar),offset_y,10,this->F_Festlager,Qt::red);
    if(this->F_Loslager!=0)     PaintArrow(offset_x+(this->distanceXb*beamlength/this->lengthBar),offset_y,10,this->F_Loslager,Qt::red);
}

void TextFinder::PaintAFestlager(int x, int y, int ts){//ts is trianglesize. the upper point of the triangle is at x,y
    QPolygonF polygon;
    polygon << QPointF(x, y) << QPointF(x-ts, y+ts)<< QPointF(x+ts, y+ts);
    scene->addPolygon(polygon,QPen(Qt::black,3,Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin) ,Qt::black );
    scene->addLine(x-2*ts,y+ts,x+2*ts,y+ts);
    for(int i=-3;i<5;i++)scene->addLine(x+i*ts/2,y+ts,x+(i-1)*ts/2,y+3*ts/2);
}
void TextFinder::PaintALoslager(int x, int y, int ts){//ts is trianglesize. the upper point of the triangle is at x,y
    QPolygonF polygon;
    polygon << QPointF(x, y) << QPointF(x-3*ts/4, y+3*ts/4)<< QPointF(x+3*ts/4, y+3*ts/4);
    scene->addPolygon(polygon,QPen(Qt::black,2.5,Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin) ,Qt::black );
    scene->addLine(x-3*ts/2,y+3*ts/4,x+3*ts/2,y+3*ts/4);
    scene->addLine(x-2*ts,y+ts,x+2*ts,y+ts);
    for(int i=-3;i<5;i++)scene->addLine(x+i*ts/2,y+ts,x+(i-1)*ts/2,y+3*ts/2,QPen(Qt::black,0.5,Qt::SolidLine,Qt::SquareCap, Qt::MiterJoin) );
}
void TextFinder::PaintArrow(int x, int y, int ts, double dir, QBrush bru){//ts = thisSize, dir = direction true goes down false up
    QPolygonF polygon;
    int i=1;
    if(dir>0)i=-1;
    polygon <<QPointF(x, y)
            <<QPointF(x-i*3*ts/4, y+i*3*ts/4)
            <<QPointF(x-i*2, y+i*3*ts/4)
            <<QPointF(x-i*2, y+i*3*ts)
            <<QPointF(x+i*2, y+i*3*ts)
            <<QPointF(x+i*2, y+i*3*ts/4)
            <<QPointF(x+i*3*ts/4, y+i*3*ts/4);
    scene->addPolygon(polygon,QPen(bru,2,Qt::SolidLine,Qt::SquareCap, Qt::MiterJoin) ,bru );

}

void TextFinder::paintTheChart(){
/*  chart = new QChart();
    series = new QLineSeries(chart); 
    double max_y=-Q_INFINITY,min_y=Q_INFINITY;
    QValueAxis *axisY = new QValueAxis(chart);


    if(this->momentsOrForces==true){
        for(int i=0;i<this->forceRes.size();i++){
            if( max_y < forceRes[i].getMoment_z() ) max_y=forceRes[i].getMoment_z();
            if( min_y > forceRes[i].getMoment_z() ) min_y=forceRes[i].getMoment_z();
        }

        //axisY->setMinorTickCount(pow(10,qRound(std::log10(max_y-min_y))));
        chart->legend()->setVisible(true);
        for(int i=0;i<this->forceRes.size();i++){
            *series << QPointF(forceRes[i].getPos_x(), forceRes[i].getMoment_z());
        }
        series->attachAxis(axisY);
        chart->addSeries(series);
        //chart->createDefaultAxes();
        chart->addAxis(axisY, Qt::AlignLeft);
        axisY->setRange(min_y, 2*max_y);
        axisY->setTitleText("Moment in [Nm]");
    }else{
        axisY->setTitleText("Force in [N]");

        for(int i=0;i<this->forceRes.size();i++){
            if( max_y < forceRes[i].getForce_y() ) max_y=forceRes[i].getForce_y();
            if( min_y > forceRes[i].getForce_y() ) min_y=forceRes[i].getForce_y();
        }
        axisY->setRange(min_y, max_y);

        for(int i=0;i<this->forceRes.size();i++){
            *series << QPointF(forceRes[i].getPos_x(), forceRes[i].getForce_y());
        }
        chart->addSeries(series);
        chart->addAxis(axisY, Qt::AlignLeft);

    }


    cout<<"MIN: "<<min_y<<"  MAX: "<<max_y<<"log10(max_y-min_y):= "<<std::log10(max_y-min_y)<<",   QRound: "<<qRound(std::log10(max_y-min_y))<<endl;

    QRect plotArea = QRect(0,0,beamlength/2,165);
    chart->setPlotArea(plotArea);
    chart->setMargins(QMargins(beamlength/8,0,beamlength/8,20));
*/
    chart = new QChart();
    chart->legend()->setVisible(false);
    series = new QLineSeries(chart);
    QValueAxis *axisY = new QValueAxis(chart);
    double max_y=-Q_INFINITY,min_y=Q_INFINITY;



    if(this->momentsOrForces==true){
        for(int i=0;i<this->forceRes.size();i++){
            if( max_y < forceRes[i].getMoment_z() ) max_y=forceRes[i].getMoment_z();
            if( min_y > forceRes[i].getMoment_z() ) min_y=forceRes[i].getMoment_z();
        }

        if(max_y-min_y>10000){
                 max_y/=1000;        //because we want [N*m] not [N*mm]
                 min_y/=1000;
                 axisY->setTitleText("Moment in [Nm]");
        }else{
                axisY->setTitleText("Moment in [Nmm]");
        }

        for(int i=0;i<this->forceRes.size();i++){
            *series << QPointF(forceRes[i].getPos_x(), forceRes[i].getMoment_z());
        }
    }else{
        for(int i=0;i<this->forceRes.size();i++){
            if( max_y < forceRes[i].getForce_y() ) max_y=forceRes[i].getForce_y();
            if( min_y > forceRes[i].getForce_y() ) min_y=forceRes[i].getForce_y();
        }
        axisY->setTitleText("Force in [N]");
        for(int i=0;i<this->forceRes.size();i++){
            *series << QPointF(forceRes[i].getPos_x(), forceRes[i].getForce_y());
        }
    }
     chart->addSeries(series);
 //    chart->createDefaultAxes();
     axisY->setRange(min_y, max_y);
     axisY->setTickCount(5);
     axisY->setLabelFormat("%d");

     chart->addAxis(axisY, Qt::AlignLeft);

    this->ui->graphicsView_chart->setChart(chart);
    if(forceRes.size()>0)  this->ui->graphicsView_chart->setVisible(true);
}

void TextFinder::on_pushButton_2_clicked()// Con Pulsar el boton Set, se ponen los valores de la barra
{

    QString stringLengthBar = ui->lineEdit_barLength->text();
    QString stringDistanceXa = ui->lineEdit_Xa->text();
    QString stringDistanceXb = ui->lineEdit_Xb->text();
    cout<<"Aqui habria que mirar que no se borre el setting por una entrada no valida, Igual dar la opcion de leer de un archivo"<<endl;

    this->lengthBar = stringLengthBar.toInt();
    this->distanceXa = stringDistanceXa.toInt();
    this->distanceXb = stringDistanceXb.toInt();

    paintTheBeam();
    cout<<"Vacia el vector de fuerzas si una de ellas se ve afectada"<<endl;
}

void TextFinder::on_pushButton_clicked() //Cada vez que se pulsa se aniade una fuerza al conjunto de fuerzas.
{
      QString stringForce_x = ui->lineEdit_Fx->text();
      QString stringForce_y = ui->lineEdit_Fy->text();
      QString stringForce_z = ui->lineEdit_Fz->text();
      QString stringPos_x =   ui->lineEdit_posX->text();

      if((stringForce_x.toDouble()!=0 || stringForce_y.toDouble()!=0 || stringForce_z.toDouble()!=0 ) && stringPos_x.toDouble()<=lengthBar){

              ForceCounter++;
              force myforce(stringForce_x.toDouble(),stringForce_y.toDouble(),stringForce_z.toDouble(),0,0,0,stringPos_x.toDouble());
              forceList.push_back(myforce);
              ui->plainTextEdit-> appendPlainText(toString_lastforceList());
              calculateLagerkraefte();
              calculateTensionFlow();
              paintTheBeam();
              paintTheChart();

      }else{
              ui->plainTextEdit-> appendPlainText("Force not addmited (equals zero or out of bounds)");
      }
}

QString TextFinder::toString_lastforceList(){ //Devuelve un String con la descripcionde la ultima fuerza del vector
    return "Force #"+ QString::number(forceList.length()) +"  with :\n"+forceList.last().toString();
}

void TextFinder::resizeEvent(QResizeEvent *event){
            // FILTRO para que solo repinte todo si hay un cambio grande de tamanio
    if((event->size().width()-event->oldSize().width())>3 ||(event->size().width()-event->oldSize().width())<-3){
        //cout<<"Event is "<<event->size().width()<<" (old "<<event->oldSize().width()<<"),";
        //cout<<" Size of GV is: "<<this->ui->graphicsView->size().width()<<endl;
        this->ui->graphicsView->resize(event->size().width()-1,this->ui->graphicsView->size().height());
        this->ui->graphicsView_chart->resize(event->size().width()-1,this->ui->graphicsView_chart->size().height());

        beamlength   = 4 * (this->ui->graphicsView->size().width()/5); //Length of beam in pixels. Why this length u ask? Because it looks nice

        paintTheBeam();
        paintTheChart();
    }
}


void TextFinder::calculateLagerkraefte(){   //Calculates Lagerkraefte by setting Moment on the right lager to 0
    double FMLoslager=0, simpleSum=0;
    for(int i=0;i<forceList.length();i++){
        FMLoslager+=forceList[i].getForce_y()*(this->distanceXb-forceList[i].getPos_x());
        simpleSum+=forceList[i].getForce_y();
    }
    this->F_Festlager=(-1*FMLoslager/(this->distanceXb-this->distanceXa));
    this->F_Loslager=-1*(simpleSum+this->F_Festlager);
//    cout << "F_Festlager= "<<this->F_Loslager<<" N"<<endl;
//    cout << "F_Loslager = "<<this->F_Loslager<<" N"<<endl;
}
bool TextFinder::isclosertoX( force A, force B){ // returns true if A is closer to X
    if(A.getPos_x()<B.getPos_x()){

        return true;

    }else{

        return false;

    }
}
void TextFinder::calculateTensionFlow(){//prepares vectors so stuff can be added easyly



    int totalPoints = 100;
    int stepSize = this->lengthBar/totalPoints;
    QVector <force> vec = this->forceList;                      //the forces applied to the bar
    vec.push_back( force(0,this->F_Festlager,0,double(this->distanceXa)) );
    vec.push_back(force(0,this->F_Loslager,0,double(this->distanceXb ) ) );

    QVector<force>res;                                          //the resulting stress forces
    sort( vec.begin(),vec.end(), isclosertoX );

     for(int i=0;i<vec.length();i++){           //Adds multiple forces at the same spot together
        if((i+1)<vec.length() && vec[i].getPos_x()==vec[i+1].getPos_x()){
            vec[i].addForces(vec[i+1]);
            vec.remove(i+1);
            i--;
        }
    }
   cout<<"\nVEC: "<<endl;
   for(int i=0;i<vec.length();i++) cout<<"["<<vec[i].getPos_x()<<"] ";
   int u=0;
   for(double i=0;i<=double(lengthBar);i+=stepSize){
       res.push_back(force(0,0,0,i));
       if( vec[u].getPos_x()<(i+stepSize+0.01)){
           if( (vec[u].getPos_x())!=(i+double(stepSize)) && u<vec.length()){
               res.push_back(force(0,0,0,vec[u].getPos_x()));
              // cout<<"\nHey se ha sumado ["<<vec[u].getPos_x()<<"] porque ";
              // cout<<vec[u].getPos_x()<<" < "<<i+stepSize<<" &&";
              // cout<<"vec[u].getPos_x()= "<<vec[u].getPos_x()<<"!="<<i<<" =i "<<" && u="<<u<<"<"<<vec.length()<<"=vec.length"<<endl;
           }
           u++;
       }
   }
   cout<<"\nPoints x_pos ahora: "<<endl;
   for(int i=0;i<res.length();i++) cout<<"["<<res[i].getPos_x()<<"] ";
   cout<<endl;

   if(vec[0].getPos_x()==res[0].getPos_x()){
       res[0]=vec[0];
       vec.removeFirst();
   }
   for(int i=1;i<res.length();i++){
       if(vec.size()>0 && vec[0].getPos_x()==res[i].getPos_x()){
           res[i]=calculatePoints(res[i-1],vec[0]);
           vec.removeFirst();
       }else{
           res[i]=calculatePoints(res[i-1],res[i]);
       }
   }
   this->forceRes=res;

}

force TextFinder::calculatePoints(force A, force B1){
    return force(A.getForce_x()+B1.getForce_x(),A.getForce_y()+B1.getForce_y(),0,0,0,A.getMoment_z()+B1.getMoment_z()-A.getForce_y()*(B1.getPos_x()-A.getPos_x()),B1.getPos_x());
}


void TextFinder::on_radioButton_Mz_pressed()
{
    momentsOrForces=true;
    paintTheChart();

}

void TextFinder::on_radioButton_Fy_pressed()
{
    momentsOrForces=false;
    paintTheChart();

}
