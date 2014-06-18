#include "BookmarkMap.h"
 
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

BookmarkMap::BookmarkMap()
{
}
BookmarkMap::~BookmarkMap()
{
}
void BookmarkMap::addDocument(KTextEditor::Document* doc)
{
  m_docmap[doc]=new DocBookmarkMap(doc);
  
}
void BookmarkMap::removeDocument(KTextEditor::Document* doc)
{
  m_docmap.remove(doc);
}
//TODO:ima description u MarkInterface
void BookmarkMap::addBookmark(KTextEditor::Document* doc,QString name,int line)
{
  m_docmap[doc]->addBookmark(name,line);
}
void BookmarkMap::removeBookmark(KTextEditor::Document* doc,QString name)
{
  m_docmap[doc]->removeBookmark(name);
}
//optional TODO: da ima opcija u config da li da moze da se brise bookmark sa
// ctrl+B ili samo sa nekom nasom skracenicom za brisanje bookmarka
void BookmarkMap::refresh(KTextEditor::Document *doc)
{
  m_docmap[doc]->refresh();
}
//return: -1 if a code doesn exist, line of named bookmark if it exists 
int BookmarkMap::getLineInDocument(KTextEditor::Document* doc,uint code)
{
  return m_docmap[doc]->getLineOfBookmark(code);
}
//return: -2 if a name doesn't exist, -1 if a name exist, but bookmark doesn't
//or line of named bookmark if it exists
int BookmarkMap::getLineInDocument(KTextEditor::Document* doc,QString name)
{
  return m_docmap[doc]->getLineOfBookmark(name);
}

QList<QString> BookmarkMap::getBookmarkNames(KTextEditor::Document* doc)
{
  return m_docmap[doc]->getBookmarkNames();
}

void BookmarkMap::serialize(KTextEditor::Document* doc)
{
  m_docmap[doc]->serialize();
}

//------------------------------------------------------------------------
//DocBookmarkMap
//------------------------------------------------------------------------

BookmarkMap::DocBookmarkMap::DocBookmarkMap(KTextEditor::Document* doc)
:m_doc(doc)
{
   max=1;
}
void BookmarkMap::DocBookmarkMap::addBookmark(QString name,int line)
{
      KTextEditor::MarkInterface* mi=qobject_cast
      <KTextEditor::MarkInterface*>(m_doc);
      uint code=((max)<<9)+1;
      //check if it already exists
      int mark=mi->mark(line);
      if(!mark)
      {
        code=((++max)<<9)+1;
        mi->addMark(line,code);
      }
      else//if it has a bookmark
      {
        foreach(KTextEditor::Mark* a, mi->marks())
        {
          if(a->line==line)
          {
            if(a->type==1)
            {
              code=((++max)<<9)+1;
              mi->addMark(line,code);
            }
            else
            {
              code=a->type;
            }
            break;
          }
        }
      }
      m_map[name]=code;
      //debug
      QTextStream qout(stdout);
      foreach(QString s, m_map.keys())
      {
        qout<<"radim:"<<s<<" "<<m_map[s];
      }
//    update of max value
      uint tmax=1;
      foreach(uint i, m_map.values())
      {
        uint shifted=(i>>9);
        tmax=tmax<shifted?shifted:tmax;
      }
      max=tmax;
      qout<<"\n";
}
void BookmarkMap::DocBookmarkMap::removeBookmark(QString name)
{
   m_map.remove(name);
}

void BookmarkMap::DocBookmarkMap::refresh()
{
      KTextEditor::MarkInterface* mi=qobject_cast
        <KTextEditor::MarkInterface*>(m_doc);
      QList<QString> pastKeys=m_map.keys();
      QMap < QString, uint > futureMap;
      foreach(KTextEditor::Mark* mark,mi->marks())
      {
        foreach(QString name, pastKeys)
        {
          if(mark->type==m_map[name])
          {
             futureMap[name]=mark->type;
          }
        }
      }
      m_map=futureMap;
}//refresh

int BookmarkMap::DocBookmarkMap::getLineOfBookmark(uint code)
{
      KTextEditor::MarkInterface* mi=qobject_cast
        <KTextEditor::MarkInterface*>(m_doc);
      foreach(KTextEditor::Mark* mark,mi->marks())
      {
        if(mark->type==code)
          return mark->line;
      }
      return -1;
}
int BookmarkMap::DocBookmarkMap::getLineOfBookmark(QString name)
{
      uint code=m_map[name];
      if(code==0)
        return -2;
      
      KTextEditor::MarkInterface* mi=qobject_cast
        <KTextEditor::MarkInterface*>(m_doc);
      foreach(KTextEditor::Mark* mark,mi->marks())
      {
        if(mark->type==code)
          return mark->line;
      }
      return -1;
}

QList<QString> BookmarkMap::DocBookmarkMap::getBookmarkNames()
{
   return m_map.keys();
}
QVariantList* BookmarkMap::DocBookmarkMap::serialize()
{
      QVariantList* serializationStrings=new QVariantList();
      qDebug()<<m_doc->documentName();
      qDebug()<<"ovde";
      QList<QString> keys=m_map.keys();
      foreach(QString s, keys)
      {
        qDebug()<<m_doc->documentName();
      }
      foreach(QString s,keys)
      {
        qDebug()<<"neki text";
        serializationStrings->push_back(s);
        serializationStrings->push_back(m_map[s]);
        qDebug()<<*serializationStrings;
      }
      qDebug()<<"onde";
      QList<QVariant>::iterator iter=serializationStrings->begin();
      QList<QVariant>::iterator end=serializationStrings->end();
      for(;iter!=end;iter++)
      {
        qDebug()<<"\nstampam:"<<*iter;
      }
      return serializationStrings;
      
}