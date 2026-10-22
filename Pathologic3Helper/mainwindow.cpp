#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QStandardPaths>
#include <QMessageBox>
#include <QFileDialog>
#include <QFontDatabase>


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
    // Шрифт
    int id = QFontDatabase::addApplicationFont(":/new/prefix1/Res/BadScript-Regular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont BSfont(family);
    setFont(BSfont);

    ui->setupUi(this);

    ui->ShabnakCheckBox->setFont(BSfont);
    this->setStyleSheet(
        "QWidget {"
        "    background-color: rgb(0, 0, 0);"
        "    color: white;"
        "    font-size: 16pt;"
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
        "QSlider::handle:horizontal {"
            "border: 1px  red;"
            "background: red;"
            "border-radius: 8px;"
        "}"
    );

    // Генерация
    int var = 0;

    // События
    LP3E_Fill(ui, &ListP3Events, "Улица в Створках", "Know_BachelorWalkedDownTheStreet");
    LP3E_Fill(ui, &ListP3Events, "Секрет Симона", "Know_Main_YouAreSimon");
    LP3E_Fill(ui, &ListP3Events, "Просьба Уклада", "Intro_Station_ToBride");

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

    ui->ShabnakCheckBox->setChecked(true);

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

void MainWindow::CheckPlayerValues(QSlider *HS, QString text)
{
    float var_f = 0;
    int var_i = lines.indexOf("\t\t\t\t\t\t" + text);
    if (var_i != -1) {
        QString line = lines.at(var_i + 2);

        line.remove(0, line.indexOf('>') + 1);
        line.remove(line.indexOf('<'), line.indexOf('>'));
        var_f = line.toFloat();
        if (var_f < 1) {
            var_f *= 100;
        }
        HS->setValue((int)var_f);
    }
}

void MainWindow::CheckValues(QCheckBox *CH, QString text)
{
    int var_i = lines.indexOf("\t\t\t\t\t\t\t\t<Name>" + text + "</Name>");
    if (var_i != -1) {
        CH->setChecked(lines.at(var_i + 1) == "\t\t\t\t\t\t\t\t<Value>true</Value>");
    }
}

void MainWindow::SetPlayerValues(QString text, QString val)
{
    int var = lines.indexOf("\t\t\t\t\t\t" + text);
    lines[var + 2] = "\t\t\t\t\t\t<Value>" + val + "</Value>";
}

void MainWindow::SetValues(QString text, bool val)
{
    int inx;
    inx = lines.indexOf("\t\t\t\t\t\t\t\t<Name>" + text + "</Name>");

    if (val && lines[inx + 1] != "\t\t\t\t\t\t\t\t<Value>true</Value>")
    {
        lines.insert(inx + 1, "\t\t\t\t\t\t\t\t<Value>true</Value>");
    } else if (!val && lines[inx + 1] == "\t\t\t\t\t\t\t\t<Value>true</Value>") {
        lines.removeAt(inx + 1);
    }
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
        }
        else {
            int var_int = 0;
            float var_f = 0.0;
            QString var_qstr;
            bool SheIsNotExist = 0;

            // Запись всех параметров
            QTextStream in(currfile);
            QString line = "";
            while (!in.atEnd()) {
                line = in.readLine();
                lines << line;
            }

            // Параметры персонажа
            CheckPlayerValues(ui->ApatItemsMultiplierHSlider, "<Key>ApatItemsMultiplier</Key>");
            CheckPlayerValues(ui->RevolverBulletsHSlider, "<Key>RevolverBullets</Key>");
            CheckPlayerValues(ui->SprayAmmoCountHSlider, "<Key>SprayAmmoCount</Key>");

            // События
            int var = 0;
            for (var = 0; var < ListP3Events.length(); ++var) {
                CheckValues(ListP3Events.at(var).CB, ListP3Events.at(var).values);
            }

            CheckValues(ui->ShabnakCheckBox, "Shabnak_Enable");

            // Её не существует
            for (var = 0; var < 9; ++var) {
                CheckValues(Shabnak_Dead_CheckBox[var],
                "Shabnak_Dead_Day" + QString::number(var + 3));
            }
        }
        currfile->close();
        ui->LeapOfFaith->setStyleSheet("color: white");
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
        Shabnak_Dead_CheckBox[var]->setChecked(!arg1);
    }
}

