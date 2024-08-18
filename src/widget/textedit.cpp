#include "textedit.h"
#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QDebug>
#include <QApplication>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QFocusEvent>
#include <QStringListModel>
#include "tokens.h"

#include "highlighter.h"
#include "queryparser.h"
#include <QStyle>
#include "completer.h"
#include <limits.h>
#include "emmet.h"
#include <QPainter>
#include <QPainterPath>
#include <emmet.h>

TextEdit::TextEdit(QWidget *parent)
: QPlainTextEdit(parent), mCompleter(nullptr), mHighlighter(nullptr)
{
    const QFont defaultFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);

#ifdef Q_OS_LINUX
    QFont font("Liberation Mono",11,QFont::Normal);
#endif

#ifdef Q_OS_WIN
    QFont font(defaultFont.defaultFamily(),10);
#endif

    setFont(font);

    connect(this,SIGNAL(textChanged()),this,SLOT(onTextChanged()));

    mCompleter = new Completer(this);
    mCompleter->setWidget(this);
    QObject::connect(mCompleter, SIGNAL(activated(QString)),
                     this, SLOT(insertCompletion(QString)));
}

TextEdit::~TextEdit()
{
    /*if (mCompleter) {
        mCompleter->deleteLater();
        mCompleter = nullptr;
    }*/
    if (mHighlighter) {
        mHighlighter->deleteLater();
        mHighlighter = nullptr;
    }
}


void TextEdit::setTokens(const Tokens& tokens) {
    //qDebug() << "setTokens";
    mTokens = tokens;
    Highlighter* highlighter = new Highlighter(tokens,0);
    setHighlighter(highlighter);
    updateCompleter();
}

Tokens TextEdit::tokens() const
{
    return mTokens;
}
#if 0
void TextEdit::setCompleter(QCompleter *completer)
{

    if (mCompleter) {
        //QObject::disconnect(mCompleter, 0, this, 0);
        mCompleter->deleteLater();
    }

    mCompleter = completer;

    if (!mCompleter) {
        return;
    }

    mCompleter->setWidget(this);
    mCompleter->setCompletionMode(QCompleter::PopupCompletion);
    mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(mCompleter, SIGNAL(activated(QString)),
                     this, SLOT(insertCompletion(QString)));

}


QCompleter *TextEdit::completer() const
{
    return mCompleter;
}
#endif


void TextEdit::setHighlighter(Highlighter *highlighter)
{
    if (mHighlighter) {
        mHighlighter->setDocument(0);
        mHighlighter->deleteLater();
    }

    mHighlighter = highlighter;
    mHighlighter->setDocument(document());
}

Highlighter *TextEdit::highlighter() const
{
    return mHighlighter;
}

void TextEdit::insertCompletion(const QString& completion)
{
    if (mCompleter->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - mCompleter->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString TextEdit::textUnderCursor() const
{
    QTextCursor tc = textCursor();

    // go to start of the word
    tc.movePosition(QTextCursor::StartOfWord);
    tc.movePosition(QTextCursor::PreviousCharacter);
    tc.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);

    // word starts with "."
    if (tc.selectedText() == ".") {
        tc.movePosition(QTextCursor::PreviousCharacter);
        tc.movePosition(QTextCursor::PreviousCharacter);
        tc.movePosition(QTextCursor::StartOfWord);
        tc.movePosition(QTextCursor::EndOfWord,QTextCursor::KeepAnchor);
        tc.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);
        tc.movePosition(QTextCursor::EndOfWord,QTextCursor::KeepAnchor);
    } else {
        tc.movePosition(QTextCursor::EndOfWord);
        tc.movePosition(QTextCursor::StartOfWord,QTextCursor::KeepAnchor);
    }

    return tc.selectedText();
}

