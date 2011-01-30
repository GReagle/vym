#ifndef VYMMODEL_H
#define VYMMODEL_H

#include <QtNetwork>

#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusVariant>

#include <QTextCursor>

#include "adaptormodel.h"
#include "file.h"
#include "imageitem.h"
#include "mapeditor.h"
#include "parser.h"
#include "treeitem.h"
#include "treemodel.h"

class AttributeItem;
class BranchItem;
class FindResultModel;
class MapEditor;
class Link;
class XLinkItem;

class QGraphicsScene;

typedef QMap<uint,QString> ItemList ;

class VymModel :  public TreeModel {	    
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.insilmaril.vym.VymModel-h")

////////////////////////////////////////////
// General housekeeping
////////////////////////////////////////////
private:
    QString version;	//!< version string saved in vym file
    QString author;
    QString comment;
    QDate date;

    static uint idLast;	    //! the last used unique ID
    uint mapID;

public:
    VymModel();
    ~VymModel ();
    void clear();
    void init();
    void makeTmpDirectories();	    //!< create temporary directories e.g. for history

    MapEditor* getMapEditor();		
    uint getID();			//! Return unique ID of model

    bool isRepositionBlocked();	    //!< While load or undo there is no need to update graphicsview

    void updateActions();	    //!< Update buttons in mainwindow


////////////////////////////////////////////
// Load/save 
////////////////////////////////////////////
private:

    bool zipped;		// should map be zipped
    static  int mapNum;		// unique number for model used in save/undo
    FileType fileType;		// type of file, e.g. vym, freemind...
    QString fileName;		// short name of file (for tab)
    QString filePath;		// path to file which will be saved
    QString fileDir;		// dir where file is saved
    QString destPath;		// path to .vym file (needed for vymlinks)
    QString mapName;		// fileName without ".vym"

    QString tmpMapDir;		// tmp directory with undo history

    QTimer *autosaveTimer;
    QTimer *fileChangedTimer;
    QDateTime fileChangedTime;

public:
    /*! This function saves all information of the map to disc.
	saveToDir also calls the functions for all BranchObj and other objects in the map.
	The structure of the map itself is returned as QString and passed back to Main, 
	where saveToDir is called initially
    */	
    QString saveToDir (const QString &tmpdir, const QString &prefix, bool writeflags, const QPointF &offset, TreeItem *saveSel);

    /*! Save all data in tree*/
    QString saveTreeToDir (const QString&,const QString&,const QPointF&,QList <Link*> &tmpLinks);// Save data recursivly to tempdir


    /*! \brief Sets filepath, filename and mapname

	 If the filepath is "/home/tux/map.xml", then the filename will be set
	 to map.xml. The destname is needed for vymLinks, pointing to another map. 
	 The destname holds the real name of the file, after it has been compressed, e.g. "map.vym"
    */	 


    /*! \brief Set File path

	 The destname is needed to construct the references between maps
    */	 
    void setFilePath (QString filepath,QString destname);   
    void setFilePath (QString);	//!< Overloaded for convenience
    QString getFilePath (); //!< Full path e.g. "/home/tux/map.xml"
    QString getFileName (); //!< e.g. "map.xml"
    QString getMapName ();  //!< e.g. "map"
    QString getDestPath (); //!< e.g. "/home/tux/map.vym"

    /*! \brief Load map

	The data is read from file. Depending on LoadMode the current
	selection gets replaced by data or the data is appended.
    */	
    ErrorCode loadMap  (
	QString,			//!< Path
	const LoadMode &lmode=NewMap,	//!< New map, replace or add to selection
	bool saveStateFlag=true,	//!< If called internally for history, don't save
	const FileType &ftype=VymMap,	//!< VymMap or FreeMind
	int pos=-1			//!< Optionally tell position where to add data
    );	

public:
    /*! \brief Save the map to file */
    ErrorCode save(const SaveMode &);	

private:
    ImageItem* loadFloatImageInt (BranchItem *dst,QString);
    void saveFloatImageInt (ImageItem*, const QString &, const QString &);
public:	
    void loadFloatImage ();
    void saveFloatImage ();

private:    
    void importDirInt(BranchItem *,QDir);
    void importDirInt(const QString&);
public:	
    void importDir();

private slots:
    void autosave ();
    void fileChanged();

////////////////////////////////////////////
// history (undo/redo)
////////////////////////////////////////////
private:
    bool mapDefault;		//!< Flag if map is untouched
    bool mapChanged;		//!< Flag if undo is possible
    bool mapUnsaved;		//!< Flag if map should be saved

