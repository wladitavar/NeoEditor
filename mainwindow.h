#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QtWidgets>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QApplication *app);

protected slots:
    QString openFileDialog();

private slots:
    void openFolder();
    void openFile(QModelIndex modelIndex);
    void initACE();

private:
    void resizeAndCenter(int screenWidth, int screenHeight);
    void showFolderTree(QString folderPath);
    QToolBox *toolBox;
    QSplitter *splitter;
    QTabWidget *tabWidget;
    QString escapeJavascriptString(const QString & str);
};


#endif // MAINWINDOW_H
