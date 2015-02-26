#include "tabbar.h"

#include "tabbededitorconstants.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/fileiconprovider.h>
#include <coreplugin/idocument.h>

#include <QMouseEvent>
#include <QShortcut>
#include <QTabBar>

using namespace Core::Internal;

using namespace TabbedEditor::Internal;

TabBar::TabBar(QWidget *parent) :
    QTabBar(parent)
{
    setExpanding(false);
    setMovable(true);
    setTabsClosable(true);
    setUsesScrollButtons(true);

    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sp.setHorizontalStretch(1);
    sp.setVerticalStretch(0);
    sp.setHeightForWidth(sizePolicy().hasHeightForWidth());
    setSizePolicy(sp);

    connect(this, &QTabBar::tabMoved, [this](int from, int to) {
        m_editors.move(from, to);
    });

    Core::EditorManager *em = Core::EditorManager::instance();

    /* TODO: That can happen only on session restore
    for (Core::IEditor *editor : em->visibleEditors()) {
        const int index = addTab(editor->document()->displayName());
        setTabToolTip(index, editor->document()->filePath());
        m_editors.append(editor);
    }
    */

    connect(em, SIGNAL(editorOpened(Core::IEditor*)), SLOT(handleEditorOpened(Core::IEditor*)));
    connect(em, SIGNAL(currentEditorChanged(Core::IEditor*)), SLOT(updateCurrentTab(Core::IEditor*)));
    connect(em, SIGNAL(editorsClosed(QList<Core::IEditor*>)), SLOT(handlerEditorClosed(QList<Core::IEditor*>)));

    connect(this, SIGNAL(currentChanged(int)), SLOT(handleCurrentChanged(int)));
    connect(this, SIGNAL(tabCloseRequested(int)), SLOT(handleTabCloseRequested(int)));

    const QString shortCutSequence = QStringLiteral("Ctrl+Alt+%1");
    for (int i = 1; i <= 10; ++i) {
        QShortcut *shortcut = new QShortcut(shortCutSequence.arg(i % 10), this);
        connect(shortcut, &QShortcut::activated, [this, shortcut]() {
            setCurrentIndex(m_shortcuts.indexOf(shortcut));
        });
        m_shortcuts.append(shortcut);
    }

    QAction *prevTabAction = new QAction(tr("Switch to previous tab"), this);
    Core::Command *prevTabCommand
            = Core::ActionManager::registerAction(prevTabAction,
                                                  TabbedEditor::Constants::PREV_TAB_ID,
                                                  Core::Context(Core::Constants::C_GLOBAL));
    prevTabCommand->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+J")));
    connect(prevTabAction, SIGNAL(triggered()), this, SLOT(prevTabAction()));

    QAction *nextTabAction = new QAction(tr("Switch to next tab"), this);
    Core::Command *nextTabCommand
            = Core::ActionManager::registerAction(nextTabAction,
                                                  TabbedEditor::Constants::NEXT_TAB_ID,
                                                  Core::Context(Core::Constants::C_GLOBAL));
    nextTabCommand->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+K")));
    connect(nextTabAction, SIGNAL(triggered()), this, SLOT(nextTabAction()));
}

void TabBar::updateCurrentTab(Core::IEditor *editor)
{
    const int index = m_editors.indexOf(editor);
    if (index == -1)
        return;
    setCurrentIndex(index);
}

void TabBar::handleCurrentChanged(int index)
{
    if (index < 0 || index >= m_editors.size())
        return;

    Core::EditorManager::instance()->activateEditor(m_editors[index]);
}

void TabBar::handleEditorOpened(Core::IEditor *editor)
{
    Core::IDocument *document = editor->document();

    const int index = addTab(document->displayName());
    setTabToolTip(index, document->filePath());
    setTabIcon(index, Core::FileIconProvider::icon(QFileInfo(document->filePath())));

    m_editors.append(editor);

    connect(document, &Core::IDocument::changed, [this, index, document]() {
        QString tabText = document->displayName();
        if (document->isModified())
            tabText += QLatin1Char('*');
        setTabText(index, tabText);
    });
}

void TabBar::handlerEditorClosed(QList<Core::IEditor*> editors)
{
    for (Core::IEditor *editor : editors) {
        const int index = m_editors.indexOf(editor);
        if (index == -1)
            continue;

        m_editors.removeAt(index);
        removeTab(index);
    }
}

void TabBar::handleTabCloseRequested(int index)
{
    if (index < 0 || index >= m_editors.size())
        return;

    Core::EditorManager::instance()->closeEditor(m_editors.takeAt(index));
    removeTab(index);
}

void TabBar::prevTabAction()
{
    int index = currentIndex();
    if (index >= 1)
        setCurrentIndex(index - 1);
    else
        setCurrentIndex(count() - 1);
}

void TabBar::nextTabAction()
{
    int index = currentIndex();
    if (index < count() - 1)
        setCurrentIndex(index + 1);
    else
        setCurrentIndex(0);
}

void TabBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
        handleTabCloseRequested(tabAt(event->pos()));
    QTabBar::mouseReleaseEvent(event);
}