    QString histPath;		//!< Path to history file
    SimpleSettings undoSet;	//!< undo/redo commands, saved in histPath
    int stepsTotal;		//!< total number of steps (undos+redos) 
    int curStep;		//!< Current step in history (ring buffer)
    int curClipboard;		//!< number of history step, which is the current clipboard
    int redosAvail;		//!< Available number of redo steps
    int undosAvail;		//!< Available number of undo steps
    bool blockReposition;	//!< block while load or undo
    bool blockSaveState;	//!< block while load or undo
public:
    bool isDefault();		//!< true, if map is still the empty default map
    void makeDefault();		//!< Reset changelog, declare this as default map
    bool hasChanged()	;	//!< true, if something has changed and is not saved yet
    void setChanged();		//!< called from TextEditor via LinkableMapObj

    /*! \brief Get name of object
      
      Returns heading of a branch or name of an object for use in comment
      of undo/redo history
    */ 
    QString getObjectName(LinkableMapObj*); 
    QString getObjectName(TreeItem*);	

    void redo();			//!< Redo last action
    bool isRedoAvailable();		//!< True, if redo is available
    void undo();			//!< Undo last action
    bool isUndoAvailable();		//!< True, if undo is available
    void gotoHistoryStep (int);		//!< Goto a specifig step in history


    QString getHistoryPath();		//!< Path to directory containing the history
    void resetHistory();		//!< Initialize history

    /*! \brief Save the current changes in map 

	Two commands and selections are saved:

	    - undocommand and undoselection to undo the change
	    - redocommand and redoselection to redo the action after an undo

	Additionally a comment is logged. 

    */	
    void saveState(
	const SaveMode& savemode, 
	const QString &undoSelection, 
	const QString &undoCommand, 
	const QString &redoSelection, 
	const QString &redoCommand, 
	const QString &comment, 
	TreeItem *saveSelection);

    /*! Overloaded for convenience */
    void saveStateChangingPart(
	TreeItem *undoSelection, 
	TreeItem* redoSelection, 
	const QString &redoCommand, 
	const QString &comment);

    /*! Overloaded for convenience */
    void saveStateRemovingPart(
	TreeItem *redoSelection, 
	const QString &comment);

    /*! Overloaded for convenience */
    void saveState(
	TreeItem *undoSelection, 
	const QString &undoCommand, 
	TreeItem *redoSelection, 
	const QString &redoCommand, 
	const QString &comment); 

    /*! Overloaded for convenience */
    void saveState(
	const QString &undoSelection, 
	const QString &undoCommand, 
	const QString &redoSelection, 
	const QString &redoCommand, 
	const QString &comment) ;

    /*! Overloaded for convenience */
    void saveState(
	const QString &undoCommand, 
	const QString &redoCommand, 
	const QString &comment) ;

    /*! Save a change in string and merge
	minor sequential  changes  */
    void saveStateMinimal(
	TreeItem *undoSelection, 
	const QString &undoCommand, 
	TreeItem *redoSelection, 
	const QString &redoCommand, 
	const QString &comment); 
	


////////////////////////////////////////////
// unsorted so far
////////////////////////////////////////////
public:
    QGraphicsScene *getScene();

    TreeItem* findBySelectString (QString s);	    
    TreeItem* findID (const uint &i);	    // find MapObj by unique ID


////////////////////////////////////////////
// Interface 
////////////////////////////////////////////
public:
    void setVersion(const  QString &);
    QString getVersion();
    void setAuthor  (const QString &);
    QString getAuthor ();
    void setComment (const QString &);
    QString getComment ();
    QString getDate();
    int branchCount();

    void setSortFilter (const QString &);
    QString getSortFilter ();
protected:  
    QString sortFilter;
signals:
    void sortFilterChanged (QString );	    //!< Notify editors of new filter

public:
    void setHeading(const QString &);	    //!< Set heading of item	
    QString getHeading ();		    //!< Get heading of item
    void setNote(const QString &s);	    //!< Set note text
    QString getNote();			    //!< Get note text
    void loadNote (const QString &fn);	    //!< Load note from file
    void saveNote (const QString &fn);	    //!< Save note to file

private:
    BranchItem* findCurrent;		    // next object in find process
    BranchItem* findPrevious;		    // next object in find process
    bool EOFind;			    // true, if search failed

public:
    void findDuplicateURLs();		    // find duplicate URLs, testing only so far
    bool findAll (FindResultModel*, QString s, Qt::CaseSensitivity cs);	// Search all objects at once, also notes
    BranchItem* findText(QString s,Qt::CaseSensitivity cs); // Find object, also in note
    void findReset();			    // Reset Search
private:    
    QString findString;

public:
    void setURL(const QString &url);
    QString getURL();			    // returns URL of selection or ""
    QStringList getURLs(bool ignoreScrolled=true);  // returns URLs of subtree