void TextEdit::focusInEvent(QFocusEvent *e)
{
    if (mCompleter)
        mCompleter->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

static Completer::Context determineContext(const QTextCursor& cur) {
    QRegularExpression rxWord("([a-z]+)", QRegularExpression::CaseInsensitiveOption);

    static QHash<QString, Completer::Context> contexts = {
        {"from",Completer::From},
        {"select",Completer::Select},
        {"join",Completer::Join},
        {"where",Completer::Where},
        {"on",Completer::On},
        {"update",Completer::Update},
        {"set",Completer::Set},
        {"table", Completer::Table},
        {"to", Completer::To},
        {"column", Completer::Column},
        };

    auto curCopy = cur;
    while (true) {
        if (!curCopy.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor)) {
            return Completer::Undefined;
        }
        if (curCopy.selectedText().contains(";")) {
            qDebug() << "went too far" << curCopy.selectedText();
            return Completer::Undefined;
        }

        auto m = rxWord.match(curCopy.selectedText());
        if (m.hasMatch()) {
            QString word = m.captured(1).toLower();
            if (contexts.contains(word)) {
                qDebug() << "contexts.contains(word)" << word;
                return contexts[word];
            } else {
                qDebug() << "!contexts.contains(word)" << word;
            }
        } else {
            qDebug() << "!m.hasMatch()" << curCopy.selectedText();
        }
        qDebug() << curCopy.selectedText();
    }
    return Completer::Undefined;
}

bool TextEdit::keyPressEventCompleter(QKeyEvent *e) {

    QCompleter* c = mCompleter;

    if (c && c->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Backtab:
        case Qt::Key_Tab:
            e->ignore();
            return true;
        default:
            break;
        }
    }

    bool handled = false;

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut) {
        QPlainTextEdit::keyPressEvent(e);
        handled = true;
    }

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return handled;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,/;'[]\\-="); // end of word
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 2
                        || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return handled;
    }
    const bool isVisible = c && c->popup()->isVisible();
    if (!isVisible && e->key() == Qt::Key_Backspace) {
        return handled;
    }
    if (completionPrefix != c->completionPrefix()) {
        qDebug() << "setCompletionPrefix" << completionPrefix;
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }

    if (mCompleter->context() == Completer::Undefined) {
        auto context = determineContext(textCursor());
        if (context == Completer::On) {
            // todo join hint
        }
        mCompleter->setContext(context);
    }

    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr);
    return handled;
}


void TextEdit::keyPressEvent(QKeyEvent *event)
{
    auto* c = mCompleter;
    if (c && c->popup()->isVisible()) {
        if (!keyPressEventCompleter(event)) {
            if (!keyPressEventEdits(event)) {
                QPlainTextEdit::keyPressEvent(event);
            }
        }
    } else {
        if (!keyPressEventEdits(event)) {
            if (!keyPressEventCompleter(event)) {
                QPlainTextEdit::keyPressEvent(event);
            }
        }
    }
    fixEdits();
}




int TextEdit::nextEditIndex() const {
    if (m_edits.isEmpty()) {
        return -1;
    }
    QList<int> dist;
    int position = this->textCursor().position();
    for(int index = 0; index < m_edits.size(); index++) {
        const auto& edit = m_edits[index];
        int d = edit.first.position() - position;
        if (d < 0) {
            d = INT_MAX;
        }
        dist.append(d);
    }
    int index = std::min_element(dist.begin(), dist.end()) - dist.begin();
    return index;
}

int TextEdit::prevEditIndex() const {
    if (m_edits.isEmpty()) {
        return -1;
    }
    QList<int> dist;
    int position = this->textCursor().position();
    for(int index = 0; index < m_edits.size(); index++) {
        const auto& edit = m_edits[index];
        int d = position - edit.first.position();
        if (d < 0) {
            d = INT_MAX;
        }
        dist.append(d);
    }
    int index = std::min_element(dist.begin(), dist.end()) - dist.begin();
    return index;
}

void TextEdit::moveTextCursorToEdit(int index) {
    const auto& edit = m_edits[index];
    QTextCursor cursor = edit.second;
    cursor.movePosition(QTextCursor::PreviousCharacter);
    setTextCursor(cursor);
}

