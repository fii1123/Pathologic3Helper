#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QStandardPaths>
#include <QMessageBox>
#include <QFileDialog>

void LP3E_Fill(Ui::MainWindow *mw, QList<P3Event> *QL, QString text, QString values)
{
    QL->append({text, values, new QCheckBox(mw->centralwidget)});
    QL->last().CB->setText(text);
    mw->EventVLayout->addWidget(QL->last().CB);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Стиль и отрисовка
    setWindowFlags(windowFlags() & ~Qt::WindowMinimizeButtonHint
                   & ~Qt::WindowMaximizeButtonHint);

    ui->setupUi(this);

    this->setStyleSheet(
        "QWidget {"
        "    background-color: rgb(0, 0, 0);"
        "    color: white;"
        "    font: 16pt 'Bad Script';"
        "}"

        "QPushButton {"
        "    color: white;"
        "    border: 1px solid gray;"
        "    border-radius: 3px;"
        "    padding: 0px 8px;"
        "    background-color: rgb(0, 0, 0);"
        "}"

        "QPushButton:hover {"
        "    color: red;"
        "}"
    );

    // Генерация
    int var = 0;

    // События
    LP3E_Fill(ui, &ListP3Events, "Улица в Створках", "<Name>Know_BachelorWalkedDownTheStreet</Name>");
    LP3E_Fill(ui, &ListP3Events, "Секрет Симона", "<Name>Know_Main_YouAreSimon</Name>");
    LP3E_Fill(ui, &ListP3Events, "Секрет Многогранника", "<Name>Know_Main_RodOfPolyhedron</Name>");

    QString qstr;
    for (var = 0; var < 9; ++var) {
        qstr = "Shabnak_Dead_Day" + QString::number(var + 3) + "CheckBox";
        Shabnak_Dead_CheckBox[var] = new QCheckBox(ui->centralwidget);
        Shabnak_Dead_CheckBox[var]->setObjectName(qstr);
        qstr = "\320\224\320\265\320\275\321\214 " + QString::number(var + 3);
        Shabnak_Dead_CheckBox[var]->setText(QApplication::translate("MainWindow",
        qstr.toStdString().c_str(), nullptr));
        ui->verticalLayout_3->addWidget(Shabnak_Dead_CheckBox[var]);
    }

    // Создание директории
    currdir = new QDir();
    currdir->mkdir("tmp");
}

MainWindow::~MainWindow()
{
    system("powershell DEL tmp\\*");
    this->currdir->rmdir("tmp");
    delete ui;
}

void MainWindow::CheckPlayerValues(QTextStream *in, QString line, QSlider *HS, QString text)
{
    float var_f = 0;
    if (line == "\t\t\t\t\t\t" + text) {
        line = in->readLine(); lines << line;
        line = in->readLine(); lines << line;
        line.remove(0, line.indexOf('>') + 1);
        line.remove(line.indexOf('<'), line.indexOf('>'));
        var_f = line.toFloat();
        if (var_f < 1) {
            var_f *= 100;
        }
        HS->setValue((int)var_f);
    }
}

void MainWindow::CheckValues(QTextStream *in, QString line, QCheckBox *CH, QString text)
{
    if (line == "\t\t\t\t\t\t\t\t" + text) {
        line = in->readLine(); lines << line;
        CH->setChecked(line == "\t\t\t\t\t\t\t\t<Value>true</Value>");
    }
}

void MainWindow::SetTrueValues(QString text)
{
    int inx;
    inx = lines.indexOf("\t\t\t\t\t\t\t\t" + text);
    if (lines[inx + 1] != "\t\t\t\t\t\t\t\t<Value>true</Value>")
    {
        lines.insert(inx + 1, "\t\t\t\t\t\t\t\t<Value>true</Value>");
    }
}

void MainWindow::SetFalseValues(QString text)
{
    int inx;
    inx = lines.indexOf("\t\t\t\t\t\t\t\t" + text);
    if (lines[inx + 1] == "\t\t\t\t\t\t\t\t<Value>true</Value>")
    {
        lines.removeAt(inx + 1);
    };
}

