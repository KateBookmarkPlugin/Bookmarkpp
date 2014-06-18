// Own includes
#include "bookmarkpp.h"
 
// Include the basics
#include <ktexteditor/document.h>
#include <ktexteditor/markinterface.h>

#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <klocale.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kdatetime.h>
#include <iostream>
#include <typeinfo>
#include <QInputDialog>
#include <QPair>
#include <map>
// This macro defines a KPluginFactory subclass named BookmarkPlusPlusFactory. The second
// argument to the macro is code that is inserted into the constructor of the class.
// I our case all we need to do is register one plugin. If you want to have more
// than one plugin in the same library then you can register multiple plugin classes
// here. The registerPlugin function takes an optional QString parameter which is a
// keyword to uniquely identify the plugin then (it maps to X-KDE-PluginKeyword in the
// .desktop file).
K_PLUGIN_FACTORY(BookmarkPlusPlusFactory,
                 registerPlugin<BookmarkPlusPlus>();
                )
 
// With the next macro call, the library exports version information about the
// Qt and KDE libraries being used and (most important) the entry symbol to get at
// the factory we defined above.
// The argument this macro takes is the constructor call of the above factory which
// provides two constructors. One which takes a KAboutData* and another one
// that takes two (optional) const char* parameters (Same as for KComponentData
// constructors).
// We put there the X-KDE-LibraryName.
// Is important to provide as last parameter "ktexteditor_plugins".
K_EXPORT_PLUGIN(BookmarkPlusPlusFactory("bookmarkpp", "ktexteditor_plugins"))
 
// Constructor
BookmarkPlusPlus::BookmarkPlusPlus(QObject *parent, const QVariantList &args)
    : KTextEditor::Plugin(parent)
{
    qDebug()<<"BookmarkPlusPlus::BookmarkPlusPlus()\n";
    // Avoid warning on compile time because of unused argument
    Q_UNUSED(args);
    m_bookmarks=new BookmarkMap();
}


// Destructor
BookmarkPlusPlus::~BookmarkPlusPlus()
{
  delete m_bookmarks;
}
 
// Create the plugin view class and add it to the views list
void BookmarkPlusPlus::addView(KTextEditor::View *view)
{
  qDebug()<<"BookmarkPlusPlus::addView(url="<<view->document()->url()<<")\n";
    BookmarkPlusPlusView *nview = new BookmarkPlusPlusView(view,m_bookmarks,this);
    m_views.append(nview);
    connect(view->document(),SIGNAL(documentUrlChanged(view->document())),nview,SLOT(slotDocumentUrlChanged()));
}
 
// Find the view where we want to remove the plugin from, and remove it.
// Do not forget to free the memory.
void BookmarkPlusPlus::removeView(KTextEditor::View *view)
{
  qDebug()<<"BookmarkPlusPlus::removeView(->doc()->name="<<view->document()->url()<<")\n";
    for (int z = 0; z < m_views.size(); z++)
    {
        if (m_views.at(z)->parentClient() == view)
        {
            BookmarkPlusPlusView *nview = m_views.at(z);
            m_views.removeAll(nview);
            delete nview;
        }
    }
}
// Add the document to documents
void BookmarkPlusPlus::addDocument(KTextEditor::Document *doc)
{
  qDebug()<<"BookmarkPlusPlus::addDocument(name="<<doc->documentName()<<",url="<<doc->url()<<")\n";
    m_bookmarks->addDocument(doc);
    m_docs.append(doc);
    readConfig(doc);
}
 
// Remove a document from documents list
void BookmarkPlusPlus::removeDocument(KTextEditor::Document *doc)
{
    qDebug()<<"BookmarkPlusPlus::removeDocument(name="<<doc->documentName()<<",url"<<doc->url()<<")\n";
    m_bookmarks->removeDocument(doc);
    for (int z = 0; z < m_docs.size(); z++)
    {
        if (m_docs[z] == doc)
        {
            m_docs.removeAt(z);
        }
    }
}

void BookmarkPlusPlus::readConfig(KTextEditor::Document* doc)
{
  qDebug()<<"BookmarkPlusPlus::readConfig(name="<<doc->documentName()<<",url="<<doc->url()<<")\n";
  KConfigGroup cg(KGlobal::config(), "BookmarkPlusPlus");
  QVariantList serializedData=cg.readEntry(doc->url().prettyUrl(), QVariantList());
  if(serializedData.isEmpty())
  {
    //TODO sta ako nema
    qDebug()<<"ne valja upis ili nema";
    return;
  }
  m_bookmarks->m_docmap[doc]->deserialize(&serializedData);  
}