bool TextEdit::cursorAtEndOfWord() const {
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    QString text = cursor.selectedText();
    if (text.isEmpty() || text == " ") {
        //qDebug() << "cursorAtEndOfWord" << true << "text" << text;
        return true;
    }
    //qDebug() << "cursorAtEndOfWord" << false << "text" << text;
    return false;
}

static QPair<QTextCursor, QTextCursor> selectionToPair(const QTextCursor& cursor) {
    /*int position = cursor.position();
    int anchor = cursor.anchor();*/
    QTextCursor first = cursor;
    QTextCursor second = cursor;
    first.setPosition(cursor.position());
    second.setPosition(cursor.anchor());
    /*first.setPosition(std::min(position, anchor));
    second.setPosition(std::max(position, anchor));*/

    /*qDebug() << first.position() << first.anchor()
             << second.position() << second.anchor();*/

    return {first, second};
}


static QTextCursor selectEmmetExpression(const QTextCursor& cursor) {
    QTextCursor begin = cursor;
    QTextCursor startOfLine = cursor;
    startOfLine.movePosition(QTextCursor::StartOfLine);

    while (begin.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor)) {
        if (begin.selectedText().mid(0,1) == " ") {
            begin.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            break;
        }
        if (begin.position() == startOfLine.position()) {
            break;
        }
    }

    begin.setPosition(begin.position());
    begin.setPosition(cursor.position(), QTextCursor::KeepAnchor);

    /*h.movePosition(QTextCursor::StartOfWord);
    h.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
    if (h.selectedText() == ",") {
        h.movePosition(QTextCursor::StartOfWord);
    }
    t.movePosition(QTextCursor::EndOfWord);

    h.setPosition(t.position(), QTextCursor::KeepAnchor);*/

    //qDebug() << begin.selectedText();

    return begin;
}

#include <QMessageBox>

bool TextEdit::tryEmmet() {
    if (!cursorAtEndOfWord()) {
        return false;
    }
    QTextCursor cursor = selectEmmetExpression(textCursor());
    QString text = cursor.selectedText();
    if (text.isEmpty()) {
        qDebug() << "cursor.selectedText() is empty";
        return false;
    }
    QString error;
    QString e = Emmet::parse(text, error);
    if (!error.isEmpty()) {
        QMessageBox::critical(this, "Error", error);
        return false;
    }
    if (e.isEmpty()) {
        qDebug() << "cannot emmet text" << text;
        return false;
    }
    cursor.insertText(e);
    auto doc = document();
    while (true) {
        QTextCursor c = doc->find("#");
        if (c.isNull()) {
            break;
        }
        c.insertText("  ");
        c.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 2);
        auto p = selectionToPair(c);
        m_edits.append(p);
    }
    //fixEdits();
    if (m_edits.size() > 0) {
        moveTextCursorToEdit(0);
    }
    return true;
}

void TextEdit::fixEdits() {
    for(int i=m_edits.size()-1;i>=0;i--) {
        int position1 = m_edits[i].first.position();
        int position2 = m_edits[i].second.position();
        //qDebug() << position1 << position2;
        if (position1 == position2) {
            m_edits.removeAt(i);
        }
    }
}

void TextEdit::clearEdits() {
    for(int i=0;i<m_edits.size();i++) {
        const auto& edit = m_edits[i];
        QTextCursor cursor = edit.first;
        cursor.setPosition(edit.second.position(), QTextCursor::KeepAnchor);
        QString text = cursor.selectedText();
        cursor.insertText(text.trimmed());
    }
    m_edits.clear();
}

int TextEdit::editIndex() const {
    if (m_edits.isEmpty()) {
        return -1;
    }
    int position = this->textCursor().position();
    for(int index = 0; index < m_edits.size(); index++) {
        const auto& edit = m_edits[index];
        if (edit.first.position() < position && position < edit.second.position()) {
            //qDebug() << "a < b < c" << edit.first.position() << position << edit.second.position();
            return index;
        }
    }
    return -1;
}

