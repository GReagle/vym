#include <QApplication>
#include <QtDBus/QDBusConnection>

#include <iostream>
using namespace std;

#include "findwidget.h"
#include "findresultwidget.h"
#include "flagrow.h"
#include "flagrowobj.h"
#include "headingeditor.h"
#include "mainwindow.h"
#include "noteeditor.h"
#include "options.h"
#include "settings.h"
#include "version.h"

#if defined(Q_OS_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <sys/types.h>		// To retrieve PID for use in DBUS
#include <unistd.h>

QString vymName;
QString vymVersion;
QString vymBuildDate;
QString vymCodeName;
QString vymInstanceName;

bool bugzillaClientAvailable;	// openSUSE specific currently

HeadingEditor *headingEditor;	    
NoteEditor    *noteEditor;	// used in Constr. of LinkableMapObj
				// initialized in mainwindow
Main *mainWindow;		// used in BranchObj::select()				    
FindWidget *findWidget;
FindResultWidget *findResultWidget;

QDBusConnection dbusConnection= QDBusConnection::sessionBus();

QString tmpVymDir;		// All temp files go there, created in mainwindow
QString clipboardDir;		// Clipboard used in all mapEditors
QString clipboardFile;		// Clipboard used in all mapEditors
QDir vymBaseDir;		// Containing all styles, scripts, images, ...
QDir lastImageDir;
QDir lastFileDir;
#if defined(Q_OS_WIN32)
QDir vymInstallDir;
#endif
QString iconPath;		// Pointing to icons used for toolbars
QString flagsPath;		// Pointing to flags
bool clipboardEmpty;		
bool debug;			// global debugging flag
FlagRow *systemFlagsMaster; 
FlagRow *standardFlagsMaster;	

Settings settings ("InSilmaril","vym"); // Organization, Application name

Options options;
ImageIO imageIO;

int statusbarTime=10000;

