#ifndef TABBEDEDITORVIEW_H
#define TABBEDEDITORVIEW_H

#include "tabbededitorplugin.h"

#include <coreplugin/editormanager/ieditor.h>

#include <QtGui>
#include <QTabWidget>
#include <QShortcut>

namespace TabbedEditor {
namespace Internal {

class TabsForEditorsWidget : public QWidget
{
    Q_OBJECT

public:
    TabsForEditorsWidget(QWidget * parent = 0);
    ~TabsForEditorsWidget();

    QWidget *getTabWidget();
    Core::IEditor *getEditor(QWidget *tab);
    QWidget *getTab(Core::IEditor *editor);

    bool isEditorWdiget(QObject *obj);

private:
    QTabWidget *tabWidget;
    QMap<QWidget*, Core::IEditor*> tabsEditors;
    QList<QShortcut*> tabShortcuts;

private slots:
    void updateCurrentTab(Core::IEditor* editor);
    void handleCurrentChanged(int index);
    void handleEditorOpened(Core::IEditor* editor);
    void handlerEditorClosed(QList<Core::IEditor*> editors);
    void handleTabCloseRequested(int index);

    void selectTabAction();

    void updateTabText();

};

} // namespace Internal
} // namespace TabbedEditor

#endif // TABBEDEDITORVIEW_H
