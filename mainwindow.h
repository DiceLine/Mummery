#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QFileInfo>
#include <QFile>
#include <QRegularExpression>
#include <QRandomGenerator>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_lineEdit_inFile_editingFinished();

    void on_checkBox_deleteInFile_stateChanged(int arg1);

    void on_checkBox_rewrite_stateChanged(int arg1);

    void on_checkBox_copying_stateChanged(int arg1);

    void on_lineEdit_operation_editingFinished();

    void on_lineEdit_8BMask_editingFinished();

    void on_checkBox_timer_stateChanged(int arg1);

    void on_lineEdit_interval_editingFinished();

    void on_pushButton_transform_clicked();

    void on_lineEdit_otFilePath_editingFinished();

    void timerTransformationStream();

private:
    Ui::MainWindow *ui;

    // Properties
    QFont lineEditDefaultFont;

    enum Operation {
        NOT = 0,
        AND,
        OR,
        XOR,
        length
    };

    static const QString op[length];
    static const int ELEMENTS = 5;
    static const int CHUNK_SIZE = 8;
    static const int COPYIES_MAX_VALUE = 3000;

    QTimer*     timer;
    qint32      interval;

    qint8       mask[CHUNK_SIZE];
    Operation   operation;

    QByteArray  data;
    QByteArray  result;

    QString     inFileName;
    QString     outFilePath;

    bool        elementHadError[ELEMENTS];

    bool        isInFileDeletionON;
    bool        isOutFileRewritingON;
    bool        isOutFileCopyingON;
    bool        isTimerModeON;


    void setLineEditError(QLineEdit* ptr, const bool& status, QString message = "");
    void transformationStream();
};
#endif // MAINWINDOW_H
