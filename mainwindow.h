#ifndef MAINWINDOW_H 
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDBus>


#include "branchpropwindow.h"
#include "extrainfodialog.h"
#include "flag.h"
#include "file.h"
#include "historywindow.h"
#include "mapeditor.h"
#include "shortcuts.h"
#include "simplescripteditor.h"
#include "texteditor.h"
#include "vymview.h"

class Main : public QMainWindow 
{
    Q_OBJECT

public:
    /*! Modifier modes are used when CTRL together with a mouse button is pressed */
    enum ModMode {
	ModModeNone,	//!< Unused
	ModModeColor,	//!< Pick color from object
	ModModeCopy,	//!< Copy object
	ModModeXLink	//!< Create a XLink (XLinkObj) from selected object
	};

    Main(QWidget* parent=0, Qt::WFlags f=0);
    ~Main();
    void loadCmdLine();

private:
    QProgressDialog progressDialog;
    int progressMax;
    int progressCounter;
    int progressCounterTotal;
public:	
    void statusMessage (const QString &);
    void setProgressMaximum (int max);
    void addProgressValue (float v);
    void removeProgressCounter();

public slots:
    void fileNew();
    void fileNewCopy();

protected:
    void closeEvent( QCloseEvent* );

private:
    void setupFileActions();
    void setupEditActions();
    void setupFormatActions();
    void setupViewActions();
    void setupModeActions();
    void setupWindowActions();
    void setupFlag(Flag *flag, QToolBar *tb, const QString &name, const QString &tooltip);
    void setupFlagActions();
    void setupNetworkActions();
    void setupSettingsActions();
    void setupTestActions();
    void setupHelpActions();
    void setupContextMenus();
    void setupRecentMapsMenu();
    void setupMacros();
    void hideEvent (QHideEvent * );
    void showEvent (QShowEvent * );
    MapEditor* currentMapEditor() const;
    VymModel* currentModel() const;
public:	
    uint currentModelID() const;
    VymModel* getModel(uint) const;
    
private slots:
    void editorChanged(QWidget*);

    ErrorCode fileLoad(QString ,const LoadMode &, const FileType & ftype=VymMap);
    void fileLoad(const LoadMode &);
    void fileLoad();
    void fileLoadRecent();
    void addRecentMap (const QString &);
    void fileSave(VymModel*, const SaveMode & );
    void fileSave();
public slots:	
    void fileSave(VymModel*);	// autosave from MapEditor
private slots:	
    void fileSaveAs(const SaveMode &);
    void fileSaveAs();
    void fileImportKDE3Bookmarks();
    void fileImportKDE4Bookmarks();
    void fileImportFirefoxBookmarks();
    void fileImportFreemind();
    void fileImportMM();
    void fileImportDir();
    void fileExportXML();
    void fileExportHTML();
    void fileExportImage();
    void fileExportPDF();
    void fileExportSVG();
    void fileExportAO();
    void fileExportASCII();
    void fileExportCSV();
    void fileExportLaTeX();
    void fileExportKDE3Bookmarks();
    void fileExportKDE4Bookmarks();
    void fileExportTaskjuggler();
    void fileExportOOPresentation();
    void fileExportLast();
    bool fileCloseMap();
    void filePrint();
    bool fileExitVYM();

public slots:
    void editUndo();	
    void editRedo();	
    void gotoHistoryStep (int);
private slots:	
    void editCopy();	
    void editPaste();	
    void editCut(); 
    void editToggleTarget();
    void editGoToTarget();
    void editMoveToTarget();
    void editOpenFindResultWidget();
    void editFindNext(QString s);
    void editFindDuplicateURLs();

public slots:    
    void updateQueries (VymModel*);
private:
    void openTabs(QStringList);
public slots:
    void editOpenURL();
    void editOpenURLTab();
private slots:
    void editOpenMultipleVisURLTabs(bool ignoreScrolled=true);
    void editOpenMultipleURLTabs();
    void editNote2URLs();
    void editURL();
    void editLocalURL();
    void editHeading2URL();
    void editBugzilla2URL();
    void getBugzillaData();
    void getBugzillaDataSubtree();
    void editFATE2URL();
    void openVymLinks(const QStringList &);
    void editVymLink();
    void editOpenMultipleVymLinks();
public slots:
    void editHeadingFinished(VymModel *m);
//    void editHeading();
    void editOpenVymLink();
private slots:
    void editDeleteVymLink();
    void editToggleHideExport();
    void editAddTimestamp();
    void editMapInfo();
    void editMoveUp();	
    void editMoveDown();    
    void editDetach();	
    void editSortChildren();
    void editSortBackChildren();
    void editToggleScroll();
    void editExpandAll();
    void editExpandOneLevel();
    void editCollapseOneLevel();
    void editCollapseUnselected();
    void editUnscrollChildren();
    void editGrowSelectionSize();
    void editShrinkSelectionSize();
    void editResetSelectionSize();
    void editAddAttribute();
    void editAddMapCenter();
    void editNewBranch();
    void editNewBranchBefore();
    void editNewBranchAbove();
    void editNewBranchBelow();
    void editImportAdd();
    void editImportReplace();
    void editSaveBranch();
    void editDeleteKeepChildren();
    void editDeleteChildren();
    void editDeleteSelection();
    void editLoadImage();
    void editSaveImage();
    void popupFollowXLink ();
    void editFollowXLink (QAction *);
    void editEditXLink (QAction *);

