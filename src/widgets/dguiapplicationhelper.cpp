#include "dguiapplicationhelper.h"

#include <QApplication>
#include <QDesktopServices>
#include <QImage>
#include <QStyle>
#include <QUrl>

static DGuiApplicationHelper *helperInstance = nullptr;
static DGuiApplicationHelper::ColorType currentPaletteType = DGuiApplicationHelper::UnknownType;
static DGuiApplicationHelper::SizeMode currentSizeMode = DGuiApplicationHelper::NormalMode;

DGuiApplicationHelper::DGuiApplicationHelper()
{
    if (!helperInstance)
        helperInstance = this;
}

DGuiApplicationHelper *DGuiApplicationHelper::instance()
{
    static DGuiApplicationHelper helper;
    return helperInstance ? helperInstance : &helper;
}

DGuiApplicationHelper::~DGuiApplicationHelper()
{
    if (helperInstance == this)
        helperInstance = nullptr;
}

DPlatformTheme *DGuiApplicationHelper::systemTheme() const
{
    DPlatformTheme *theme = new DPlatformTheme();
    return theme;
}

DPlatformTheme *DGuiApplicationHelper::applicationTheme() const
{
    DPlatformTheme *theme = new DPlatformTheme();
    return theme;
}

DPalette DGuiApplicationHelper::applicationPalette() const
{
    return applicationPalette(currentPaletteType);
}

DPalette DGuiApplicationHelper::applicationPalette(ColorType paletteType) const
{
    if (paletteType == UnknownType && qApp)
        return DPalette(qApp->palette());

    return standardPalette(paletteType == UnknownType ? LightType : paletteType);
}

DGuiApplicationHelper::ColorType DGuiApplicationHelper::themeType() const
{
    return currentPaletteType;
}

DGuiApplicationHelper::ColorType DGuiApplicationHelper::paletteType() const
{
    return currentPaletteType;
}

bool DGuiApplicationHelper::hasUserManual() const
{
    return false;
}

void DGuiApplicationHelper::setPaletteType(DGuiApplicationHelper::ColorType paletteType)
{
    currentPaletteType = paletteType;

    if (qApp && (paletteType == LightType || paletteType == DarkType))
        qApp->setPalette(standardPalette(paletteType));
}

void DGuiApplicationHelper::setThemeType(DGuiApplicationHelper::ColorType themeType)
{
    setPaletteType(themeType);
}

DPalette DGuiApplicationHelper::standardPalette(ColorType type)
{
    DPalette palette;

    if (type == DarkType) {
        palette.setColor(QPalette::Window, QColor(37, 37, 37));
        palette.setColor(QPalette::WindowText, QColor(235, 235, 235));
        palette.setColor(QPalette::Base, QColor(30, 30, 30));
        palette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
        palette.setColor(QPalette::ToolTipBase, QColor(45, 45, 45));
        palette.setColor(QPalette::ToolTipText, QColor(245, 245, 245));
        palette.setColor(QPalette::Text, QColor(235, 235, 235));
        palette.setColor(QPalette::Button, QColor(48, 48, 48));
        palette.setColor(QPalette::ButtonText, QColor(235, 235, 235));
        palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
        palette.setColor(QPalette::Light, QColor(68, 68, 68));
        palette.setColor(QPalette::Midlight, QColor(58, 58, 58));
        palette.setColor(QPalette::Mid, QColor(82, 82, 82));
        palette.setColor(QPalette::Dark, QColor(24, 24, 24));
        palette.setColor(QPalette::Shadow, QColor(0, 0, 0));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
        palette.setColor(QPalette::Link, QColor(64, 160, 255));
        palette.setColor(QPalette::LinkVisited, QColor(150, 120, 220));
    } else {
        palette = qApp ? DPalette(qApp->style()->standardPalette()) : DPalette(QPalette());
        palette.setColor(QPalette::Window, QColor(248, 248, 248));
        palette.setColor(QPalette::WindowText, QColor(35, 35, 35));
        palette.setColor(QPalette::Base, QColor(255, 255, 255));
        palette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
        palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
        palette.setColor(QPalette::ToolTipText, QColor(35, 35, 35));
        palette.setColor(QPalette::Text, QColor(35, 35, 35));
        palette.setColor(QPalette::Button, QColor(250, 250, 250));
        palette.setColor(QPalette::ButtonText, QColor(35, 35, 35));
        palette.setColor(QPalette::Light, QColor(255, 255, 255));
        palette.setColor(QPalette::Midlight, QColor(245, 245, 245));
        palette.setColor(QPalette::Mid, QColor(210, 210, 210));
        palette.setColor(QPalette::Dark, QColor(170, 170, 170));
        palette.setColor(QPalette::Shadow, QColor(120, 120, 120));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
        palette.setColor(QPalette::Link, QColor(0, 112, 210));
        palette.setColor(QPalette::LinkVisited, QColor(100, 80, 180));
    }

    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(160, 160, 160));

    return palette;
}

void DGuiApplicationHelper::setApplicationPalette(const DPalette &palette)
{
    qApp->setPalette(palette);
    currentPaletteType = toColorType(palette);
}

DGuiApplicationHelper::ColorType DGuiApplicationHelper::toColorType(const QColor &color)
{
    return color.lightness() < 128 ? DarkType : LightType;
}

