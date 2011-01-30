#ifndef VYMVIEW_H 
#define VYMVIEW_H

#include <QItemSelectionModel>
#include <QWidget>

class VymModel;
class MapEditor;
class TreeEditor;

class QTreeView;

class VymView:public QWidget
{
    Q_OBJECT
public:
    VymView(VymModel *model);
    ~VymView();
    VymModel* getModel();
    MapEditor* getMapEditor();
    TreeEditor* getTreeEditor();
    void initFocus();

public slots:
    void changeSelection (const QItemSelection &newSel, const QItemSelection &delSel);
    void expandAll ();
    void expandOneLevel ();
    void collapseOneLevel ();
    void collapseUnselected();
    void showSelection ();
    void toggleTreeEditor();

private:
    VymModel *model;
    TreeEditor *treeEditor;
    QItemSelectionModel *selModel;
    MapEditor *mapEditor;
};


#endif