    void setFrameType(const FrameObj::FrameType &);
    void setFrameType(const QString &);
    void setFrameIncludeChildren (bool);
    void setFramePenColor (const QColor &);
    void setFrameBrushColor (const QColor &);
    void setFramePadding (const int &);
    void setFrameBorderWidth (const int &);
    void setIncludeImagesVer(bool);
    void setIncludeImagesHor(bool);
    void setHideLinkUnselected (bool);

    /*! Should object be hidden in exports (clouded)? */
    void setHideExport(bool);		

    /*! Should object be hidden in exports (clouded)? */
    void toggleHideExport();	    

    void addTimestamp();	

    void copy();			//!< Copy to clipboard
private:    
    void pasteNoSave(const int &n);	//!< paste clipboard to branch
public:	
    void paste();	    //!< Paste clipboard to branch and backup
    void cut();		    //!< Cut to clipboard (and copy)

    bool moveUp(BranchItem *bi);    //!< Move branch up without saving state
    void moveUp();		    //!< Move branch up with saving state
    bool moveDown(BranchItem *bi);  //!< Move branch down without saving state
    void moveDown();	    //!< Move branch down
    void detach();		    //!< Detach branch and use as new mapcenter
    void sortChildren(bool inverse=false);  //!< Sort children lexically

    // The create methods are used to quickly parse a XML file
    BranchItem* createMapCenter();		//!< Create MapCenter 
    BranchItem* createBranch(BranchItem *dst);	//!< Create Branch
    ImageItem* createImage(BranchItem *dst);	//!< Create image

public:	
    bool createLink(Link *l, bool createMO=false);	    //!< Create XLink 

    AttributeItem* addAttribute();
    AttributeItem* addAttribute(BranchItem *dst, AttributeItem* );

    /*! \brief Add new mapcenter

	Disclaimer: Still experimental, not fully supported yet.
    */	
    BranchItem* addMapCenter(bool saveStateFlag=true);
private:    
    BranchItem* addMapCenter(QPointF absPos);

    /*! \brief Add new branch

	Depending on num the new branch is created

	-3 above selection as child of selections parent
	-2 as child of selection
	-1 below selection as child of selections parent
	0..n	insert at a specific position in selections parent
	(needed for free relinking)
    */	

private:    
    BranchItem* addNewBranchInt(BranchItem *dst, int pos);  // pos allows to add above/below selection
public:	
    /*! \Add new branch
	
	Depending on num the new branch is created
	-1 above selection
	 0 as child of selection
	 1 below selection
    */
    BranchItem* addNewBranch(int pos=0);	
    BranchItem* addNewBranchBefore();	    //!< Insert branch between selection and its parent
    /*! \brief Relink a branch to a new destination dst 
	Relinks branch to dst at branch position pos. There is no saveState
	here, as for example moveUp or moving in MapEditor have
	different needs to call saveState
	Returns true if relinking was successful.
    */	
    bool relinkBranch (BranchItem* branch, BranchItem* dst, int pos =-1, bool updateSelection=false);   
    bool relinkImage  (ImageItem* image, BranchItem* dst);  

private:
    bool cleaningUpLinks;		//!< True while cleaning up to avoid recursion
public:	
   void cleanupItems();		//!< Delete orphaned Items
    void deleteLater (uint);		//!< Delete later with new beginRemoveRow
    void deleteSelection();		//!< Delete selection
    void deleteKeepChildren(bool saveStateFlag=true);	//!< remove branch, but keep children
public:	
    void deleteChildren();		//!< keep branch, but remove children

    TreeItem* deleteItem(TreeItem*);	//!< Delete item and return parent (if parent!= rootItem)
    void deleteLink (Link*);		//!< Remove Link and related LinkItems in TreeModel
    void clearItem (TreeItem* ti);	//!< Remove all children of TreeItem ti
    bool scrollBranch(BranchItem *);
    bool unscrollBranch(BranchItem *);
    void toggleScroll();
    void unscrollChildren();
    void setScale (qreal,qreal);
    void growSelectionSize();
    void shrinkSelectionSize();
    void resetSelectionSize();
    void emitExpandAll();
    void emitExpandOneLevel();
    void emitCollapseOneLevel();
    void emitCollapseUnselected();
signals:    
    void expandAll();
    void expandOneLevel();
    void collapseOneLevel();
    void collapseUnselected();

public:	
    void toggleTarget();
    ItemList getTargets();

