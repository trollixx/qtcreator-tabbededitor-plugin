#ifndef TABBEDEDITOR_H
#define TABBEDEDITOR_H

#include "tabbededitor_global.h"

#include <extensionsystem/iplugin.h>
#include <QFrame>
#include <coreplugin/editormanager/editormanager.h>

namespace TabbedEditor {
namespace Internal {

class TabsForEditorsWidget;

class TabbedEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "TabbedEditor.json")

private:
    TabsForEditorsWidget *tabbedWidget;
    QFrame *backgroundFrame;
    bool styleUpdatedToBaseColor;
    Core::EditorManager *em;
    QString getQssStringFromColor(const QColor &color);

public:
    TabbedEditorPlugin();
    ~TabbedEditorPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();
private slots:
    QString getStylesheetPatternFromFile(const QString &filepath);
    void updateStyleToBaseColor();
};

} // namespace Internal
} // namespace TabbedEditor

#endif // TABBEDEDITOR_H
