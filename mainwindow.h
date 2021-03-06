#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

class QLineEdit;
class QDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void newFile();   // 新建操作
    bool maybeSave(); // 判断是否需要保存
    bool save();      // 保存操作
    bool saveAs();    // 另存为操作
    bool saveFile(const QString &fileName); // 保存文件
    bool AsaveFile(const QString &fileName, const QString &content);
    bool AsaveAsCoe(const QString &content);    // 汇编另存为操作
    bool AsaveAsBin(const QString &content);    // 汇编另存为操作
    bool loadFile(const QString &fileName); // 加载文件

    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_action_N_triggered();

    void on_action_S_triggered();

    void on_action_A_triggered();

    void on_action_O_triggered();

    void on_action_C_2_triggered();

    void on_actionExit_X_triggered();

    void on_action_U_triggered();

    void on_action_T_triggered();

    void on_action_C_triggered();

    void on_action_P_2_triggered();

    void showFindText();
    void showFindPrevText();

    void on_action_F_triggered();

    void on_action_Asm_triggered();

    void on_actionCoe_triggered();

    void on_actionBin_triggered();

    void on_action_triggered();

private:
    Ui::MainWindow *ui;
    bool isUntitled;// 文件尚未存在
    bool isUnsaved;// 文件尚未保存
    bool isAUntitled;// 汇编文件尚未存在
    QString curFile;// 保存当前文件的路径
    QString curAFile;// 保存汇编文件的路径
    QLineEdit *findLineEdit; // 查找输入条
    QDialog *findDlg; // 查找对话框
};

#endif // MAINWINDOW_H