int main(int argc, char* argv[])
{
    //Q_INIT_RESOURCE (application);

    QApplication app(argc,argv);

    vymName=__VYM_NAME;
    vymVersion=__VYM_VERSION;
    vymBuildDate=__VYM_BUILD_DATE;
    vymCodeName=__VYM_CODENAME;

    // Reading and initializing options commandline options
    options.add ("batch", Option::Switch, "b", "batch");
    options.add ("debug", Option::Switch, "d", "debug");
    options.add ("help", Option::Switch, "h", "help");
    options.add ("local", Option::Switch, "l", "local");
    options.add ("name", Option::String, "n", "name");
    options.add ("quit", Option::Switch, "q", "quit");
    options.add ("run", Option::String, "r", "run");
    options.add ("shortcuts", Option::Switch, "s", "shortcuts");
    options.add ("shortcutsLaTeX", Option::Switch, "sl", "shortcutsLaTeX");
    options.add ("version", Option::Switch, "v","version");
    options.setHelpText (
	"VYM - View Your Mind\n"
	"--------------------\n\n"
	"Information about vym can be found in vym.pdf,\n"
	"which should be part of the vym package.\n"
	"It is also available at the project homepage:\n\n"
	"http://www.InSilmaril.de/vym\n\n"
	"Usage: vym [OPTION]... [FILE]... \n"
	"Open FILEs with vym\n\n"
	"-b           batch       batch mode: hide windows\n"
	"-d           debug       Show debugging output\n"
	"-h           help        Show this help text\n"
	"-l           local       Run with ressources in current directory\n"
	"-n  STRING   name        Set name of instance for DBus access\n"
	"-q           quit        Quit immediatly after start for benchmarking\n"
	"-r  FILE     run         Run script\n"
	"-s           shortcuts   Show Keyboard shortcuts on start\n"
	"-sl          LaTeX       Show Keyboard shortcuts in LaTeX format on start\n"
	"-v           version     Show vym version\n"
    );
    if (options.parse())
    {
	cout << endl << qPrintable( options.getHelpText())<<endl;
	return 1;
    }

    debug=options.isOn ("debug");

    if (options.isOn ("version"))
    {
	cout << "VYM - View Your Mind (c) 2004-2010 Uwe Drechsel "  << endl
	    <<"   Version: "<<__VYM_VERSION <<endl
	    <<"Build date: "<<__VYM_BUILD_DATE << endl
	    <<"  "<<__VYM_CODENAME<<endl;
	    
	return 0;   
    }	    
    
    // Register for DBUS
    if (debug) cout << "PID="<<getpid()<<endl;
    QString pidString=QString ("%1").arg(getpid());
    if (!dbusConnection.registerService ("org.insilmaril.vym-"+pidString))
    {
       fprintf(stderr, "%s\n",
	    qPrintable(QDBusConnection::sessionBus().lastError().message()));        
        exit(1);
    }	

    if (options.isOn ("name"))
	vymInstanceName=options.getArg ("name");
    else
	vymInstanceName=pidString;
    

    // Use /usr/share/vym or /usr/local/share/vym or . ?
    // First try options
    if (options.isOn ("local"))
    {
	vymBaseDir.setPath (vymBaseDir.currentPath());
    } else
    // then look for environment variable
    if (getenv("VYMHOME")!=0)
    {
	vymBaseDir.setPath (getenv("VYMHOME"));
    } else
    // ok, let's find my way on my own
    {
	#if defined (Q_OS_MACX)
	    vymBaseDir.setPath(vymBaseDir.currentPath() +"/vym.app/Contents/Resources");

        #elif defined (Q_OS_WIN32)
            QString basePath;

            wchar_t wbuf[512];
            if (GetModuleFileName(NULL, wbuf, 512))
            {
                QString mfn(QString::fromWCharArray(wbuf));
                mfn.replace('\\', '/');
                if (mfn.endsWith("/bin/vym.exe", Qt::CaseInsensitive))
                {
                    mfn.chop(12);
                    basePath = mfn;
                }
            }

            if (basePath.isEmpty())
                basePath = vymBaseDir.currentDirPath();

            vymInstallDir.setPath(basePath);
            vymBaseDir.setPath(basePath + "/share/vym");

	#else
	    vymBaseDir.setPath ("/usr/share/vym");
	    if (!vymBaseDir.exists())
	    {
		vymBaseDir.setPath ("/usr/local/share/vym");
		if (!vymBaseDir.exists())
		    vymBaseDir.setPath(vymBaseDir.currentPath() );
	    }	    
	#endif
    }

    iconPath=vymBaseDir.path()+"/icons/";
    flagsPath=vymBaseDir.path()+"/flags/";

    // Some directories
    lastImageDir=QDir().current();
    lastFileDir=QDir().current();

    if (options.isOn ("help"))
    {
	cout << qPrintable (options.getHelpText())<<endl;
	return 0;   
    }	

    // Initialize translations
    QTranslator translator (0);
    //translator.load( QString("vym_")+QTextCodec::locale(), vymBaseDir.path() + "/lang");
    translator.load( QString("vym_")+QLocale().name(), vymBaseDir.path() + "/lang");
    app.installTranslator( &translator );

    // Initializing the master rows of flags
    systemFlagsMaster=new FlagRow;
    systemFlagsMaster->setName ("systemFlagsMaster");
    standardFlagsMaster=new FlagRow;
    standardFlagsMaster->setName ("standardFlagsMaster");

    // Initialize window of NoteEditor
    noteEditor = new NoteEditor();
    noteEditor->setWindowIcon (QPixmap (iconPath+"vym-editor.png"));

    headingEditor = new HeadingEditor();

    // Check if there is a BugzillaClient
    QFileInfo fi("/var/adm/perl-modules/perl-SUSE-BugzillaClient");
    bugzillaClientAvailable=fi.exists();

    // Initialize mainwindow 
#if defined(Q_OS_WIN32)
    Main m(0, Qt::Window | Qt::MSWindowsOwnDC);
#else
    Main m;
#endif

    m.setWindowIcon (QPixmap (iconPath+"vym.png"));
    m.fileNew();
    if (options.isOn ("batch"))
	m.hide();
    else	
    {
	// Paint Mainwindow first time
	qApp->processEvents();
	m.show();
    }

    m.loadCmdLine();

    // Run script
    if (options.isOn ("run"))
    {
	QString script;
	QString fn=options.getArg ("run");
	if ( !fn.isEmpty() )
	{
	    QFile f( fn );
	    if ( !f.open( QFile::ReadOnly|QFile::Text ) )
	    {
		QString error (QObject::tr("Error"));
		QString msg (QObject::tr("Couldn't open \"%1\"\n%2.").arg(fn).arg(f.errorString()));
		if (options.isOn("batch"))
		    qWarning ()<<error+": "+msg;
		else    
		    QMessageBox::warning(0, error,msg);
		return 0;
	    }	

	    QTextStream in( &f );
	    script= in.readAll();
	    f.close();
	    m.setScript (script);
	    m.runScriptEverywhere (script);
	}
    }	    
    
    // For benchmarking we may want to quit instead of entering event loop
    if (options.isOn ("quit")) return 0;

    // Enable some last minute cleanup
    QObject::connect( &app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );

    return app.exec();
}