#ifndef TABBEDEDITORPLUGIN_H
#define TABBEDEDITORPLUGIN_H

#include <extensionsystem/iplugin.h>

QT_BEGIN_NAMESPACE
class QFrame;
QT_END_NAMESPACE

namespace Core {
class EditorManager;
}

namespace TabbedEditor {
namespace Internal {

class TabsForEditorsWidget;

class TabbedEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "TabbedEditor.json")
public:
    TabbedEditorPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    QString getStylesheetPatternFromFile(const QString &filepath);
    void updateStyleToBaseColor();

private:
    QString getQssStringFromColor(const QColor &color) const;

    TabsForEditorsWidget *m_tabbedWidget;
    QFrame *m_backgroundFrame;
    bool m_styleUpdatedToBaseColor;
    Core::EditorManager *m_editorManager;
};

} // namespace Internal
} // namespace TabbedEditor

#endif // TABBEDEDITORPLUGIN_H
