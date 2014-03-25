#include "tabbededitorplugin.h"
#include "tabbededitorconstants.h"
#include "tabsforeditorswidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <utils/stylehelper.h>

#include <coreplugin/editormanager/editormanager.h>

#include <QAction>
#include <QMainWindow>
#include <QMenu>

#include <QtPlugin>
#include <QByteArray>

#include <QBoxLayout>
#include <QFrame>
#include <QMainWindow>

using namespace TabbedEditor::Internal;

TabbedEditorPlugin::TabbedEditorPlugin()
{
    styleUpdatedToBaseColor = false;
}

TabbedEditorPlugin::~TabbedEditorPlugin()
{
    tabbedWidget->deleteLater();
}

bool TabbedEditorPlugin::initialize(const QStringList &arguments, QString *errorString)
{   
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    em = Core::EditorManager::instance();

    connect(em, SIGNAL(editorOpened(Core::IEditor*)), this, SLOT(updateStyleToBaseColor()));

    backgroundFrame = new QFrame();
    backgroundFrame->setMinimumHeight(25);
    backgroundFrame->setMaximumHeight(25);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    tabbedWidget = new TabsForEditorsWidget(backgroundFrame);
    layout->addWidget(tabbedWidget->getTabWidget());
    backgroundFrame->setLayout(layout);

    QWidget *coreWidget = Core::ICore::mainWindow();
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(coreWidget);
    mainWindow->layout()->setSpacing(0);

    QWidget* oldCentralWidget = mainWindow->centralWidget();
    QWidget* newCentralWidget = new  QWidget(mainWindow);
    newCentralWidget->setMinimumHeight(0);

    QVBoxLayout *newCentralWidgetLayout = new QVBoxLayout();
    newCentralWidgetLayout->setSpacing(0);
    newCentralWidgetLayout->setContentsMargins(0, 0, 0, 0);

    newCentralWidgetLayout->addWidget(backgroundFrame);
    newCentralWidgetLayout->addWidget(oldCentralWidget);

    newCentralWidget->setLayout(newCentralWidgetLayout);

    mainWindow->setCentralWidget(newCentralWidget);

    backgroundFrame->setHidden(true);

    addAutoReleasedObject(tabbedWidget);

    return true;
}

void TabbedEditorPlugin::updateStyleToBaseColor()
{
    QColor baseColor = Utils::StyleHelper::baseColor();
    baseColor = baseColor.lighter(130);
    QString baseColorQSS = getQssStringFromColor(baseColor);

    QColor borderColor = Utils::StyleHelper::borderColor();
    QString borderColorQSS = getQssStringFromColor(borderColor);

    QColor highlightColor = Utils::StyleHelper::highlightColor();
    QString highlightColorQSS = getQssStringFromColor(highlightColor);

    QColor selectedTabBorderColor = highlightColor.lighter();
    QString selectedTabBorderColorQSS = getQssStringFromColor(selectedTabBorderColor);

    QColor shadowColor = Utils::StyleHelper::shadowColor();
    QString shadowColorQSS = getQssStringFromColor(shadowColor);

    if (styleUpdatedToBaseColor)
    {
        disconnect(em, SIGNAL(editorOpened(Core::IEditor*)), this, SLOT(updateStyleToBaseColor()));
        return;
    }
    QString stylesheetPattern = getStylesheetPatternFromFile(QString::fromUtf8(":/styles/styles/default.qss"));

    stylesheetPattern = stylesheetPattern.replace(QString::fromUtf8("%FRAME_BACKGROUND_COLOR%"), highlightColorQSS);
    stylesheetPattern = stylesheetPattern.replace(QString::fromUtf8("%TAB_SELECTED_BORDER_COLOR%"), selectedTabBorderColorQSS);
    stylesheetPattern = stylesheetPattern.replace(QString::fromUtf8("%TAB_SELECTED_BACKGROUND_COLOR%"), baseColorQSS);
    stylesheetPattern = stylesheetPattern.replace(QString::fromUtf8("%TAB_SELECTED_BOTTOM_BORDER_COLOR%"), baseColorQSS);

    stylesheetPattern = stylesheetPattern.replace(QString::fromUtf8("%TAB_BACKGROUND_COLOR_FROM%"), shadowColorQSS);
    stylesheetPattern = stylesheetPattern.replace(QString::fromUtf8("%TAB_BACKGROUND_COLOR_TO%"), shadowColorQSS);
    stylesheetPattern = stylesheetPattern.replace(QString::fromUtf8("%TAB_BORDER_COLOR%"), borderColorQSS);
    stylesheetPattern = stylesheetPattern.replace(QString::fromUtf8("%TAB_BOTTOM_BORDER_COLOR%"), borderColorQSS);

    backgroundFrame->setStyleSheet(stylesheetPattern);
    backgroundFrame->setHidden(false);

    styleUpdatedToBaseColor = true;
}
QString TabbedEditorPlugin::getQssStringFromColor(const QColor &color)
{
    QString QssString = QString::fromUtf8("rgba( " ) +
            QString::number(color.red()) +
            QString::fromUtf8(", ") +
            QString::number(color.green()) +
            QString::fromUtf8(", ") +
            QString::number(color.blue()) +
            QString::fromUtf8(", ") +
            QString::number(color.alpha()) +
            QString::fromUtf8(" )");
    return QssString;
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
    QString stylesheetContent;
    if (stylesheetFile.exists())
    {
        if (stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            stylesheetContent = QString::fromUtf8(stylesheetFile.readAll());
            stylesheetFile.close();
        }
    }
    return stylesheetContent;
}
Q_EXPORT_PLUGIN2(TabbedEditor, TabbedEditorPlugin)