bool TextEdit::keyPressEventEdits(QKeyEvent *event) {
    if (event->key() == Qt::Key_Tab) {

        int index = editIndex();
        if (index > -1) {
            moveTextCursorToEdit((index + 1) % m_edits.size());
        } else {
            if (tryEmmet()) {
                mCompleter->setContext(Completer::Undefined);
            } else {
                index = nextEditIndex();
                if (index > -1) {
                    moveTextCursorToEdit(index);
                }
            }
        }
        return true;
    } else if (event->key() == Qt::Key_Backtab) {
        //fixEdits();
        int index = editIndex();
        if (index > -1) {
            moveTextCursorToEdit((m_edits.size() + index - 1) % m_edits.size());
        } else {
            index = prevEditIndex();
            if (index > -1) {
                moveTextCursorToEdit(index);
            }
        }
        return true;
    } else if (event->key() == Qt::Key_Escape) {
        clearEdits();
        return true;
    }
    /*if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        fixEdits();
    }*/
    return false;
}



void TextEdit::updateCompleter() {
    /*QCompleter* completer = new QCompleter();
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    QStringList items = mTokens.autocompletion(mAliases);
    //dumpJson("D:\\w\\completer-%1.json", items);
    QStringListModel* stringListModel = new QStringListModel(items,completer);
    completer->setModel(stringListModel);
    setCompleter(completer);*/
    mCompleter->setData(mTokens.completerData());
}

void TextEdit::setText(const QString &text)
{
    setPlainText(text);
}

void TextEdit::onTextChanged() {
// todo query aliases
#if 0
    QMap<QString,QString> aliases = QueryParser::filterAliases(QueryParser::aliases(this->toPlainText()),mTokens.tables());
    if (aliases == mAliases) {
        return;
    }
    mAliases = aliases;
    updateCompleter();
#endif
}


void TextEdit::paintEvent(QPaintEvent *e)
{
    QPlainTextEdit::paintEvent(e);
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);
    for(const QPair<QTextCursor, QTextCursor>& edit: m_edits) {
        QTextCursor begin = edit.first;
        QTextCursor end = edit.second;
        auto path = createSelectionPath(begin, end, e->rect());
        painter.setPen(Qt::blue);
        painter.drawPath(path);
    }
}

