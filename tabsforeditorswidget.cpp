#include "tabsforeditorswidget.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/idocument.h>
#include <texteditor/itexteditor.h>

#include <QShortcut>
#include <QTabWidget>

using namespace Core::Internal;

using namespace TabbedEditor::Internal;

TabsForEditorsWidget::TabsForEditorsWidget(QWidget *parent) :
    QWidget(parent),
    m_tabWidget(new QTabWidget(this))
{
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_tabWidget->sizePolicy().hasHeightForWidth());
    m_tabWidget->setSizePolicy(sizePolicy);
    m_tabWidget->setUsesScrollButtons(true);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);

    Core::EditorManager *em = Core::EditorManager::instance();

    QList<Core::IEditor*> editors = em->visibleEditors();
    foreach (Core::IEditor *editor, editors) {
        QWidget *tab = new QWidget();
        m_tabWidget->addTab(tab, editor->document()->displayName());
        m_tabsEditors.insert(tab, editor);
    }

    connect(em, SIGNAL(editorOpened(Core::IEditor*)), SLOT(handleEditorOpened(Core::IEditor*)));
    connect(em, SIGNAL(currentEditorChanged(Core::IEditor*)), SLOT(updateCurrentTab(Core::IEditor*)));
    connect(em, SIGNAL(editorsClosed(QList<Core::IEditor*>)), SLOT(handlerEditorClosed(QList<Core::IEditor*>)));

    connect(m_tabWidget, SIGNAL(currentChanged(int)), SLOT(handleCurrentChanged(int)));
    connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(handleTabCloseRequested(int)));

    QString shortCutSequence = QLatin1String("Ctrl+Alt+%1");
    for (int i = 1; i <= 10; ++i) {
        int key = i;
        if (key == 10)
            key = 0;
        QShortcut *shortcut = new QShortcut(shortCutSequence.arg(key), m_tabWidget);
        m_tabShortcuts.append(shortcut);
        connect(shortcut, SIGNAL(activated()), SLOT(selectTabAction()));
    }
}

QWidget *TabsForEditorsWidget::tabWidget() const
{
    return m_tabWidget;
}

void TabsForEditorsWidget::updateCurrentTab(Core::IEditor *editor)
{
    disconnect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleCurrentChanged(int))); //prevent update
    if (!editor)
        return;
    QWidget *currentTab = getTab(editor);
    if (!currentTab)
        return;
    m_tabWidget->setCurrentWidget(currentTab);
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleCurrentChanged(int))); //restore update
}


void TabsForEditorsWidget::handleCurrentChanged(int index)
{
    if (index == -1)
        return;

    QWidget *tab = m_tabWidget->widget(index);
    if (!tab)
        return;
    if (m_tabsEditors.contains(tab)) {
        Core::IEditor *editor = getEditor(tab);
        if (!editor)
            return;
        Core::EditorManager::instance()->activateEditor(editor);
    }
}

void TabsForEditorsWidget::handleEditorOpened(Core::IEditor *editor)
{
    QWidget *tab = new QWidget();
    m_tabWidget->addTab(tab, editor->document()->displayName());
    m_tabsEditors.insert(tab, editor);
    connect(editor->document(), SIGNAL(changed()), SLOT(updateTabText()));
}

void TabsForEditorsWidget::handlerEditorClosed(QList<Core::IEditor*> editors)
{
    foreach (Core::IEditor *editor, editors) {
        if (!editor)
            continue;
        QWidget *tab = getTab(editor);
        if (!tab)
            continue;

        m_tabsEditors.remove(tab);
        if (-1 < m_tabWidget->indexOf(tab))
            m_tabWidget->removeTab(m_tabWidget->indexOf(tab));
    }
}

void TabsForEditorsWidget::handleTabCloseRequested(int index)
{
    if (-1 < index) {
        QWidget *tab = m_tabWidget->widget(index);
        if (!tab)
            return;
        QList<Core::IEditor*> editorsToClose;
        editorsToClose.clear();
        if (m_tabsEditors.contains(tab)) {
            Core::IEditor *editor = getEditor(tab);
            if (!editor)
                return;
            editorsToClose.append(editor);

            Core::EditorManager::instance()->closeEditors(editorsToClose);
            if (m_tabsEditors.contains(tab))
                m_tabsEditors.remove(tab);
            if (-1 < m_tabWidget->indexOf(tab))
                m_tabWidget->removeTab(m_tabWidget->indexOf(tab));
        }
    }
}

void TabsForEditorsWidget::selectTabAction()
{
    QShortcut *shortcut = qobject_cast<QShortcut*>(sender());
    if (!shortcut)
        return;
    int index = m_tabShortcuts.indexOf(shortcut);
    m_tabWidget->setCurrentIndex(index);
}

void TabsForEditorsWidget::updateTabText()
{
    Core::IDocument *document = qobject_cast<Core::IDocument*>(QObject::sender());
    if (!document)
        return;

    QString tabTitle = document->displayName();
    if (document->isModified())
        tabTitle += QString::fromUtf8("*");

    QList<Core::IEditor*> editors
            = Core::EditorManager::instance()->documentModel()->editorsForDocument(document);
    foreach (Core::IEditor *editor, editors) {
        QWidget *tabToUpdate = getTab(editor);
        int tabToUpdateIndex = m_tabWidget->indexOf(tabToUpdate);
        m_tabWidget->setTabText(tabToUpdateIndex, tabTitle);
    }
}

Core::IEditor *TabsForEditorsWidget::getEditor(QWidget *tab) const
{
    return m_tabsEditors.value(tab);
}

QWidget *TabsForEditorsWidget::getTab(Core::IEditor *editor) const
{
    return m_tabsEditors.key(editor);
}

bool TabsForEditorsWidget::isEditorWidget(QObject *obj) const
{
    if (!obj)
        return false;
    QMapIterator<QWidget*, Core::IEditor*> i(m_tabsEditors);
    while (i.hasNext()) {
        i.next();
        if (obj == i.value()->widget())
            return true;
    }
    return false;
}

