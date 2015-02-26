#ifndef TABBEDEDITORPLUGIN_H
#define TABBEDEDITORPLUGIN_H

#include <extensionsystem/iplugin.h>

QT_BEGIN_NAMESPACE
class QFrame;
QT_END_NAMESPACE

namespace TabbedEditor {
namespace Internal {

class TabBar;

class TabbedEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "TabbedEditor.json")
public:
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized() {}

private slots:
    QString getStylesheetPatternFromFile(const QString &filepath);
    void updateStyleToBaseColor();
    void showTabBar();

private:
    static inline QString getQssStringFromColor(const QColor &color);

    TabBar *m_tabBar = nullptr;
    bool m_styleUpdatedToBaseColor = false;
};

} // namespace Internal
} // namespace TabbedEditor

#endif // TABBEDEDITORPLUGIN_H