    void toggleStandardFlag (const QString &name, FlagRow *master=NULL);
    void addFloatImage(const QImage &img);

    void colorBranch(QColor);
    void colorSubtree(QColor);
    QColor getCurrentHeadingColor();


    void note2URLs();			    // get URLs from note
    void editURL();			    // edit URL
    void editLocalURL();		    // edit URL to local file
    void editHeading2URL();		    // copy heading to URL
    void editBugzilla2URL();		    // create URL to Bugzilla
    void getBugzillaData(bool subtree);	    // get data from Novell Bugzilla
    void editFATE2URL();		    // create URL to FATE
    void editVymLink();			    // edit link to another map
    void setVymLink (const QString &);	// Set vymLink for selection
    void deleteVymLink();		    // delete link to another map
    QString getVymLink();		    // return path to map
    QStringList getVymLinks();		    // return paths in subtree
    void followXLink (int);
    void editXLink (int i=0);




////////////////////////////////////////////
// Scripting
////////////////////////////////////////////
public:	

    /* \brief Process one command and its parameters */
    QVariant parseAtom (const QString &atom, bool &noError, QString &errorMsg);	

    /* \brief Runs the script */
    QVariant runScript (const QString &script);

private:
    Parser parser;

////////////////////////////////////////////
// Exports
////////////////////////////////////////////
private:
    TreeItem::HideTmpMode hidemode; // true while exporting to hide some stuff

public:
    /*! Set or unset temporary hiding of objects during export  */
    void setExportMode (bool);

    /*! Save as image. Returns offset to upper left corner of image */
    QPointF exportImage (QString fname="",bool askForName=true,QString format="PNG");

    /*! Save as PDF  . Returns offset to upper left corner of image */
    QPointF exportPDF (QString fname="",bool askForName=true);

    /*! Save as SVG  . Returns offset to upper left corner of image */
    QPointF exportSVG (QString fname="",bool askForName=true);

    /*! Export as XTML to directory */
    void exportXML(QString dir="", bool askForName=true);

    /*! Export as A&O report text to file */
    void exportAO (QString fname="",bool askForName=true);  

    /*! Export as ASCII text to file */
    void exportASCII (QString fname="",bool askForName=true);  

    /*! Export as HTML to directory */
    void exportHTML(const QString& dir="", bool useDialog=true);    

    /*! Export as OpenOfficeOrg presentation */
    void exportOOPresentation(const QString &,const QString &);	

    /*! Returns if Export in recently used format is possible*/
    bool exportLastAvailable(QString &description, QString &command, QString &path);

    /*! Export in recently used format (saved in map)*/
    void exportLast();


////////////////////////////////////////////
// View related
////////////////////////////////////////////
public:
    void registerEditor (QWidget *);
    void unregisterEditor (QWidget *);

    void setMapZoomFactor (const double &); 
private: 
    double zoomFactor;
    QPointF contextPos;			//!< local position during context menu
public:
    void setContextPos (QPointF);	//!< local position during context menu
    void unsetContextPos ();		//!< forget local position after context menu

    void updateNoteFlag();		//!< Signal origination in TextEditor
    void reposition();			//!< Call reposition for all MCOs
    void setHideTmpMode (TreeItem::HideTmpMode mode);	

    void emitNoteHasChanged  (TreeItem *ti);
    void emitDataHasChanged  (TreeItem *ti);
    void emitUpdateQueries   ();       //!< tell MainWindow to update find results...
    void emitUpdateLayout ();

signals:
    void updateQueries(VymModel *m);
    void updateLayout();
    void noteHasChanged (QModelIndex ix);
    void newChildObject(QModelIndex ix);

private:
    MapEditor *mapEditor;

    QColor defLinkColor;	// default color for links
    QColor defXLinkColor;	// default color for xlinks
    int defXLinkWidth;		// default width for xlinks
    LinkableMapObj::ColorHint linkcolorhint;// use heading color or own color
    LinkableMapObj::Style linkstyle;	    // default style for links

public:
    void setMapLinkStyle (const QString &); // Set style of link
    LinkableMapObj::Style getMapLinkStyle ();	// requested in LMO
    void setMapDefLinkColor(QColor);	    // default color of links
    void setMapLinkColorHintInt();	    // color of links
    void setMapLinkColorHint(LinkableMapObj::ColorHint);// color of links
    void toggleMapLinkColorHint();	    // after changing linkStyles
    void selectMapBackgroundImage();
    void setMapBackgroundImage(const QString &);
    void selectMapBackgroundColor();
    void setMapBackgroundColor(QColor);
    QColor getMapBackgroundColor();


