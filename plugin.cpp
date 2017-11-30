#include "plugin.h"

#include "constants.h"
#include "tabbar.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>
#include <utils/stylehelper.h>
#include <utils/theme/theme.h>

#include <QBoxLayout>
#include <QFile>
#include <QMainWindow>

using namespace TabbedEditor::Internal;

TabbedEditorPlugin::TabbedEditorPlugin() :
    m_tabBar(nullptr),
    m_styleUpdatedToBaseColor(false)
{
}

bool TabbedEditorPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    connect(Core::ICore::instance(), SIGNAL(themeChanged()), this, SLOT(updateStyleToBaseColor()));
    connect(Core::EditorManager::instance(), SIGNAL(editorOpened(Core::IEditor*)), SLOT(showTabBar()));

    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(Core::ICore::mainWindow());
    mainWindow->layout()->setSpacing(0);

    QWidget *wrapper = new QWidget(mainWindow);
    wrapper->setMinimumHeight(0);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tabBar = new TabBar();
    layout->addWidget(m_tabBar);
    layout->addWidget(mainWindow->centralWidget());

    wrapper->setLayout(layout);

    mainWindow->setCentralWidget(wrapper);

    return true;
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
    Utils::Theme *theme = Utils::creatorTheme();

    QString baseColorQss;
    QString borderColorQss;
    QString highlightColorQss;
    QString selectedTabBorderColorQss;
    QString shadowColorQss;

    if(theme->preferredStyles().isEmpty()) {
        baseColorQss = getQssStringFromColor(Utils::StyleHelper::baseColor().lighter(130));
        borderColorQss = getQssStringFromColor(Utils::StyleHelper::borderColor());
        highlightColorQss = getQssStringFromColor(Utils::StyleHelper::baseColor());
        selectedTabBorderColorQss
                = getQssStringFromColor(Utils::StyleHelper::highlightColor().lighter());
        shadowColorQss = getQssStringFromColor(Utils::StyleHelper::shadowColor());
    } else { // Flat widget style
        baseColorQss
                = getQssStringFromColor(theme->color(Utils::Theme::BackgroundColorHover));
        borderColorQss = getQssStringFromColor(theme->color(Utils::Theme::BackgroundColorHover));
        highlightColorQss = getQssStringFromColor(theme->color(Utils::Theme::BackgroundColorDark));
        selectedTabBorderColorQss
                = getQssStringFromColor(theme->color(Utils::Theme::BackgroundColorDark));
        shadowColorQss = getQssStringFromColor(theme->color(Utils::Theme::BackgroundColorNormal));
    }

    QString stylesheetPattern = getStylesheetPatternFromFile(QStringLiteral(":/styles/default.qss"));

    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%FRAME_BACKGROUND_COLOR%"), highlightColorQss);
    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_SELECTED_BORDER_COLOR%"), selectedTabBorderColorQss);
    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_SELECTED_BACKGROUND_COLOR%"), baseColorQss);
    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_SELECTED_BOTTOM_BORDER_COLOR%"), baseColorQss);

    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_BACKGROUND_COLOR_FROM%"), shadowColorQss);
    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_BACKGROUND_COLOR_TO%"), shadowColorQss);
    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_BORDER_COLOR%"), borderColorQss);
    stylesheetPattern = stylesheetPattern.replace(QLatin1String("%TAB_BOTTOM_BORDER_COLOR%"), borderColorQss);

    m_tabBar->setStyleSheet(stylesheetPattern);
}

void TabbedEditorPlugin::showTabBar()
{
    updateStyleToBaseColor();

    if (m_styleUpdatedToBaseColor) {
        disconnect(Core::EditorManager::instance(), SIGNAL(editorOpened(Core::IEditor*)),
                   this, SLOT(showTabBar()));
        return;
    }

    m_styleUpdatedToBaseColor = true;
}

QString TabbedEditorPlugin::getQssStringFromColor(const QColor &color)
{
    return QString::fromLatin1("rgba(%1, %2, %3, %4)").arg(
                QString::number(color.red()),
                QString::number(color.green()),
                QString::number(color.blue()),
                QString::number(color.alpha()));
}