    void formatSelectColor();
    void formatPickColor();
    void colorChanged(QColor);
    void formatColorBranch();
    void formatColorSubtree();
    void formatLinkStyleLine();
    void formatLinkStyleParabel();
    void formatLinkStylePolyLine();
    void formatLinkStylePolyParabel();
    void formatSelectBackColor();
    void formatSelectBackImage();
    void formatSelectLinkColor();
    void formatSelectSelectionColor();
    void formatToggleLinkColorHint();
    void formatHideLinkUnselected();

public slots:
    void viewZoomReset();
    void viewZoomIn();
    void viewZoomOut();
    void viewCenter();

public slots:
    void networkStartServer();
    void networkConnect();
    bool settingsPDF();
    bool settingsURL();
    void settingsMacroDir();
    void settingsToggleDelKey();
    void settingsUndoLevels();
    void settingsAutosaveToggle();
    void settingsAutosaveTime();
    void settingsAutoLayoutToggle();
    void settingsWriteBackupFileToggle();
    void settingsToggleNoteEditorIsDockWindow();
    void settingsToggleAnimation();

    void windowToggleNoteEditor();
    void windowToggleTreeEditor();
    void windowToggleHistory();
    void windowToggleProperty();
    void updateHistory(SimpleSettings &);
    void windowToggleAntiAlias();
public:
    bool isAliased();
    bool hasSmoothPixmapTransform();
public slots:
    void windowToggleSmoothPixmap();
    void updateHeading();
    void updateNoteFlag();
    void updateNoteEditor (QModelIndex index);
    void selectInNoteEditor (QString s, int i);
    void changeSelection (VymModel *model,const QItemSelection &newSel, const QItemSelection &delSel);

    void updateActions();
    ModMode getModMode();
    bool autoEditNewBranch();
    bool autoSelectNewBranch();
    void setScript(const QString &);
    void runScript(const QString &);
    void runScriptEverywhere (const QString &);

private slots:
    void windowNextEditor();
    void windowPreviousEditor();
    void windowShowNoteEditor();
    void windowHideNoteEditor();

    void standardFlagChanged();

    void testFunction1();
    void testFunction2();
    void testCommand();

    void helpDoc();
    void helpDemo();
    void helpAbout();
    void helpAboutQT();

    void callMacro ();

private:
    QTabWidget *tabWidget;
    qint64 *browserPID;

    QStringList imageTypes;

    QList <VymView*> vymViews;	    //! Keeps track of models and views related to a tab 
    QString prevSelection;

    HistoryWindow *historyWindow;

    QDockWidget *headingEditorDW;
    QDockWidget *noteEditorDW;

    BranchPropertyWindow *branchPropertyWindow;
    SimpleScriptEditor *scriptEditor;

    QList <QAction*> actionListBranches;

    QColor currentColor;

    int xLinkMenuWidth;