void MainWindow::LoadSaveFile()
{
    // Извлекчение файлов из архива
    QString strcmd = "powershell Expand-Archive -Path '" + SaveFilePath +
            "' -DestinationPath '" + currdir->path() + "/tmp' -Force";

    if (!currdir->isEmpty()) {
        system("powershell DEL tmp\\*");
    }
    system(strcmd.toStdString().c_str());

    // Работа с файлом Current
    currfile = new QFile("tmp\\Current.xml");

    currfile->open(QIODevice::ReadOnly);

        if (!currfile->isOpen()) {
            QMessageBox::critical(this, "Ошибка файла", "Не удалось открыть файл");
            exit(1);
        }
        else {
            int var_int = 0;
            float var_f = 0.0;
            QString var_qstr;
            bool SheIsNotExist = 0;

            // Запись всех параметров
            QTextStream in(currfile);
            QString line = "";

            // Перемотка для оптимизации
            for (int i = 0; i < 40000; ++i) {
                line = in.readLine();
                lines << line;
            }

            int var = 0;
            while (!in.atEnd()) {
                line = in.readLine();
                lines << line;

                // Параметры персонажа
                CheckPlayerValues(&in, line, ui->ApatItemsMultiplierHSlider, "<Key>ApatItemsMultiplier</Key>");
                CheckPlayerValues(&in, line, ui->RevolverBulletsHSlider, "<Key>RevolverBullets</Key>");
                CheckPlayerValues(&in, line, ui->SprayAmmoCountHSlider, "<Key>SprayAmmoCount</Key>");

                // События
                for (var = 0; var < ListP3Events.length(); ++var) {
                    CheckValues(&in, line, ListP3Events.at(var).CB, ListP3Events.at(var).values);
                }

                CheckValues(&in, line, ui->ShabnakCheckBox, "<Name>Shabnak_Enable</Name>");

                // Её не существует
                for (var = 0; var < 9; ++var) {
                    CheckValues(&in, line, Shabnak_Dead_CheckBox[var],
                    "<Name>Shabnak_Dead_Day" + QString::number(var + 3) + "</Name>");
                }
            }

        }
        currfile->close();
}

void MainWindow::on_SaveFilePathButton_clicked()
{
    SaveFilePath = QStandardPaths::writableLocation
            (QStandardPaths::AppConfigLocation);
    SaveFilePath.remove(SaveFilePath.lastIndexOf('/'), 18);
    SaveFilePath += "Low/Ice-Pick Lodge/Pathologic 3/Saves/Default/";

    SaveFilePath = QFileDialog::getOpenFileName(this,
       "Open save file", SaveFilePath, "Files (*.zip)");
    if (SaveFilePath != "") {
        ui->SaveFileLineEdit->setText(SaveFilePath);
        LoadSaveFile();
    }
}

void MainWindow::on_ApatItemsMultiplierHSlider_valueChanged(int value)
{
    if (value < 84 || value > 84) {
        ui->ApatItemsMultiplierLabel_2->setStyleSheet("color: red");
    } else {
        ui->ApatItemsMultiplierLabel_2->setStyleSheet("color: white");
    }
    ui->ApatItemsMultiplierLabel_2->setText(QString::number((float)value / 100));
}

void MainWindow::on_RevolverBulletsHSlider_valueChanged(int value)
{
    ui->RevolverBulletsLabel_2->setText(QString::number(value));
}

void MainWindow::on_SprayAmmoCountHSlider_valueChanged(int value)
{
    ui->SprayAmmoCountLabel_2->setText(QString::number(value));
}

void MainWindow::on_ShabnakCheckBox_stateChanged(int arg1)
{
    if (arg1 == 0) {
        ui->ShabnakCheckBox->setText("Её не существует");
    } else {
        ui->ShabnakCheckBox->setText("Она ждет...");
    }

    for (int var = 0; var < 9; ++var) {
        Shabnak_Dead_CheckBox[var]->setChecked(arg1);
    }

}

void MainWindow::SetValues(QCheckBox *CB, QString text)
{
    if (CB->isChecked()) {
        SetTrueValues(text);
    } else {
        SetFalseValues(text);
    }
}