DGuiApplicationHelper::ColorType DGuiApplicationHelper::toColorType(const QPalette &palette)
{
    return toColorType(palette.color(QPalette::Window));
}

void DGuiApplicationHelper::generatePaletteColor(DPalette &base, QPalette::ColorRole role, ColorType type)
{
    base.setColor(role, standardPalette(type).color(role));
}

void DGuiApplicationHelper::generatePaletteColor(DPalette &base, DPalette::ColorType role, ColorType type)
{
    Q_UNUSED(base)
    Q_UNUSED(role)
    Q_UNUSED(type)
}

void DGuiApplicationHelper::generatePalette(DPalette &base, ColorType type)
{
    base = standardPalette(type == UnknownType ? LightType : type);
}

DPalette DGuiApplicationHelper::fetchPalette(const DPlatformTheme *theme)
{
    Q_UNUSED(theme)
    return standardPalette(currentPaletteType == UnknownType ? LightType : currentPaletteType);
}

QColor DGuiApplicationHelper::adjustColor(const QColor &base, qint8 hueFloat, qint8 saturationFloat, qint8 lightnessFloat,
                                          qint8 redFloat, qint8 greenFloat, qint8 blueFloat, qint8 alphaFloat)
{
    QColor color = base;
    int h = color.hue();
    int s = color.saturation();
    int l = color.lightness();
    color.setHsl(qBound(0, h + hueFloat, 359), qBound(0, s + saturationFloat, 255), qBound(0, l + lightnessFloat, 255), color.alpha());
    color.setRed(qBound(0, color.red() + redFloat, 255));
    color.setGreen(qBound(0, color.green() + greenFloat, 255));
    color.setBlue(qBound(0, color.blue() + blueFloat, 255));
    color.setAlpha(qBound(0, color.alpha() + alphaFloat, 255));
    return color;
}

QImage DGuiApplicationHelper::adjustColor(const QImage &base, qint8 hueFloat, qint8 saturationFloat, qint8 lightnessFloat,
                                          qint8 redFloat, qint8 greenFloat, qint8 blueFloat, qint8 alphaFloat)
{
    QImage image = base.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    for (int y = 0; y < image.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
        for (int x = 0; x < image.width(); ++x)
            line[x] = adjustColor(QColor::fromRgba(line[x]), hueFloat, saturationFloat, lightnessFloat, redFloat, greenFloat, blueFloat, alphaFloat).rgba();
    }

    return image;
}

QColor DGuiApplicationHelper::blendColor(const QColor &substrate, const QColor &superstratum)
{
    const qreal alpha = superstratum.alphaF();
    return QColor(qRound(superstratum.red() * alpha + substrate.red() * (1 - alpha)),
                  qRound(superstratum.green() * alpha + substrate.green() * (1 - alpha)),
                  qRound(superstratum.blue() * alpha + substrate.blue() * (1 - alpha)),
                  255);
}

bool DGuiApplicationHelper::isXWindowPlatform()
{
    return qApp->platformName().contains(QStringLiteral("xcb"));
}

bool DGuiApplicationHelper::isTabletEnvironment()
{
    return false;
}

bool DGuiApplicationHelper::isSpecialEffectsEnvironment()
{
    return true;
}

void DGuiApplicationHelper::setAttribute(Attribute attribute, bool enable)
{
    Q_UNUSED(attribute)
    Q_UNUSED(enable)
}

bool DGuiApplicationHelper::testAttribute(Attribute attribute)
{
    Q_UNUSED(attribute)
    return false;
}

void DGuiApplicationHelper::setUseInactiveColorGroup(bool on)
{
    Q_UNUSED(on)
}

void DGuiApplicationHelper::setColorCompositingEnabled(bool on)
{
    Q_UNUSED(on)
}

bool DGuiApplicationHelper::setSingleInstance(const QString &key, SingleScope singleScope)
{
    Q_UNUSED(key)
    Q_UNUSED(singleScope)
    return true;
}

void DGuiApplicationHelper::setSingleInstanceInterval(int interval)
{
    Q_UNUSED(interval)
}

void DGuiApplicationHelper::setSingelInstanceInterval(int interval)
{
    setSingleInstanceInterval(interval);
}

QStringList DGuiApplicationHelper::userManualPaths(const QString &appName)
{
    Q_UNUSED(appName)
    return QStringList();
}

bool DGuiApplicationHelper::loadTranslator(const QString &fileName, const QList<QString> &translateDirs, const QList<QLocale> &localeFallback)
{
    Q_UNUSED(fileName)
    Q_UNUSED(translateDirs)
    Q_UNUSED(localeFallback)
    return false;
}

bool DGuiApplicationHelper::loadTranslator(const QList<QLocale> &localeFallback)
{
    Q_UNUSED(localeFallback)
    return false;
}

void DGuiApplicationHelper::openUrl(const QString &url)
{
    QDesktopServices::openUrl(QUrl(url));

}

void DGuiApplicationHelper::handleHelpAction()
{

}

DGuiApplicationHelper::SizeMode DGuiApplicationHelper::sizeMode() const
{
    return currentSizeMode;
}

void DGuiApplicationHelper::setSizeMode(const DGuiApplicationHelper::SizeMode mode)
{
    currentSizeMode = mode;
}

void DGuiApplicationHelper::resetSizeMode()
{
    currentSizeMode = NormalMode;
}