QPainterPath TextEdit::createSelectionPath(const QTextCursor &begin, const QTextCursor &end,
                                           const QRect &clip)
{
    int borderWidth = 0;

    if (begin.isNull() || end.isNull() || begin.position() > end.position())
        return QPainterPath();

    QPointF offset = this->contentOffset();
    QRect viewportRect = rect();
    QTextDocument *document = this->document();

    if (this->blockBoundingGeometry(begin.block()).translated(offset).top() > clip.bottom() + 10
        || this->blockBoundingGeometry(end.block()).translated(offset).bottom()
               < clip.top() - 10) {
        return QPainterPath(); // nothing of the selection is visible
    }

    QTextBlock block = begin.block();

    if (block.blockNumber() < this->firstVisibleBlock().blockNumber() - 1)
        block = document->findBlockByNumber(this->firstVisibleBlock().blockNumber() - 1);

    if (begin.position() == end.position()) {
        // special case empty selections
        const QRectF blockGeometry = this->blockBoundingGeometry(block);
        QTextLayout *blockLayout = block.layout();
        int pos = begin.position() - begin.block().position();
        QTextLine line = blockLayout->lineForTextPosition(pos);
        //QTC_ASSERT(line.isValid(), return {});
        QRectF lineRect = line.naturalTextRect();
        lineRect = lineRect.translated(blockGeometry.topLeft());
        int x = line.cursorToX(pos);
        lineRect.setLeft(x - borderWidth);
        lineRect.setRight(x + borderWidth);
        lineRect.setBottom(lineRect.bottom() + borderWidth);
        QPainterPath path;
        path.addRect(lineRect);
        path.translate(offset);
        return path;
    }

    QPointF top;    //      *------|
    QPointF left;   //  *---|      |
    QPointF right;  //  |      |---*
    QPointF bottom; //  |------*

    for (; block.isValid() && block.blockNumber() <= end.blockNumber(); block = block.next()) {
        if (!block.isVisible())
            continue;

        const QRectF blockGeometry = this->blockBoundingGeometry(block);
        QTextLayout *blockLayout = block.layout();

        int firstLine = 0;

        int beginChar = 0;
        if (block == begin.block()) {
            beginChar = begin.positionInBlock();
            const QString preeditAreaText = begin.block().layout()->preeditAreaText();
            if (!preeditAreaText.isEmpty() && beginChar >= begin.block().layout()->preeditAreaPosition())
                beginChar += preeditAreaText.length();
            QTextLine line = blockLayout->lineForTextPosition(beginChar);
            //QTC_ASSERT(line.isValid(), return {});
            firstLine = line.lineNumber();
            const int lineEnd = line.textStart() + line.textLength();
            if (beginChar == lineEnd)
                continue;
        }

        int lastLine = blockLayout->lineCount() - 1;
        int endChar = -1;
        if (block == end.block()) {
            endChar = end.positionInBlock();
            const QString preeditAreaText = end.block().layout()->preeditAreaText();
            if (!preeditAreaText.isEmpty() && endChar >= end.block().layout()->preeditAreaPosition())
                endChar += preeditAreaText.length();
            QTextLine line = blockLayout->lineForTextPosition(endChar);
            //QTC_ASSERT(line.isValid(), return {});
            lastLine = line.lineNumber();
            if (endChar == beginChar)
                break; // Do not expand overlay to empty selection at end
        } else {
            endChar = block.length();
            while (endChar > beginChar
                   && document->characterAt(block.position() + endChar - 1).isSpace())
                --endChar;
        }

        for (int i = firstLine; i <= lastLine; ++i) {
            QTextLine line = blockLayout->lineAt(i);
            //QTC_ASSERT(line.isValid(), return {});
            QRectF lineRect = line.naturalTextRect();
            if (i == firstLine && beginChar > 0)
                lineRect.setLeft(line.cursorToX(beginChar));
            if (line.lineNumber() == lastLine)
                lineRect.setRight(line.cursorToX(endChar));
            lineRect = lineRect.translated(blockGeometry.topLeft());
            if (top.isNull())
                top = lineRect.topLeft();
            else if (left.isNull())
                left = lineRect.topLeft();
            else
                left.setX(std::min(left.x(), lineRect.left()));
            if (i == lastLine && block == end.block() && lineRect.right() <= right.x())
                bottom = lineRect.bottomRight();
            else
                right = {std::max(lineRect.right(), right.x()), lineRect.bottom()};
        }

        if (blockGeometry.translated(offset).y() > 2 * viewportRect.height())
            break;
    }

    if (top.isNull())
        return {};

    if (bottom.isNull())
        bottom = right;

    if (left.isNull())
        left = top;

    if (right.isNull())
        right = bottom;

    //QPointF marginOffset = {borderWidth, borderWidth};
    /*right += marginOffset;
    bottom += marginOffset;*/

    //      *------|
    //  *---|      |
    //  |      |---*
    //  |------*

    /*top += QPointF(-5, 0);
    bottom += QPointF(5, 0);

    left += QPointF(-5, 0);
    right += QPointF(5, 0);*/

    QPainterPath path;
    path.moveTo(top);
    path.lineTo(right.x(), top.y());
    path.lineTo(right);
    path.lineTo(bottom.x(), right.y());
    path.lineTo(bottom);
    path.lineTo(left.x(), bottom.y());
    path.lineTo(left);
    path.lineTo(top.x(), left.y());
    path.lineTo(top);

    /*
    qDebug() << QPointF(top)
             << QPointF(right.x(), top.y())
             << QPointF(right)
             << QPointF(bottom.x(), right.y())
             << QPointF(bottom)
             << QPointF(left.x(), bottom.y())
             << QPointF(left)
             << QPointF(top.x(), left.y())
             << QPointF(top);
*/

    path.closeSubpath();
    path.translate(offset);
    return path;
}
