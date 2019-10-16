/**************************************************************************/
/*!
 * @file mainwindow.cpp
 *
 * @mainpage QtArduSim AVR MCU and electronic circuit simulator
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's FXOS8700 driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit FXOS8700 breakout: https://www.adafruit.com/products/3463
 *
 * These sensors use I2C to communicate, 2 pins (SCL+SDA) are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 * This library depends on <a href="https://github.com/adafruit/Adafruit_Sensor">
 * Adafruit_Sensor</a> being present on your system. Please make sure you have
 * installed the latest version before using this library.
 *
 * @section author Santiago González
 *
 * Copyright (C) 2012 by Santiago González: santigoro@gmail.com
 *
 * @section license License
 *
 * See the GNU General Public License <http://www.gnu.org/licenses/> for more details.
 *
 */
 /***************************************************************************/

#include "mainwindow.h"
#include "appiface.h"
#include "circuit.h"
#include "propertieswidget.h"
#include "componentselector.h"
#include "circuitwidget.h"
#include "utils.h"
#include "simuapi_apppath.h"


MainWindow* MainWindow::m_pSelf = 0l;

MainWindow::MainWindow()
          : QMainWindow()
          , m_settings( "QtArduSim", "QtArduSim" )
{
    setWindowIcon( QIcon(":/qtardusim.png") );
    m_pSelf   = this;
    m_circuit = 0l;
    m_version = "QtArduSim-"+QString( APP_VERSION );
    
    this->setWindowTitle(m_version);

    QString userAddonPath = SIMUAPI_AppPath::self()->RWDataFolder().absoluteFilePath("addons");

    QDir pluginsDir( userAddonPath );

    if( !pluginsDir.exists() ) pluginsDir.mkpath( userAddonPath );

    m_fontScale = 1.0;
    if( m_settings.contains( "fontScale" ) ) 
    {
        m_fontScale = m_settings.value( "fontScale" ).toFloat();
        if( m_fontScale == 0 ) m_fontScale = 1;
    }
    else
    {
        double dpiX = qApp->desktop()->logicalDpiX();
        m_fontScale = dpiX/96.0;
    }
    //qDebug()<<dpiX;
    createWidgets();
    readSettings();
    
    loadPlugins();
}
MainWindow::~MainWindow(){ }

void MainWindow::closeEvent( QCloseEvent *event )
{
    if( !m_circuit->newCircuit()) { event->ignore(); return; }
    
    writeSettings();
    
    event->accept();
}

void MainWindow::readSettings()
{
    restoreGeometry(                     m_settings.value("geometry" ).toByteArray());
    restoreState(                        m_settings.value("windowState" ).toByteArray());
    m_Centralsplitter->restoreState(     m_settings.value("Centralsplitter/geometry").toByteArray());
}

void MainWindow::writeSettings()
{
    m_settings.setValue( "fontScale", m_fontScale );
    m_settings.setValue( "geometry", saveGeometry() );
    m_settings.setValue( "windowState", saveState() );
    m_settings.setValue( "Centralsplitter/geometry", m_Centralsplitter->saveState() );
    
    QList<QTreeWidgetItem*> list = m_components->findItems( "", Qt::MatchStartsWith | Qt::MatchRecursive );

    foreach( QTreeWidgetItem* item, list  )
        m_settings.setValue( item->text(0)+"/collapsed", !item->isExpanded() );
}

void MainWindow::setTitle( QString title )
{
    setWindowTitle(m_version+"  -  "+title);
}

void MainWindow::about()
{
   /*QMessageBox::about(this, tr("About Application"),
            tr("Circuit simulation"
               "and IDE for mcu development"));*/
}

void MainWindow::createWidgets()
{
    QWidget *centralWidget = new QWidget( this );
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);

    QGridLayout *baseWidgetLayout = new QGridLayout( centralWidget );
    baseWidgetLayout->setSpacing(0);
    baseWidgetLayout->setContentsMargins(0, 0, 0, 0);
    baseWidgetLayout->setObjectName("gridLayout");

    m_Centralsplitter = new QSplitter( this );
    m_Centralsplitter->setObjectName("Centralsplitter");
    m_Centralsplitter->setOrientation( Qt::Horizontal );

    m_sidepanel = new QTabWidget( this );
    m_sidepanel->setObjectName("sidepanel");
    m_sidepanel->setTabPosition( QTabWidget::West );
    QString fontSize = QString::number( int(11*m_fontScale) );
    m_sidepanel->tabBar()->setStyleSheet("QTabBar { font-size:"+fontSize+"px; }");
    m_Centralsplitter->addWidget( m_sidepanel );

    m_components = new ComponentSelector( m_sidepanel );
    m_components->setObjectName( "components" );
    m_sidepanel->addTab( m_components, tr("Components") );

    m_itemprop = new PropertiesWidget( this );
    m_itemprop->setObjectName( "properties" );
    m_sidepanel->addTab( m_itemprop,  tr( "Properties" ));

    m_circuit = new CircuitWidget( this );
    m_circuit->setObjectName( "circuit" );
    m_Centralsplitter->addWidget( m_circuit );

    baseWidgetLayout->addWidget( m_Centralsplitter, 0, 0 );

    QList<int> sizes;
    sizes << 150 << 350 << 500;
    m_Centralsplitter->setSizes( sizes );
}

