// Avoid multiple header inclusion
#ifndef BOOKMARKMAP_H
#define BOOKMARKMAP_H
 
// Include the basics
#include <ktexteditor/plugin.h>
#include <ktexteditor/view.h>
#include <ktexteditor/document.h>
#include <ktexteditor/markinterface.h>
#include <kxmlguiclient.h>
#include <klocalizedstring.h>
#include <ktexteditor/sessionconfiginterface.h>
#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtCore/QList>
#include <KConfigGroup>


class BookmarkMap
{
public:
  BookmarkMap();
  ~BookmarkMap();
  void addDocument(KTextEditor::Document* doc);
  void removeDocument(KTextEditor::Document* doc);
  void addBookmark(KTextEditor::Document* doc,QString name,int line);
  void removeBookmark(KTextEditor::Document* doc,QString name);
  void refresh(KTextEditor::Document* doc);
  void serialize(KTextEditor::Document* doc);
  int getLineInDocument(KTextEditor::Document* doc,uint code);
  int getLineInDocument(KTextEditor::Document* doc,QString name);
  QList<QString> getBookmarkNames(KTextEditor::Document *doc);
  class DocBookmarkMap
  {
  public:
    DocBookmarkMap(KTextEditor::Document* doc);
    void addBookmark(QString name,int line);
    void removeBookmark(QString name);
    void refresh();
    int getLineOfBookmark(uint code);
    int getLineOfBookmark(QString name);
    QList<QString> getBookmarkNames();
    QVariantList* serialize();
  private:
    KTextEditor::Document * m_doc;
    QMap < QString, uint > m_map;
    //TODO:update maxa
    uint max;
  };
  QMap <KTextEditor::Document*,DocBookmarkMap*> m_docmap;
};
#endif // BOOKMARKMAP_H