void BookmarkPlusPlus::writeConfig(KTextEditor::Document* doc)
{
  qDebug()<<"BookmarkPlusPlus::writeConfig(name="<<doc->documentName()<<",url="<<doc->url()<<")\n";
  KConfigGroup cg(KGlobal::config(), "BookmarkPlusPlus" );
  cg.writeEntry(doc->url().prettyUrl(), *(m_bookmarks->m_docmap[doc]->serialize()));
}
//------------------------------------------------------------------------
// Plugin view class
//------------------------------------------------------------------------
BookmarkPlusPlusView::BookmarkPlusPlusView(KTextEditor::View *view,BookmarkMap* books,BookmarkPlusPlus* plugin)
  : QObject(view)
  , KXMLGUIClient(view)
  , m_view(view)
  , m_books(books)
  , m_parent(plugin)
{
    //debug
    qDebug()<<"BookmarkPlusPlusView::BookmarkPlusPlusView(view->doc->name="<<view->document()->documentName()<<
    ",url="<<view->document()->url()<<")\n";
  
    setComponentData(BookmarkPlusPlusFactory::componentData());
  
    // Here we need as first parameter the same we declared at the resource
    // contents file (ui.rc). We named the action "tools_insert_timedate".
    // Here is where we connect it to an actual KDE action.
   
    KAction *action = new KAction(i18n("Dummyyyyy"), this);
    actionCollection()->addAction("tools_insert_timedate", action);
//     action->setShortcut(Qt::CTRL + Qt::Key_D);
    // As usual, we connect the signal triggered() to a slot here. When the menu
    // element is clicked, we go to the slot slotInsertTimeDate().
    connect(action, SIGNAL(triggered()), this, SLOT(slotInsertTimeDate()));
    
    //proba
    connect(view->document(),SIGNAL(marksChanged(KTextEditor::Document*)),
            this,SLOT(slotInsertTimeDate()));
    //set bookmark
    KAction *setBookmarkAction = new KAction(i18n("Set Bookmark"), this);
    actionCollection()->addAction("tools_set_bookmark",setBookmarkAction);
    setBookmarkAction->setShortcut(Qt::CTRL +Qt::ALT+Qt::Key_B);
    connect(setBookmarkAction,SIGNAL(triggered()),this,SLOT(slotSetBookmark()));
    
    
    // This is always needed, tell the KDE XML GUI client that we are using
    // that file for reading actions from.
    setXMLFile("ui.rc");
}
 
// Destructor
BookmarkPlusPlusView::~BookmarkPlusPlusView()
{
  qDebug()<<"BookmarkPlusPlusView::~BookmarkPlusPlusView()\n";
}


void BookmarkPlusPlusView::slotSetBookmark()
{
  qDebug()<<"BookmarkPlusPlusView::slotSetBookmark()";
  bool ok;
  QString text=QInputDialog::getText(NULL,QString("Set Bookmark"),
      QString("Bookmark name:"),QLineEdit::Normal,
      QString("name"),&ok,NULL);
  if(ok)
    qDebug()<<"\nback from the dialog...ok=true,text="<<text<<"\n";
  else
  {
    qDebug()<<"\n...ok=false, something wrong...";
    return;
  }
  m_books->addBookmark(m_view->document(),text,
                       m_view->cursorPosition().line());
  m_parent->writeConfig(m_view->document());
  
}
// The slot that will be called when the menu element "Dummy" is
// clicked.
void BookmarkPlusPlusView::slotInsertTimeDate()
{
    qDebug()<<"BookmarkPlusPlusView::slotInsertTimeDate()";
    qDebug()<<m_view->document()->url()<<" "<<m_books->getBookmarkNames(m_view->document());
    m_parent->readConfig(m_view->document());
    m_books->refresh(m_view->document());
//     std::cout<<typeid(m_view->document()).name()<<std::endl;
//     KTextEditor::MarkInterface* mi=qobject_cast
//       <KTextEditor::MarkInterface*>(m_view->document());
    QTextStream qout(stdout);
    qout<<"\n\nmarkdescription\n";
    KTextEditor::MarkInterface* marki= qobject_cast<KTextEditor::MarkInterface*>
    (m_view->document());
    qout<<marki->markDescription( KTextEditor::MarkInterface::markType21)<<"\n\n";
//     foreach(KTextEditor::Mark* a,mi->marks())
//     {
//       qout<<a->line<<" "<<a->type<<"\n";
//     }
    QList<QString> bukmarci=m_books->getBookmarkNames(m_view->document());
    foreach(QString s,bukmarci)
    {
      qout<<s<<"\n";
    }
    /// We insert the information in the document at the current cursor position
    // with the default string declared on the header.
    //int n=m_view->document()->lines();
    //QString stringa=QString(std::string(n,'*'));
    //std::cout<<n<<std::endl;
    //m_view->document()->insertText(m_view->cursorPosition(), stringa);
}

void BookmarkPlusPlusView::slotDocumentUrlChanged()
{
  qDebug()<<"BookmarkPlusPlusView::slotDocumentUrlChanged()";
}

void BookmarkPlusPlusView::slotMarksChanged()
{
  qDebug()<<"BookmarkPlusPlusView::slotMarksChanged()";
  m_parent->writeConfig(m_view->document());
}

// We need to include the moc file since we have declared slots and we are using
// the Q_OBJECT macro on the BookmarkPlusPlusView class.
#include "bookmarkpp.moc"