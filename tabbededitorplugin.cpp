#include "tabbededitorplugin.h"

#include "tabbededitorconstants.h"
#include "tabsforeditorswidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/editormanager/editormanager.h>
#include <utils/stylehelper.h>

#include <QBoxLayout>
#include <QFile>
#include <QFrame>
#include <QMainWindow>

using namespace TabbedEditor::Internal;

TabbedEditorPlugin::TabbedEditorPlugin() :
    m_styleUpdatedToBaseColor(false)
{

}

bool TabbedEditorPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    m_editorManager = Core::EditorManager::instance();
    connect(m_editorManager, SIGNAL(editorOpened(Core::IEditor*)), SLOT(updateStyleToBaseColor()));

    m_backgroundFrame = new QFrame();
    m_backgroundFrame->setMinimumHeight(25);
    m_backgroundFrame->setMaximumHeight(25);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    m_tabbedWidget = new TabsForEditorsWidget(m_backgroundFrame);
    layout->addWidget(m_tabbedWidget->tabWidget());
    m_backgroundFrame->setLayout(layout);

    QWidget *coreWidget = Core::ICore::mainWindow();
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(coreWidget);
    mainWindow->layout()->setSpacing(0);

    QWidget* oldCentralWidget = mainWindow->centralWidget();
    QWidget* newCentralWidget = new QWidget(mainWindow);
    newCentralWidget->setMinimumHeight(0);

    QVBoxLayout *newCentralWidgetLayout = new QVBoxLayout();
    newCentralWidgetLayout->setSpacing(0);
    newCentralWidgetLayout->setContentsMargins(0, 0, 0, 0);

    newCentralWidgetLayout->addWidget(m_backgroundFrame);
    newCentralWidgetLayout->addWidget(oldCentralWidget);

    newCentralWidget->setLayout(newCentralWidgetLayout);

    mainWindow->setCentralWidget(newCentralWidget);

    m_backgroundFrame->setHidden(true);

    addAutoReleasedObject(m_tabbedWidget);

    return true;
}

void TabbedEditorPlugin::extensionsInitialized()
{

}

ExtensionSystem::IPlugin::ShutdownFlag TabbedEditorPlugin::aboutToShutdown()
{
    return SynchronousShutdown;
}

QString TabbedEditorPlugin::getStylesheetPatternFromFile(const QString &filepath)
{
    QFile stylesheetFile(filepath);
    if (!stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    return QString::fromUtf8(stylesheetFile.readAll());
}

void TabbedEditorPlugin::updateStyleToBaseColor()
{
    const QString baseColorQss
            = getQssStringFromColor(Utils::StyleHelper::baseColor().lighter(130));
    const QString borderColorQss = getQssStringFromColor(Utils::StyleHelper::borderColor());
    const QString highlightColorQss = getQssStringFromColor(Utils::StyleHelper::highlightColor());
    const QString selectedTabBorderColorQss
            = getQssStringFromColor(Utils::StyleHelper::highlightColor().lighter());
    const QString shadowColorQSS = getQssStringFromColor(Utils::StyleHelper::shadowColor());

    if (m_styleUpdatedToBaseColor) {
        disconnect(m_editorManager, SIGNAL(editorOpened(Core::IEditor*)),
                   this, SLOT(updateStyleToBaseColor()));
        return;
    }

    QString stylesheetPattern = getStylesheetPatternFromFile(QString::fromUtf8(":/styles/styles/default.qss"));

    stylesheetPattern = stylesheetPattern.replace(QStringLiteral("%FRAME_BACKGROUND_COLOR%"), highlightColorQss);
    stylesheetPattern = stylesheetPattern.replace(QStringLiteral("%TAB_SELECTED_BORDER_COLOR%"), selectedTabBorderColorQss);
    stylesheetPattern = stylesheetPattern.replace(QStringLiteral("%TAB_SELECTED_BACKGROUND_COLOR%"), baseColorQss);
    stylesheetPattern = stylesheetPattern.replace(QStringLiteral("%TAB_SELECTED_BOTTOM_BORDER_COLOR%"), baseColorQss);

    stylesheetPattern = stylesheetPattern.replace(QStringLiteral("%TAB_BACKGROUND_COLOR_FROM%"), shadowColorQSS);
    stylesheetPattern = stylesheetPattern.replace(QStringLiteral("%TAB_BACKGROUND_COLOR_TO%"), shadowColorQSS);
    stylesheetPattern = stylesheetPattern.replace(QStringLiteral("%TAB_BORDER_COLOR%"), borderColorQss);
    stylesheetPattern = stylesheetPattern.replace(QStringLiteral("%TAB_BOTTOM_BORDER_COLOR%"), borderColorQss);

    m_backgroundFrame->setStyleSheet(stylesheetPattern);
    m_backgroundFrame->setHidden(false);

    m_styleUpdatedToBaseColor = true;
}

QString TabbedEditorPlugin::getQssStringFromColor(const QColor &color) const
{
    return QStringLiteral("rgba( ") +
            QString::number(color.red()) +
            QStringLiteral(", ") +
            QString::number(color.green()) +
            QStringLiteral(", ") +
            QString::number(color.blue()) +
            QStringLiteral(", ") +
            QString::number(color.alpha()) +
            QStringLiteral(" )");
}