void MainWindow::loadPlugins()
{
    // Load main Plugins
    QDir pluginsDir( qApp->applicationDirPath() );

    pluginsDir.cd( "../lib/qtardusim/plugins" );
    
    loadPluginsAt( pluginsDir );

    // Load main Component Sets
    QDir compSetDir = SIMUAPI_AppPath::self()->RODataFolder();

    if( compSetDir.exists() ) ComponentSelector::self()->LoadCompSetAt( compSetDir );

    // Load Addons
    QString userPluginsPath = SIMUAPI_AppPath::self()->RWDataFolder().absoluteFilePath("addons");
    
    pluginsDir.setPath( userPluginsPath );

    if( !pluginsDir.exists() ) return;

    foreach( QString pluginFolder, pluginsDir.entryList( QDir::Dirs ) )
    {
        if( pluginFolder.contains( "." ) ) continue;
        //qDebug() << pluginFolder;
        pluginsDir.cd( pluginFolder );

        ComponentSelector::self()->LoadCompSetAt( pluginsDir );

        if( pluginsDir.entryList( QDir::Dirs ).contains( "lib"))
        {
            pluginsDir.cd( "lib" );
            loadPluginsAt( pluginsDir );
            pluginsDir.cd( "../" );
        }
        pluginsDir.cd( "../" );
    }
}

void MainWindow::loadPluginsAt( QDir pluginsDir )
{
    QString pluginName = "*plugin";
    pluginName += ".so";

    pluginsDir.setNameFilters( QStringList(pluginName) );

    QStringList fileList = pluginsDir.entryList( QDir::Files );

    if( fileList.isEmpty() ) return;                                    // No plugins to load

    qDebug() << "\n    Loading Plugins at:\n"<<pluginsDir.absolutePath()<<"\n";

    foreach( QString libName, fileList )
    {
        pluginName = libName.split(".").first().remove("lib").remove("plugin").toUpper();
            
        if( m_plugins.contains(pluginName) ) continue;

        QPluginLoader* pluginLoader = new QPluginLoader( pluginsDir.absoluteFilePath( libName ) );
        QObject* plugin = pluginLoader->instance();

        if( plugin )
        {
            AppIface* item = qobject_cast<AppIface*>( plugin );

            if( item )
            {
                item->initialize();
                m_plugins[pluginName] = pluginLoader;
                qDebug()<< "        Plugin Loaded Successfully:\t" << pluginName;
            }
            else
            {
                pluginLoader->unload();
                delete pluginLoader;
            }
        }
        else
        {
            QString errorMsg = pluginLoader->errorString();
            qDebug()<< "        " << pluginName << "\tplugin FAILED: " << errorMsg;

            if( errorMsg.contains( "libQt5SerialPort" ) )
                errorMsg = tr( " Qt5SerialPort is not installed in your system\n\n    Mcu SerialPort will not work\n    Just Install libQt5SerialPort package\n    To have Mcu Serial Port Working" );

            QMessageBox::warning( 0,tr( "Plugin Error:" ), errorMsg );
        }
    }
    qDebug() << "\n";
}

void MainWindow::unLoadPugin( QString pluginName )
{
    if( m_plugins.contains( pluginName ) )
    {
        QPluginLoader* pluginLoader = m_plugins[pluginName];
        QObject* plugin = pluginLoader->instance();
        AppIface* item = qobject_cast<AppIface*>( plugin );
        item->terminate();
        pluginLoader->unload();
        m_plugins.remove( pluginName );
        delete pluginLoader;
    }
}

void MainWindow::applyStile()
{
    QFile file(":/qtardusim.qss");
    file.open(QFile::ReadOnly);

    m_styleSheet = QLatin1String(file.readAll());

    qApp->setStyleSheet( m_styleSheet );
}

QSettings* MainWindow::settings() { return &m_settings; }

#include  "moc_mainwindow.cpp"


