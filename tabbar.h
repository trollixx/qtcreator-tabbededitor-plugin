#ifndef TABBAR_H
#define TABBAR_H

#include <QTabBar>

QT_BEGIN_NAMESPACE
class QShortcut;
QT_END_NAMESPACE

namespace Core {
class IEditor;
}

namespace TabbedEditor {
namespace Internal {

class TabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit TabBar(QWidget *parent = 0);

private slots:
    void updateCurrentTab(Core::IEditor *editor);
    void handleCurrentChanged(int index);
    void handleEditorOpened(Core::IEditor *editor);
    void handlerEditorClosed(QList<Core::IEditor*> editors);
    void handleTabCloseRequested(int index);
    void prevTabAction();
    void nextTabAction();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QList<Core::IEditor *> m_editors;
    QList<QShortcut *> m_shortcuts;
};

} // namespace Internal
} // namespace TabbedEditor

#endif // TABBAR_H
