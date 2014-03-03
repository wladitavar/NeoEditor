#include "mainwindow.h"
#include "webview.h"

MainWindow::MainWindow(QApplication *app)
{
    //initial window size
    QDesktopWidget *desktop = app->desktop();
    this->resizeAndCenter(desktop->width(), desktop->height());

    //tool bar
    QToolBar *fileToolbar = addToolBar(tr("&File"));

    QAction *openFolderAction = new QAction(tr("&Open Folder"), this);
    openFolderAction->setIcon(QIcon(":/images/folder-open.svg"));
    openFolderAction->setShortcut(QKeySequence::Open);
    openFolderAction->setStatusTip(tr("Open Folder"));
    connect(openFolderAction, SIGNAL(triggered()), this, SLOT(openFolder()));
    fileToolbar->addAction(openFolderAction);

    QAction *saveFileAction = new QAction(tr("&Save File"), this);
    saveFileAction->setIcon(QIcon(":/images/document-save.svg"));
    saveFileAction->setShortcut(QKeySequence::Save);
    saveFileAction->setStatusTip("Save File");
    connect(saveFileAction, SIGNAL(triggered()), this, SLOT(saveFile()));
    fileToolbar->addAction(saveFileAction);

    //left panel
    toolBox = new QToolBox(this);

    //right panel
    tabWidget = new QTabWidget(this);

    //layout
    splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(toolBox);
    splitter->addWidget(tabWidget);
    this->setCentralWidget(splitter);
    splitter->setVisible(false);
    splitter->setStretchFactor(1, 1);
}

void MainWindow::resizeAndCenter(int screenWidth, int screenHeight)
{
    int marginLeft = 200;
    int marginTop = 100;
    if(screenWidth < screenHeight)
    {
        marginLeft = 100;
        marginTop = 200;
    }
    this->resize(screenWidth - marginLeft * 2, screenHeight - marginTop * 2);
    this->move(marginLeft, marginTop);
}

void MainWindow::openFolder()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(folderPath != NULL)
    {
        showFolderTree(folderPath);
    }
}

void MainWindow::saveFile()
{
    QMessageBox::about(this, "title", "saving file");
}

void MainWindow::showFolderTree(QString folderPath)
{
    splitter->setVisible(true);
    QTreeView *treeView = new QTreeView(this);
    QFileSystemModel *fileSystemModel = new QFileSystemModel(this);
    fileSystemModel->setRootPath(folderPath);
    treeView->setModel(fileSystemModel);
    treeView->setRootIndex(fileSystemModel->index(folderPath));
    treeView->setColumnHidden(1, true);
    treeView->setColumnHidden(2, true);
    treeView->setColumnHidden(3, true);
    treeView->setHeaderHidden(true);
    treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openFile(QModelIndex)));
    toolBox->addItem(treeView, folderPath);
    if(toolBox->count() == 1)
    {
        QList<int> list = splitter->sizes();
        list[0] = 200;
        list[1] = 200;
        splitter->setSizes(list);
    }
    toolBox->setCurrentWidget(treeView);
}

void MainWindow::openFile(QModelIndex modelIndex)
{
    splitter->setVisible(true);
    QTreeView *treeView = (QTreeView*)sender();
    QFileSystemModel *fileSystemModel = (QFileSystemModel*)treeView->model();
    QFileInfo fileInfo = fileSystemModel->fileInfo(modelIndex);
    if(!fileInfo.isFile() || !fileInfo.isReadable())
    {
        return;
    }
    QString filePath = fileInfo.absoluteFilePath();

    WebView *webView = new WebView(this, filePath);
    webView->load(QUrl("qrc:///html/editor.html"));
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(initACE()));
    tabWidget->addTab(webView, filePath);
    tabWidget->setCurrentWidget(webView);
}

void MainWindow::initACE()
{
    WebView *webView = (WebView*)sender();
    QString filePath = webView->filePath();
    webView->page()->mainFrame()->evaluateJavaScript(QString("editor.getSession().setMode(modelist.getModeForPath('%1').mode);null;").arg(escapeJavascriptString(filePath)));
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    QString content = QString(file.readAll());
    file.close();
    webView->page()->mainFrame()->evaluateJavaScript(QString("editor.setValue('%1', -1);null;").arg(escapeJavascriptString(content)));
}

QString MainWindow::escapeJavascriptString(const QString &input)
{
    QString output;
    QRegExp regExp("(\\r|\\n|\\\\|\')");
    int pos = 0, lastPos = 0;
    while ((pos = regExp.indexIn(input, pos)) != -1)
    {
        output += input.mid(lastPos, pos - lastPos);
        switch (regExp.cap(1).at(0).unicode())
        {
        case '\r':
            output += "\\r";
            break;
        case '\n':
            output += "\\n";
            break;
        case '\'':
            output += "\\\'";
            break;
        case '\\':
            output += "\\\\";
            break;
        }
        pos += 1;
        lastPos = pos;
    }
    output += input.mid(lastPos);
    return output;
}