void MainWindow::on_ApplyButton_clicked()
{
    if (SaveFilePath == "" || !currfile->exists()) {
        QMessageBox::information(this, "Не выбран файл", "Выберите файл для обработки.");
    } else {
        int i;
        // События
        for (i = 2; i < ListP3Events.length(); ++i) {
            SetValues(ListP3Events.at(i).values, ListP3Events.at(i).CB->isChecked());
        }
        // События с улицы
        SetValues("Know_BachelorWalkedDownTheStreet", ListP3Events.at(0).CB->isChecked());

        SetPlayerValues("<Key>ApatItemsMultiplier</Key>",
                        QString::number((float)ui->ApatItemsMultiplierHSlider->value() / 100));
        SetPlayerValues("<Key>RevolverBullets</Key>", QString::number(ui->RevolverBulletsHSlider->value()));
        SetPlayerValues("<Key>SprayAmmoCount</Key>", QString::number(ui->SprayAmmoCountHSlider->value()));

        QString tmpstr = "\t\t\t\t\t\t\t\t\t\t<Value>true</Value>\r\n";
        // Зачистка района
        for (i = 0; i < 9; ++i) {
            SetValues("Shabnak_Dead_Day" + QString::number(i + 3),
                              Shabnak_Dead_CheckBox[i]->isChecked());

            QFile tmpfile("tmp/Day" + QString::number(i + 3) + ".xml");
            if (tmpfile.exists()) {
                tmpfile.open(QIODevice::ReadWrite);
                if (tmpfile.isOpen()) {
                    QTextStream tmpts(&tmpfile);
                    QStringList tmpQSL;
                    QString tmpline = "";
                    while (!tmpfile.atEnd()) {
                        tmpline = tmpfile.readLine();
                        tmpQSL << tmpline;
                    }

                    int tmpinx = tmpQSL.indexOf("\t\t\t\t\t\t\t\t\t<Key>ShabnakMain</Key>\r\n");
                    if (Shabnak_Dead_CheckBox[i]->isChecked() &&
                            tmpQSL[tmpinx + 2] == tmpstr) {
                        tmpQSL.removeAt(tmpinx + 2);
                    } else if (!Shabnak_Dead_CheckBox[i]->isChecked() &&
                               tmpQSL[tmpinx + 2] != tmpstr)
                    {
                        tmpQSL.insert(tmpinx + 2, tmpstr);
                    }

                    tmpts.seek(0);

                    for (const QString& line : tmpQSL) {
                        tmpts << line;
                    }
                    tmpfile.close();
                }
            }

        }

        SetValues("Shabnak_Enable", ui->ShabnakCheckBox->isChecked());
        SetValues("ShabnakMain", ui->ShabnakCheckBox->isChecked());

        // пройтись по файлам дней


        currfile->open(QIODevice::WriteOnly);
        QTextStream out(currfile);

        QString str = "";
        for (const QString& line : lines) {
            out << line << "\n";
        }
        currfile->close();

        // Архивация
        QString strcmd = "powershell Compress-Archive -Path '" + currdir->path() +
                "/tmp/*' -DestinationPath '" + SaveFilePath + "'  -Force";

        system(strcmd.toStdString().c_str());

        QMessageBox::information(this, "Успех", "Файл успешно изменен!");
    }

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

void MainWindow::on_LeapOfFaith_clicked()
{
    ui->LeapOfFaith->setStyleSheet("color: red");
    // События Евы
    QList <QString> QuestEva = {
        "Day0_Q5_EvaBody_Done",
        "Day0_Q5_EvaBody_Day1-9_Done",
        "Day0_Q5_Cloud_Eva_Complete",
        "Citizen_StairwayToHeaven_Complete",
        "Day0_Q5_MM_NoDeath",
        "Day0_Q5_MM_EvaDied",
        "Day0_Q5_MM_Yesterday",
        "Cutscene_Eva_Body_Done",
        "Day0_Q5_MM_NoDeath_Resetable",
        "Day0_Q5_MM_EvaDied_Resetable",
        "Day0_Q5_MM_Yesterday_Resetable",
        "Day0_Q6_EvaResurrection_Done",
        "Day0_Q6_MM_Miracle ",
        "Day0_Q6_MM_Resurrection ",
        "Eva_Revival_Complete",
        "Day0_Q6_MM_Miracle_Resetable",
        "Day0_Q6_MM_Resurrection_Resetable",
        "Day0_Q7_EvaAliveAgain_Done",
        "Eva_Saved_Complete",
        "Day10_Q1_LeapOfFaith_Done",
        "Day10_Q1_LeapOfFaith_EvaFall",
        "Day10_Q1_LeapOfFaith_Know",
        "Day10_Q1_Eva_NoDeath_Complete",
        "Shadows_Eva_LeapOfFaith_Complete",
        "Day10_Q1_MM_NoDeathFinal",
        "Day10_Q1_MM_NoDeathStart",
        "Day10_Q1_MM_EvaDied",
        "Day10_Q1_MM_NoDeathFinal_Resetable",
        "Day10_Q1_MM_NoDeathStart_Resetable",
        "Day10_Q1_MM_EvaDied_Resetable",
        "Day10_Q11_EvaOnTheStairs_1_Done",
        "Day10_Q11_EvaOnTheStairs_1_Force",
        "Day10_Q11_EvaOnTheStairs_1_Know",
        "Day10_Q11_Caretaker_Key_Complete",
        "Day10_Q11_Tsipukh_Trespassing_Complete",
        "Day10_Q11_Karminskiy_Roof_Complete",
        "Day10_Q11_Cloud_AliveEva_Complete",
        "Day10_Q11_Eva_Alive_Complete",
        "Day10_Q11_SwampLights",
        "Day10_Q11_Cloud_CathedralDoor_Complete",
        "Day10_Q11_1_MM_KeyKeeper",
        "Day10_Q11_1_MM_Serenity",
        "Day10_Q11_1_MM_Eurydice",
        "Day10_Q11_1_MM_Skies",
        "Day10_Q11_1_MM_Cathedral",
        "Day10_Q11_1_MM_KeyKeeper_Resetable",
        "Day10_Q11_1_MM_Serenity_Resetable",
        "Day10_Q11_1_MM_Eurydice_Resetable",
        "Day10_Q11_1_MM_Skies_Resetable",
        "Day10_Q11_1_MM_Cathedral_Resetable",
        "Day10_Q11_EvaOnTheStairs_2_Done",
        "Day10_Q11_EvaOnTheStairs_2_Force",
        "Day10_Q11_EvaOnTheStairs_2_Know",
        "Day10_Q11_Eva_Liminal_Complete",
        "Day10_Q11_2_MM_ObserverEffect",
        "Day10_Q11_2_MM_WhereIsMind",
        "Day10_Q11_EvaOnTheStairs_3_Done",
        "Day10_Q11_EvaOnTheStairs_3_Force",
        "Day10_Q11_EvaOnTheStairs_3_Know",
        "Day10_Q11_Caretaker_NoEva_Complete",
        "Day10_Q11_Eva_Saved_Complete",
        "Day10_Q11_3_MM_Choice",
        "Day10_Q11_3_MM_Concentration",
        "Day10_Q11_3_MM_Soul",
        "Day10_Q11_3_MM_Freedom",
        "Know_EvaIsAlive",
        "Know_EvaIsLiminal",
        "Day10_Q11_3_MM_Choice_Resetable",
        "Day10_Q11_3_MM_Concentration_Resetable",
        "Day10_Q11_3_MM_Soul_Resetable",
        "Day10_Q11_3_MM_Freedom_Resetable",
        "Day12_Eva_FateImmortal",
        "Day12_Eva_FateLiminal",
        "Day12_Eva_FateImmortal_Told",
        "Day12_Eva_FateLiminal_Told",
        "Day12_Q15_Eva_Final_Complete",
        "Day12_Eva_FateImmortal_Told",
        "Day12_Q15_LiminalEva_Final_Complete",
        "Day12_Q15_Cloud_Eva_Final_Complete"
    };
    for (int i = 0; i < QuestEva.length(); ++i) {
        SetValues(QuestEva[i], false);
    }
}