    QMenu *recentFilesMenu;
    enum { MaxRecentFiles = 9 };
    QAction *recentFileActions[MaxRecentFiles];

    QAction *macroActions[12];
    QStringList macro;

    Switchboard switchboard;

    QAction* actionFileNewCopy;
    QAction* actionFileSave;
    QAction* actionFilePrint;
    QAction* actionFileExportLast;
    QAction* actionUndo;
    QAction* actionRedo;
    QAction *actionCopy;
    QAction *actionCut;
    QAction *actionPaste;
    QAction *actionMoveUp;
    QAction *actionMoveDown;
    QAction *actionDetach;
    QAction *actionSortChildren;
    QAction *actionSortBackChildren;
    QAction *actionToggleScroll;
    QAction *actionExpandAll;
    QAction *actionExpandOneLevel;
    QAction *actionCollapseOneLevel;
    QAction *actionCollapseUnselected;
    QAction* actionOpenURL;
    QAction* actionOpenURLTab;
    QAction* actionOpenMultipleVisURLTabs;
    QAction* actionOpenMultipleURLTabs;
    QAction* actionURL;
    QAction* actionLocalURL;
    QAction* actionHeading2URL;
    QAction* actionBugzilla2URL;
    QAction* actionGetBugzillaData;
    QAction* actionGetBugzillaDataSubtree;
    QAction* actionFATE2URL;
    QAction *actionOpenVymLink;
    QAction *actionOpenMultipleVymLinks;
    QAction *actionVymLink;
    QAction *actionDeleteVymLink;
    QAction *actionAddTimestamp;
    QAction *actionToggleHideExport;
    QAction *actionMapInfo;
    QAction *actionHeading;
    QAction *actionDelete;
    QAction *actionAddAttribute;
public:
    QAction *actionAddMapCenter;

private:    
    QAction *actionAddBranch;
    QAction *actionAddBranchBefore;
    QAction *actionAddBranchAbove;
    QAction *actionAddBranchBelow;
    QAction *actionDeleteKeepChildren;
    QAction *actionDeleteChildren;
    QAction *actionImportAdd;
    QAction *actionImportReplace;
    QAction *actionSaveBranch;
    QAction *actionLoadImage;

    QAction* actionFormatColor;
    QAction* actionFormatPickColor;
    QAction* actionFormatColorBranch;
    QAction* actionFormatColorSubtree;
    QAction* actionFormatLinkColorHint;
    QAction* actionFormatBackColor;
    QAction* actionFormatBackImage;
    QAction* actionFormatLinkColor;
    QAction* actionFormatSelectionColor;

    QActionGroup* actionGroupModModes;
    QAction* actionModModeColor;
    QAction* actionModModeXLink;
    QAction* actionModModeCopy;

    QActionGroup *actionGroupFormatFrameTypes;


    QActionGroup *actionGroupFormatLinkStyles;
    QAction *actionFormatLinkStyleLine;
    QAction *actionFormatLinkStyleParabel;
    QAction *actionFormatLinkStylePolyLine;
    QAction *actionFormatLinkStylePolyParabel;
    QAction *actionFormatHideLinkUnselected;

    QAction *actionViewToggleNoteEditor;
    QAction *actionViewToggleHeadingEditor;
    QAction *actionViewToggleTreeEditor;
    QAction *actionViewToggleHistoryWindow;
    QAction *actionViewTogglePropertyWindow;
    QAction *actionViewToggleAntiAlias;
    QAction *actionViewToggleSmoothPixmapTransform;
    QAction* actionViewCenter;

    QAction* actionSettingsAutoEditNewBranch;
    QAction* actionSettingsAutoSelectNewBranch;
    QAction* actionSettingsAutoSelectText;
    QAction* actionSettingsUseDelKey;
    QAction* actionSettingsUseFlagGroups;
    QAction* actionSettingsUseHideExport;
    QAction* actionSettingsAutosaveToggle;
    QAction* actionSettingsAutosaveTime;
    QAction* actionSettingsAutoLayoutToggle;
    QAction* actionSettingsWriteBackupFile;
    QAction* actionSettingsUseAnimation;
    QAction* actionSettingsNoteEditorIsDockWindow;
};


#endif
