#ifndef TABSFOREDITORSWIDGET_H
#define TABSFOREDITORSWIDGET_H

#include <QMap>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QShortcut;
class QTabWidget;
QT_END_NAMESPACE

namespace Core {
class IEditor;
}

namespace TabbedEditor {
namespace Internal {

class TabsForEditorsWidget : public QWidget
{
    Q_OBJECT
public:
    TabsForEditorsWidget(QWidget * parent = 0);

    QWidget *tabWidget() const;

private slots:
    void updateCurrentTab(Core::IEditor *getEditor);
    void handleCurrentChanged(int index);
    void handleEditorOpened(Core::IEditor *getEditor);
    void handlerEditorClosed(QList<Core::IEditor*> editors);
    void handleTabCloseRequested(int index);
    void selectTabAction();
    void updateTabText();
    void prevTabAction();
    void nextTabAction();

private:
    Core::IEditor *getEditor(QWidget *getTab) const;
    QWidget *getTab(Core::IEditor *getEditor) const;
    bool isEditorWidget(QObject *obj) const;

    QTabWidget *m_tabWidget;
    QMap<QWidget *, Core::IEditor *> m_tabsEditors;
    QList<QShortcut *> m_tabShortcuts;
};

} // namespace Internal
} // namespace TabbedEditor

#endif // TABSFOREDITORSWIDGET_H
