#include "tabsforeditorswidget.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/editorview.h>
#include <coreplugin/idocument.h>
#include <texteditor/itexteditor.h>
#include <texteditor/basetexteditor.h>
#include <utils/stylehelper.h>

#include <QList>
#include <QMap>
#include <QListIterator>
#include <QAction>
#include <QLabel>
#include <QPointer>
#include <QObject>
#include <QMetaObject>

namespace TabbedEditor {
namespace Internal {

using namespace Core::Internal;

TabsForEditorsWidget::TabsForEditorsWidget(QWidget *parent):QWidget(parent)
{
    tabWidget = new QTabWidget(this);

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
    tabWidget->setSizePolicy(sizePolicy);
    tabWidget->setUsesScrollButtons(true);
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);

    Core::EditorManager *em = Core::EditorManager::instance();

    QList<Core::IEditor*> editors = em->visibleEditors();
    QListIterator<Core::IEditor*> editorsItr(editors);
    while (editorsItr.hasNext())
    {
        QWidget *tab = new QWidget();
        Core::IEditor *editor = editorsItr.next();

        tabWidget->addTab(tab, editor->document()->displayName());

        tabsEditors.insert(tab, editor);
    }

    connect(em, SIGNAL(editorOpened(Core::IEditor*)), this, SLOT(handleEditorOpened(Core::IEditor*)));
    connect(em, SIGNAL(currentEditorChanged(Core::IEditor*)), this, SLOT(updateCurrentTab(Core::IEditor*)));
    connect(em, SIGNAL(editorsClosed(QList<Core::IEditor*>)), this, SLOT(handlerEditorClosed(QList<Core::IEditor*>)));

    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleCurrentChanged(int)));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(handleTabCloseRequested(int)));

    QString shortCutSequence = QLatin1String("Ctrl+Alt+%1");
    for (int i = 1; i <=  10; ++i) {
        int key = i;
        if (key == 10)
            key = 0;
        QShortcut *shortCut = new QShortcut(shortCutSequence.arg(key), tabWidget);
        tabShortcuts.append(shortCut);
        connect(shortCut, SIGNAL(activated()), this, SLOT(selectTabAction()));
    }
}

TabsForEditorsWidget::~TabsForEditorsWidget()
{
    tabWidget->deleteLater();
}

void TabsForEditorsWidget::updateCurrentTab(Core::IEditor *editor)
{
    disconnect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleCurrentChanged(int))); //prevent update
    if(!editor)
        return;
    if (!tabWidget)
        return;
    QWidget *currentTab = this->getTab(editor);
    if (!currentTab)
        return;
    tabWidget->setCurrentWidget(currentTab);
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleCurrentChanged(int))); //restore update
}


void TabsForEditorsWidget::handleCurrentChanged(int index)
{
    if (index != -1)
    {
        QWidget *tab = tabWidget->widget(index);
        if (!tab)
            return;
        if (tabsEditors.contains(tab))
        {
            Core::IEditor *editor = this->getEditor(tab);
            if(!editor)
                return;
            Core::EditorManager::instance()->activateEditor(editor);
        }
    }
    else
    {
        return;
    }
}

void TabsForEditorsWidget::handleEditorOpened(Core::IEditor *editor)
{
    QWidget *tab = new QWidget();

    tabWidget->addTab(tab, editor->document()->displayName());
    tabsEditors.insert(tab, editor);

    connect(editor->document(), SIGNAL(changed()), this, SLOT(updateTabText()));
}

void TabsForEditorsWidget::handlerEditorClosed(QList<Core::IEditor *> editors)
{
    QListIterator<Core::IEditor*> editorsItr(editors);

    while (editorsItr.hasNext())
    {
        Core::IEditor *editor = editorsItr.next();
        if(!editor)
            return;
        QWidget *tab = getTab(editor);
        if (!tab)
            return;
        if(!tabWidget)
            return;

        if (tabsEditors.contains(tab))
            tabsEditors.remove(tab);
        if (-1 < tabWidget->indexOf(tab))
            tabWidget->removeTab( tabWidget->indexOf(tab) );
    }
}

void TabsForEditorsWidget::handleTabCloseRequested(int index)
{
    if (-1 < index) {
        QWidget *tab = tabWidget->widget(index);
        if (!tab)
            return;
        QList<Core::IEditor*> editorsToClose;
        editorsToClose.clear();
        if (tabsEditors.contains(tab)) {
            Core::IEditor *editor;
            editor = this->getEditor(tab);
            if (!editor)
            {
                return;
            }
            editorsToClose.append(this->getEditor(tab));

            Core::EditorManager::instance()->closeEditors(editorsToClose);
            if (tabsEditors.contains(tab))
                tabsEditors.remove(tab);
            if (-1 < tabWidget->indexOf(tab))
                tabWidget->removeTab(tabWidget->indexOf(tab));
        }
    }
}

void TabsForEditorsWidget::selectTabAction()
{
    if (QShortcut *shortCut = qobject_cast<QShortcut*>(sender())) {
        int index = tabShortcuts.indexOf(shortCut);
        tabWidget->setCurrentIndex(index);
    }
}

void TabsForEditorsWidget::updateTabText()
{
    Core::IDocument *document = qobject_cast<Core::IDocument *>(QObject::sender());
    QString tabTitle = document->displayName();
    if (document->isModified())
    {
        tabTitle = tabTitle + QString::fromUtf8("*");
    }

    Core::EditorManager *em = Core::EditorManager::instance();
    QList<Core::IEditor*> editors = em->documentModel()->editorsForDocument(document);
    for (int i = 0 ; i < editors.count() ; i++)
    {
        QWidget *tabToUpdate = this->getTab(editors.at(i));
        int tabToUpdateIndex = tabWidget->indexOf( tabToUpdate );
        tabWidget->setTabText(tabToUpdateIndex , tabTitle );
    }
}

QWidget *TabsForEditorsWidget::getTabWidget()
{
    return this->tabWidget;
}

Core::IEditor *TabsForEditorsWidget::getEditor(QWidget *tab)
{
    return tabsEditors.value(tab);
}

QWidget *TabsForEditorsWidget::getTab(Core::IEditor *editor)
{
    return tabsEditors.key(editor);
}

bool TabsForEditorsWidget::isEditorWdiget(QObject *obj)
{
    if(!obj)
        return false;
    QMapIterator<QWidget*, Core::IEditor*> i(tabsEditors);
    while (i.hasNext()) {
        i.next();
        if (obj == i.value()->widget())
            return true;
    }
    return false;
}

}//Internal
}//TabbedEditor
