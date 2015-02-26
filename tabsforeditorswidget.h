#ifndef TABSFOREDITORSWIDGET_H
#define TABSFOREDITORSWIDGET_H

#include <QTabBar>

QT_BEGIN_NAMESPACE
class QShortcut;
QT_END_NAMESPACE

namespace Core {
class IEditor;
}

namespace TabbedEditor {
namespace Internal {

class TabsForEditorsWidget : public QTabBar
{
    Q_OBJECT
public:
    TabsForEditorsWidget(QWidget *parent = 0);

private slots:
    void updateCurrentTab(Core::IEditor *editor);
    void handleCurrentChanged(int index);
    void handleEditorOpened(Core::IEditor *editor);
    void handlerEditorClosed(QList<Core::IEditor*> editors);
    void handleTabCloseRequested(int index);
    void selectTabAction();
    void prevTabAction();
    void nextTabAction();

private:
    QList<Core::IEditor *> m_editors;
    QList<QShortcut *> m_shortcuts;
};

} // namespace Internal
} // namespace TabbedEditor

#endif // TABSFOREDITORSWIDGET_H
