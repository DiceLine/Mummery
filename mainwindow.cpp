#include "mainwindow.h"
#include "./ui_mainwindow.h"

const QString MainWindow::op[length] = {
    "not",
    "and",
    "or",
    "xor"
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for(int i = 0; i < ELEMENTS; ++i) {
        elementHadError[i] = false;
    }

    lineEditFont = ui->lineEdit_inFile->font();

    ui->lineEdit_interval->hide();

    timer = new QTimer(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setLineEditError(QLineEdit* ptr, const bool& status, QString message)
{
    if(status) {
        ptr->setStyleSheet("background-color: red;");
        ptr->setStatusTip(message);
        return;
    }

    ptr->setStyleSheet("");
    ptr->setFont(lineEditFont);
    ptr->setStatusTip("");
}


void MainWindow::on_lineEdit_inFile_editingFinished() {
    inFileName = ui->lineEdit_inFile->text();

    if(inFileName.isEmpty()) {
        elementHadError[0] = true;
        setLineEditError(ui->lineEdit_inFile, false);
        return;
    }

    if(QFile::exists(inFileName)) {
        elementHadError[0] = false;
        setLineEditError(ui->lineEdit_inFile, false);
        return;
    }

    elementHadError[0] = true;
    setLineEditError(ui->lineEdit_inFile, true, "Недопустимый ввод: Такого файла не существует");
}


void MainWindow::on_checkBox_deleteInFile_stateChanged(int arg1)
{
    isInFileDeletionON = arg1;
}


void MainWindow::on_checkBox_rewrite_stateChanged(int arg1)
{
    isOutFileRewritingON = arg1;

    if(isOutFileRewritingON) {
        ui->checkBox_copying->hide();
        return;
    }

    ui->checkBox_copying->show();
}


void MainWindow::on_checkBox_copying_stateChanged(int arg1)
{
    isOutFileCopyingON = arg1;
}


void MainWindow::on_lineEdit_operation_editingFinished()
{
    static QString str;

    str = ui->lineEdit_operation->text();

    if(str.isEmpty()) {
        setLineEditError(ui->lineEdit_operation, false);
        elementHadError[1] = true;
        return;
    }

    for(int i = 0; i < length; ++i) {
        if(str.compare(op[i], Qt::CaseInsensitive) == 0) {
            operation = static_cast<Operation>(i);
            setLineEditError(ui->lineEdit_operation, false);
            elementHadError[1] = false;
            return;
        }
    }

    str = "Недопустимый ввод: Укажите логическую операцию. Список операций см. по кнопке справка.";

    elementHadError[1] = true;
    setLineEditError(ui->lineEdit_operation, true, str);
}


void MainWindow::on_lineEdit_8BMask_editingFinished()
{
    static bool         status;
    static qint8        value;
    static qint32       size;
    static QString      input;

    input = ui->lineEdit_8BMask->text();

    elementHadError[2] = false;

    if(input.isEmpty()) {
        elementHadError[2] = true;
        setLineEditError(ui->lineEdit_8BMask, false);
        return;
    }

    size = input.size();

    for(int i = 0; i < size; ++i) {
        value = input.mid(i, 2).toInt(&status, 16);
        if(!status) {
            elementHadError[2] = true;
            setLineEditError(ui->lineEdit_8BMask, true);
            return;
        }
        mask[i] = value;
    }

    setLineEditError(ui->lineEdit_8BMask, false);
}


void MainWindow::on_checkBox_timer_stateChanged(int arg1)
{
    isTimerON = arg1;

    if(isTimerON) {
        ui->lineEdit_interval->show();
        return;
    }

    ui->lineEdit_interval->hide();
}


void MainWindow::on_lineEdit_interval_editingFinished()
{
    static bool     success;
    static QString  input;
    static qint32   value;

    input = ui->lineEdit_interval->text();

    if(input.isEmpty()) {
        setLineEditError(ui->lineEdit_interval, false);
        elementHadError[3] = true;
        return;
    }

    if(input.endsWith("ms", Qt::CaseInsensitive)) {
        input = input.left(input.length() - 2);
        value = input.toInt(&success);
    } else {
        value = input.toInt(&success) * 1000;
    }

    if(success) {
        interval = value;
        setLineEditError(ui->lineEdit_interval, false);
        elementHadError[3] = false;
    } else {
        setLineEditError(ui->lineEdit_interval, true, "Недопустимый ввод: Требуется числовой формат");
        elementHadError[3] = true;
    }
}


void MainWindow::transformationStream()
{
    static QFile file;


    for(int i = 0; i < ELEMENTS; ++i) {
        if(elementHadError[i]) {
            return;
        }
    }

    file.setFileName(inFileName);

    if(!file.open(QIODeviceBase::ReadOnly)) {
        return;
    }

    data = file.readAll();

    file.close();


    static qint64 size;

    size = data.size();

    if(size % CHUNK_SIZE != 0) {
        return;
    }

    static QByteArray chunk;
    static QString prefixForAddingMode;

    prefixForAddingMode = "New data block (" + QString::number(QRandomGenerator::global()->generate()) + "):\n";

    result.clear();
    if(!isOutFileRewritingON && !isOutFileCopyingON) {
        result.append(prefixForAddingMode.toStdString());
    }

    for(qint64 i = 0; i < size; i += CHUNK_SIZE) {
        chunk = data.mid(i, CHUNK_SIZE);

        static qint8 resultByte, iter;

        iter = 0;
        for(qint8 byte: std::as_const(chunk)) {
            switch(operation)
            {
            case NOT: {
                resultByte = ~byte;
                break;
            }
            case AND: {
                resultByte = byte & mask[iter];
                break;
            }
            case OR: {
                resultByte = byte | mask[iter];
                break;
            }
            case XOR: {
                resultByte = byte ^ mask[iter];
                break;
            }
            default:
                return;
            }

            ++iter;
            result.append(resultByte);
        }
    }


    /*static QRegularExpression expr(R"((\w+(\.\w+)+))");

    if(!outFilePath.contains(expr)) {
        outFilePath += "/masked_file.txt";
    }*/

    static QFile::OpenModeFlag mode;

    mode = QFile::WriteOnly;

    if(!isOutFileRewritingON && !isOutFileCopyingON) {
        mode = QFile::Append;

    } else if(isOutFileCopyingON) {
        static int k;
        k = 0;
        do {
            outFilePath = "/masked_file(" + QString::number(k++) + ").txt";
        } while(QFile::exists(outFilePath) && k < 1000);
    }

    file.setFileName(outFilePath);

    if(!file.open(mode)) {
        return;
    }

    file.write(result);

    file.close();

    if(isInFileDeletionON) {
        QFile::remove(inFileName);
    }
}


void MainWindow::on_pushButton_transform_clicked()
{
    if(timer->isActive()) {
        timer->stop();
        ui->groupBox_inFile->setEnabled(true);
        ui->groupBox_otFile->setEnabled(true);
        ui->groupBox_transform->setEnabled(true);
        return;
    }

    if(!isTimerON) {
        transformationStream();
        return;
    }

    ui->groupBox_inFile->setDisabled(true);
    ui->groupBox_otFile->setDisabled(true);
    ui->groupBox_transform->setDisabled(true);

    timer->callOnTimeout(this, &MainWindow::transformationStream);
    timer->start(interval);
}