    LinkableMapObj::ColorHint getMapLinkColorHint();
    QColor getMapDefLinkColor();
    void setMapDefXLinkColor(QColor);
    QColor getMapDefXLinkColor();
    void setMapDefXLinkWidth (int);
    int getMapDefXLinkWidth();

    /*!  Move absolutly to (x,y).  */	
    void move    (const double &x, const double &y);

    /*!  Move relativly to (x,y).  */	
    void moveRel (const double &x, const double &y);

////////////////////////////////////////////
// Animation  **experimental**
////////////////////////////////////////////
private:    
    QTimer *animationTimer;
    bool animationUse;
    uint animationTicks;
    uint animationInterval;
    int timerId;		// animation timer
    QList <MapObj*> animObjList;// list with animated objects 

private slots:
    void animate();			//!< Called by timer to animate stuff
public:
    void startAnimation(BranchObj *bo, const QPointF &v);
    void startAnimation(BranchObj *bo, const QPointF &start, const QPointF &dest);
    void stopAnimation(MapObj *mo);
    void stopAllAnimation();

////////////////////////////////////////////
// Network related 
////////////////////////////////////////////
public:
    /*! \brief Networking states
	
	In Network modus we want to switch of saveState, autosave, ...
    */
    enum NetState {
	Offline,	    //!< Offline
	Client,		    //!< I am the client and connected to server
	Server		    //!< I am the server
    };

private:
    // Network connections **Experimental**
    NetState netstate;		// offline, client, server
    QTcpServer *tcpServer;	// Act as server in conference mode (experimental)
    QList <QTcpSocket*> clientList;	// List of connected clients
    quint16 sendCounter;	// Increased with every sent command

    QTcpSocket	*clientSocket;	// socket of this client
    QString server;		// server address of this client
    int port;			// server port of this client



protected:
    void sendSelection();

public:
    void newServer();
    void connectToServer();

private slots:	
    void newClient();
    void sendData(const QString &s);
    void readData();
    void displayNetworkError (QAbstractSocket::SocketError);

////////////////////////////////////////////
// DBUS related 
////////////////////////////////////////////

private:
AdaptorModel *adaptorModel; //FIXME-3

public slots:
    void testslot();

////////////////////////////////////////////
// Selection related 
////////////////////////////////////////////
private:
    TreeItem *latestAddedItem;		    // latest added object, reset on setChanged()

public:
    void setSelectionModel(QItemSelectionModel *);	// Set common selectionModel
    QItemSelectionModel* getSelectionModel();

    void setSelectionBlocked(bool);
    bool isSelectionBlocked();

    bool select ();			    //! select by using common QItemSelectionModel
    bool select (const QString &);	    //! Select by string
    bool select (LinkableMapObj *lmo);	    //! Select by pointer to LMO
    bool select (TreeItem *ti );	    //! Select by pointer to TreeItem
    bool select (TreeItem *ti,int i );	    //! Select by pointer to TreeItem and index in note
    bool select (const QModelIndex &index); //! Select by ModelIndex
    void unselect();
    bool reselect();

    void emitShowSelection();		    //!< Show selection in all views
signals:
    void showSelection();

public:	
    bool selectFirstBranch();
    bool selectLastBranch();
    bool selectLastSelectedBranch();
    bool selectLastImage();
    bool selectParent();

public:
    TreeItem::Type selectionType();
    LinkableMapObj* getSelectedLMO();
    BranchObj* getSelectedBranchObj();	
    BranchItem* getSelectedBranch();
    ImageItem* getSelectedImage();
    AttributeItem* getSelectedAttribute();
    TreeItem* getSelectedItem();
    QModelIndex getSelectedIndex();
    QString getSelectString ();
    QString getSelectString (LinkableMapObj *lmo);
    QString getSelectString (TreeItem *item);
    QString getSelectString (BranchItem *item);
    
    
signals:
    void selectionChanged(const QItemSelection &newsel, const QItemSelection &oldsel);

public:
    void emitSelectionChanged(const QItemSelection &oldsel);
    void emitSelectionChanged();
    void selectMapLinkColor();
    void selectMapSelectionColor();
private:    
    void setSelectionColorInt(QColor);
    QItemSelectionModel *selModel;
    QString lastSelectString;
    bool selectionBlocked;	//! Used to block changes of selection while editing a heading

public:	
    void setSelectionColor(QColor);
    QColor getSelectionColor();
};

#endif