#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QSlider>
#include <QCheckBox>

struct P3Event {
    QString text;
    QString values;
    QCheckBox *CB;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void LoadSaveFile();

private slots:
    void on_SaveFilePathButton_clicked();

    void on_ApatItemsMultiplierHSlider_valueChanged(int value);

    void on_RevolverBulletsHSlider_valueChanged(int value);

    void on_SprayAmmoCountHSlider_valueChanged(int value);

    void on_ShabnakCheckBox_stateChanged(int arg1);

    void on_ApplyButton_clicked();

    void on_InfoButton_clicked();

    void on_AutoFindButton_clicked();

    void on_LeapOfFaith_clicked();

private:
    Ui::MainWindow *ui;

    QList <P3Event> ListP3Events;   // События. их много и они будут пополнятся
    QCheckBox *Shabnak_Dead_CheckBox[9]; // Дни... Её...

    QDir *currdir;       // Временная директория
    QString SaveFilePath;
    QFile *currfile;    // Файл текущего дня
    QStringList lines; // список всех параметров

    void CheckPlayerValues(QSlider *HS, QString text);
    void CheckValues(QCheckBox *CH, QString text);

    void SetPlayerValues(QString text, QString val);
    void SetValues(QString text, bool val);

};
#endif // MAINWINDOW_H
