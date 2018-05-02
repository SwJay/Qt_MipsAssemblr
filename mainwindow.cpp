#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QTextStream>
#include <QString>
#include <QDialog>
#include <QLineEdit>
#include "assembler.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize( QSize( 800, 600 )); // initialize the size of window
    ui->textEdit->setReadOnly(true); // 窗口只读
    ui->textEdit_2->setReadOnly(true); // 窗口只读
    ui->textEdit_3->setReadOnly(true); // 窗口只读
    ui->textEdit->append("<font color = 'gray'>NO FILE</font>");

    //源文件
    isUntitled = false;
    isUnsaved = false;
    //汇编文件
    isAUntitled = false;

    //查找的弹窗设置
    findDlg = new QDialog(this);
    findDlg->setWindowTitle("查找");
    findLineEdit = new QLineEdit(findDlg);
    QPushButton *btn1 = new QPushButton("查找下一个",findDlg);
    QPushButton *btn2 = new QPushButton("查找上一个",findDlg);
    QVBoxLayout *layout = new QVBoxLayout(findDlg);
    layout->addWidget(findLineEdit);
    layout->addWidget(btn1);
    layout->addWidget(btn2);
    connect(btn1,&QPushButton::clicked,this,&MainWindow::showFindText);
    connect(btn2,&QPushButton::clicked,this,&MainWindow::showFindPrevText);

}

void MainWindow::newFile(){
    maybeSave();
    isUntitled = true;
    isUnsaved = true;
    isAUntitled = true;
    curFile = "未命名.txt";
    setWindowTitle("MIPS_Assemblr @"+curFile);
    ui->textEdit->clear();
    ui->textEdit->setReadOnly(false);
}

bool MainWindow::maybeSave(){
    if(ui->textEdit->document()->isModified() && isUnsaved){//有更改，提示是否保存
        QMessageBox box;
        box.setWindowTitle("提示");
        box.setIcon(QMessageBox::Warning);
        box.setText(curFile + "尚未保存，是否保存？");
        QPushButton *yesBut = box.addButton("是(&Y)",
                                            QMessageBox::YesRole);
        QPushButton *cancelBut = box.addButton("否(&N)",
                                               QMessageBox::RejectRole);

        box.exec();
        if(box.clickedButton()==yesBut)
            return save();
        else if(box.clickedButton()==cancelBut)
            return false;
    }
    return true;
}

bool MainWindow::save(){
    if(isUntitled)
        return saveAs();
    else
        return saveFile(curFile);
}

bool MainWindow::saveAs(){
    QString fileName = QFileDialog::getSaveFileName(this,"另存为",curAFile,
                                                    "TXT (*.txt);;Asm (*.asm);;COE (*.coe);;BIN (*.bin)");
    if(fileName.isEmpty())
        return false;
    return saveFile(fileName);
}

bool MainWindow::saveFile(const QString &fileName){
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text)){ // 报错
        QMessageBox::warning(this, "MIPS_Assemblr",
                             QString("无法写入文件 %1, /n %2")
                             .arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    //鼠标指针变为等待状态
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << ui->textEdit->toPlainText();
    //鼠标指针恢复原来的状态
    QApplication::restoreOverrideCursor();
    isUntitled = false;
    isUnsaved = false;
    //获得文件的标准路径
    curFile = QFileInfo(fileName).canonicalFilePath();
    setWindowTitle("MIPS_Assemblr @"+curFile);
    return true;
}

bool MainWindow::loadFile(const QString &fileName){
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)){ // 打开报错
        QMessageBox::warning(this,"MIPS_Assemblr",
                             QString("无法读取文件 %1:\n%2.")
                             .arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->textEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    ui->textEdit->setReadOnly(false); // 窗口可写
    isUntitled = false; // 此文件已存在
    isAUntitled = true; // 假设汇编文件不存在
    curFile = QFileInfo(fileName).canonicalFilePath();
    setWindowTitle("MIPS_Assemblr @"+curFile);
    return true;
}

void MainWindow::showFindText(){
    QString str = findLineEdit->text();

    if(!ui->textEdit->find(str,QTextDocument::FindFlag(0)))
        QMessageBox::warning(this,"查找",QString("找不到%1").arg(str));
}

void MainWindow::showFindPrevText(){
    QString str = findLineEdit->text();

    if(!ui->textEdit->find(str,QTextDocument::FindBackward))
        QMessageBox::warning(this,"查找",QString("找不到%1").arg(str));
}

MainWindow::~MainWindow()
{
    delete ui;
}
// 新建动作
void MainWindow::on_action_N_triggered()
{
    newFile();
}
// 保存动作
void MainWindow::on_action_S_triggered()
{
    save();
}
// 另存为动作
void MainWindow::on_action_A_triggered()
{
    saveAs();
}
// 打开动作
void MainWindow::on_action_O_triggered()
{
    maybeSave();
    QString fileName = QFileDialog::getOpenFileName(this,"打开",QString(),
                                                    "TXT (*.txt);;Asm (*.asm);;COE (*.coe);;BIN (*.bin)");
    // 文件名非空则加载
    if(!fileName.isEmpty())
        loadFile(fileName);
}
// 关闭动作
void MainWindow::on_action_C_2_triggered()
{
    maybeSave();
    ui->textEdit->clear();
    ui->textEdit->setReadOnly(true);
    ui->textEdit->append("<font color = 'gray'>NO FILE</font>");
    isUntitled = false;
    isUnsaved = false;
}
// 退出动作
void MainWindow::on_actionExit_X_triggered()
{
    on_action_C_2_triggered();
    qApp->quit(); // qApp是指向应用程序的全局指针
}
// 撤销动作
void MainWindow::on_action_U_triggered()
{
    ui->textEdit->undo();
}
// 剪切动作
void MainWindow::on_action_T_triggered()
{
    ui->textEdit->cut();
}
// 复制动作
void MainWindow::on_action_C_triggered()
{
    ui->textEdit->copy();
}
// 粘贴动作
void MainWindow::on_action_P_2_triggered()
{
    ui->textEdit->paste();
}
// 直接关闭窗口
void MainWindow::closeEvent(QCloseEvent *event){
    maybeSave();
    event->accept();
}

void MainWindow::on_action_F_triggered(){
    findDlg->show();
}

void MainWindow::on_action_Asm_triggered(){
    Assembler assembler;//嵌入汇编器
    QString myasm = ui->textEdit->toPlainText(); // 框里的汇编语言
    QString mybin = assembler.convert(myasm); // 转成机器语言
    Asave(mybin);
    isAUntitled = false; // 第一次项目汇编后 汇编文件存在
    ui->textEdit_2->append(mybin);
}

bool MainWindow::Asave(const QString &content){
    if(isAUntitled)
        return AsaveAs(content);
    else
        return AsaveFile(curAFile,content);
}

bool MainWindow::AsaveAs(const QString &content){
    QString fileName = QFileDialog::getSaveFileName(this,"另存为",curAFile,
                                                    "TXT N(*.txt);;Asm (*.asm);;COE (*.coe);;BIN (*.bin)");
    if(fileName.isEmpty())
        return false;
    return AsaveFile(fileName,content);
}
bool MainWindow::AsaveFile(const QString &fileName, const QString &content){
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text)){ // 报错
        QMessageBox::warning(this, "MIPS_Assemblr",
                             QString("无法写入文件 %1, /n %2")
                             .arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    //鼠标指针变为等待状态
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << content;
    //鼠标指针恢复原来的状态
    QApplication::restoreOverrideCursor();
    isAUntitled = false;
    curAFile = QFileInfo(fileName).canonicalFilePath();
    return true;
}