void MainWindow::on_ApplyButton_clicked()
{
    int i = 0;
    // События
    for (i = 0; i < ListP3Events.length(); ++i) {
        SetValues(ListP3Events.at(i).CB, ListP3Events.at(i).values);
    }

    if (ListP3Events.at(0).CB->isChecked()) {
        SetTrueValues("<Name>Know_BachelorWalkedDownTheStreet</Name>");
        SetTrueValues("<Name>Day0_Q8_BadTrip_Done</Name>");
        SetTrueValues("<Name>Day0_Q8_LurichevaStreet_Visited</Name>");
        SetTrueValues("<Name>Day0_Q8_LurichevaStreet_BadTrip</Name>");
    } else {
        SetFalseValues("<Name>Know_BachelorWalkedDownTheStreet</Name>");
        SetFalseValues("<Name>Day0_Q8_BadTrip_Done</Name>");
        SetFalseValues("<Name>Day0_Q8_LurichevaStreet_Visited</Name>");
        SetFalseValues("<Name>Day0_Q8_LurichevaStreet_BadTrip</Name>");
    }


    for (i = 0; i < lines.length(); ++i) {
        if (lines[i] == "\t\t\t\t\t\t<Key>ApatItemsMultiplier</Key>") {
            lines[i + 2] = "\t\t\t\t\t\t\t\t<Value>" +
                    QString::number(ui->ApatItemsMultiplierHSlider->value())
                    + "</Value>";
        }

        if (lines[i] == "\t\t\t\t\t\t<Key>RevolverBullets</Key>") {
            lines[i + 2] = "\t\t\t\t\t\t\t\t<Value>" +
                    QString::number(ui->RevolverBulletsHSlider->value())
                    + "</Value>";
        }

        if (lines[i] == "\t\t\t\t\t\t<Key>SprayAmmoCount</Key>") {
            lines[i + 2] = "\t\t\t\t\t\t\t\t<Value>" +
                    QString::number(ui->SprayAmmoCountHSlider->value())
                    + "</Value>";
        }
    }

    // Зачистка района
    for (i = 0; i < 9; ++i) {
        if (Shabnak_Dead_CheckBox[i]->isChecked()) {
            SetTrueValues("<Name>Shabnak_Dead_Day" + QString::number(i + 3) + "</Name>");
        } else {
            SetFalseValues("<Name>Shabnak_Dead_Day" + QString::number(i + 3) + "</Name>");
        }
    }

    currfile->open(QIODevice::WriteOnly);
    QTextStream out(currfile);

    int inx = 0;
    QString str = "";
    for (const QString& line : lines) {
        inx++;
        out << line << "\n";
    }
    currfile->close();

    // Архивация
    QString strcmd = "powershell Compress-Archive -Path '" + currdir->path() +
            "/tmp/*' -DestinationPath '" + SaveFilePath + "'  -Force";

    system(strcmd.toStdString().c_str());


    QMessageBox::information(this, "Успех", "Файл успешно изменен!");
}

void MainWindow::on_InfoButton_clicked()
{
    QMessageBox::information(this, "О программе",
                             "Pathologic 3 Helper - это программа, предназначенная "
                             "для редактирования файла сохранения.\n"
                             "Разработана она по причине большого числа багов\n"
                             "и спорных решений, которые портят впечатление от игры.\n"
                             "Автор: FIA\n"
                             "Email: f2000_99@mail.ru\n");
}


void MainWindow::on_AutoFindButton_clicked()
{
    SaveFilePath = QStandardPaths::writableLocation
            (QStandardPaths::AppConfigLocation);
    SaveFilePath.remove(SaveFilePath.lastIndexOf('/'), 18);
    SaveFilePath += "Low/Ice-Pick Lodge/Pathologic 3/Saves/Default/";
    QDir *qd = new QDir(SaveFilePath);
    qd->setFilter(QDir::Files);
    qd->setSorting(QDir::Time);

    SaveFilePath = qd->filePath(qd->entryInfoList().at(0).fileName());

    QFile curr_file(SaveFilePath);
    curr_file.open(QFile::ReadOnly);

    QMessageBox QMB(this);

    if (!curr_file.isOpen()) {
        QMB.setWindowTitle("Не удалось открыть файл!");
        QMB.setText("Файл сохранения обычно лежит по пути:\n"
                    "%AppData%\\..\\LocalLow\\Ice-Pick Lodge\\Pathologic 3\\Saves\\Default ");
        QMB.exec();
    } else {
        QMB.setWindowTitle("Найдено сохранение!");
        QMB.setText("Будет загружен самый свежий файл сохранения!");
        QMB.exec();

        curr_file.close();
        ui->SaveFileLineEdit->setText(SaveFilePath);
        LoadSaveFile();
    }
}

