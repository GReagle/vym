#include "noteobj.h"
#include "misc.h"

#include <QRegExp>
#include <QDebug>
#include <QTextDocument>    // for mightBeRichText

/////////////////////////////////////////////////////////////////
// NoteObj
/////////////////////////////////////////////////////////////////

NoteObj::NoteObj()  //FIXME-3 transform this to general "Text" Baseclass for both plain and richtext
{
    clear();
}

void NoteObj::operator= (const NoteObj &other)
{
    copy (other);
}

void NoteObj::copy (NoteObj other)
{
    note=other.note;
    fonthint=other.fonthint;
    filenamehint=other.filenamehint;
    richText = other.richText;
}

void NoteObj::clear()
{
    note = "";
    fonthint = "undef";
    filenamehint = "";
    richText = false;
}

void NoteObj::setRichText(bool b)
{
    richText = b;
    richText = true;
}

bool NoteObj::isRichText()const
{
    return richText;
}

void NoteObj::setText (const QString &s)
{
    note = s;
}

void NoteObj::setNoteRichText (const QString &s)
{
    qDebug()<<"NO::setNoteRT: "<<s;
    note = s;
    richText = true;
}

void NoteObj::setNotePlain (const QString &s)
{
    qDebug()<<"NO::setNotePlain: "<<s;
    note = unquotemeta(s);
    richText = false;
}

QString NoteObj::getText() const
{
    return note;
}

QString NoteObj::getNoteASCII()
{
    return getNoteASCII ("",80);
}

QString NoteObj::getNoteASCII(QString indent, const int &)  //FIXME-3 use width
{
    if (note.isEmpty()) return note;

    QString r=note;
    QRegExp rx;
    rx.setMinimal(true);

    if (!isRichText()) 
    {
        rx.setPattern("^");
        r=r.replace (rx,indent);
        rx.setPattern("\n");
        return r.replace (rx,"\n"+indent);
    }

    r = richTextToPlain( r );

    // Indent everything
    rx.setPattern ("^\n");
    r.replace (rx,indent);
    r=indent + r;   // Don't forget first line

/* FIXME-3  wrap text at width
    if (fonthint !="fixed")
    {
    }
*/
    r=indent+"\n"+r+indent+"\n\n";
    return r;
}

QString NoteObj::getNoteOpenDoc()
{
    // Evil hack to transform QT Richtext into
    // something which can be used in OpenDoc format
    // 
    // TODO create clean XML transformation which also
    // considers fonts, colors, ...

    QString r=note;

    // Remove header
    QRegExp re("<head>.*</head>");
    re.setMinimal(true);
    r.replace (re,"");

    // convert all "<br*>"
    re.setPattern ("<br.*>");
    re.setMinimal(true);
    r.replace (re,"<text:line-break/>");

    // convert all "<p>" 
    re.setPattern ("<p>");
    r.replace (re,"<text:line-break/>");
    
    // Remove all other tags, e.g. paragraphs will be added in 
    // templates used during export
    re.setPattern ("</?html.*>");
    r.replace (re,"");
    re.setPattern ("</?body.*>");
    r.replace (re,"");
    re.setPattern ("</?meta.*>");
    r.replace (re,"");
    re.setPattern ("</?span.*>");
    r.replace (re,"");
    re.setPattern ("</?p.*>");
    r.replace (re,"");

    r="<text:span text:style-name=\"vym-notestyle\">"+r+"</text:span>";
    return r;
}

void NoteObj::setFontHint (const QString &s)
{
    // only for backward compatibility (pre 1.5 )
    fonthint=s;
}

QString NoteObj::getFontHint() const
{
    // only for backward compatibility (pre 1.5 )
    return fonthint;
}

void NoteObj::setFilenameHint (const QString &s)
{
    filenamehint=s;
}

QString NoteObj::getFilenameHint() const
{
    return filenamehint;
}

bool NoteObj::isEmpty ()
{
    return note.isEmpty();
}

QString NoteObj::saveToDir ()
{
    qDebug()<<"NO::saveToDir rt="<<isRichText(); //FIXME-0
    if (richText )
    {
        QString n=note;

        // Remove the doctype, which will confuse parsing
        // with XmlReader in Qt >= 4.4
        QRegExp rx("<!DOCTYPE.*>");
        rx.setMinimal(true);
        n.replace (rx,"");

        // QTextEdit may generate fontnames with unquoted &, like
        // in "Lucida B&H". This is invalid in XML and thus would crash
        // the XML parser

        // More invalid XML is generated with bullet lists:
        // There are 2 <style> tags in one <li>, so we merge them here
        int pos=0;
        bool inbracket=false;
        int begin_bracket=0;
        bool inquot=false;

        while (pos<n.length())
        {
            if (n.mid(pos,1)=="<")
            {
                inbracket=true;
                begin_bracket=pos;
            }
            if (n.mid(pos,1)==">")
            {
                inbracket=false;
                QString s=n.mid(begin_bracket,pos-begin_bracket+1);
                int sl=s.length();
                if (s.count("style=\"")>1)
                {
                    rx.setPattern ("style=\\s*\"(.*)\"\\s*style=\\s*\"(.*)\"");
                    s.replace(rx,"style=\"\\1 \\2\"");
                    n.replace (begin_bracket,sl,s);
                    pos=pos-(sl-s.length());
                }
            }
            if (n.mid(pos,1)=="\"" && inbracket)
            {
                if (!inquot)
                    inquot=true;
                else
                    inquot=false;
            }
            if (n.mid(pos,1)=="&" && inquot)
            {
                // Now we are inside  <  "  "  >
                n.replace(pos,1,"&amp;");
                pos=pos+3;
            }
            pos++;
        }

        return beginElement ("vymnote",attribut("fonthint",fonthint)) +
                n+
                "\n" +
                endElement ("vymnote");
    } else
        return valueElement("vymnote", quotemeta(note), attribut("fonthint",fonthint));
}

