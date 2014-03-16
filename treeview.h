#ifndef TREEVIEW_H
#define TREEVIEW_H


#include <QtWidgets>

class TreeView : public QTreeView
{
    Q_OBJECT

public:
    TreeView(QWidget* parent);

private slots:
    void showContextMenu(const QPoint &point);
    void deleteFile();
    void newFile();
    void newFolder();
};


#endif // TREEVIEW_H
