#include "palettetab.h"

#include "dbackgroundgroup.h"
#include "dblursurface.h"
#include "dcardwidget.h"
#include "dguiapplicationhelper.h"
#include "dswitchbutton.h"
#include "dtypographylabel.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

static QLabel *label(const QString &text, DTypographyLabel::TextRole role, QWidget *parent)
{
    DTypographyLabel *l = new DTypographyLabel(text, role, parent);
    l->setWordWrap(true);
    return l;
}

static QWidget *row(const QString &title, const QString &description, QWidget *rightWidget, QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(w);
    QVBoxLayout *textLayout = new QVBoxLayout;
    textLayout->setSpacing(2);
    textLayout->addWidget(label(title, DTypographyLabel::Body, w));
    DTypographyLabel *caption = new DTypographyLabel(description, DTypographyLabel::Caption, w);
    caption->setSecondary(true);
    textLayout->addWidget(caption);

    layout->setContentsMargins(14, 10, 14, 10);
    layout->addLayout(textLayout);
    layout->addStretch();
    if (rightWidget)
        layout->addWidget(rightWidget);
    return w;
}

static QWidget *createPreview(const QString &title, DGuiApplicationHelper::ColorType type, QWidget *parent)
{
    QWidget *page = new QWidget(parent);
    page->setAutoFillBackground(true);
    page->setPalette(DGuiApplicationHelper::standardPalette(type));

    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);

    DCardWidget *card = new DCardWidget(page);
    card->setPalette(page->palette());
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 14, 18, 14);
    cardLayout->setSpacing(8);
    cardLayout->addWidget(new DSectionTitle(title, card));
    DTypographyLabel *body = new DTypographyLabel("Palette-aware widgets use QPalette roles, so the same component code renders correctly in light and dark themes.", DTypographyLabel::Body, card);
    body->setWordWrap(true);
    cardLayout->addWidget(body);
    cardLayout->addWidget(new QPushButton("Action", card), 0, Qt::AlignLeft);
    layout->addWidget(card);

    DBackgroundGroup *group = new DBackgroundGroup(new QVBoxLayout, page);
    group->setPalette(page->palette());
    group->layout()->setContentsMargins(0, 0, 0, 0);
    group->setItemMargins(QMargins(0, 0, 0, 0));
    group->layout()->addWidget(row("Grouped row", "Shared rounded background follows the preview palette.", new DSwitchButton(group), group));
    group->layout()->addWidget(row("Secondary text", "Caption text keeps contrast without custom colors.", new QPushButton("Open", group), group));
    layout->addWidget(group);

    DBlurSurface *surface = new DBlurSurface(page);
    surface->setPalette(page->palette());
    surface->setMaskAlpha(type == DGuiApplicationHelper::DarkType ? 120 : 82);
    QVBoxLayout *surfaceLayout = new QVBoxLayout(surface);
    surfaceLayout->setContentsMargins(18, 14, 18, 14);
    surfaceLayout->addWidget(label("Blur surface", DTypographyLabel::Title, surface));
    surfaceLayout->addWidget(label("Built-in palettes also feed blur masks and translucent panels.", DTypographyLabel::Caption, surface));
    layout->addWidget(surface);

    layout->addStretch();
    return page;
}

PaletteTab::PaletteTab(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(14);

    DTypographyLabel *heading = new DTypographyLabel("Built-in palette themes", DTypographyLabel::Title, this);
    heading->setEmphasis(true);
    mainLayout->addWidget(heading);
    DTypographyLabel *intro = new DTypographyLabel("DTK2 now has built-in light and dark QPalette presets. These previews render the same widgets under both palettes without relying on system ddark/dlight styles.", DTypographyLabel::Body, this);
    intro->setSecondary(true);
    intro->setWordWrap(true);
    mainLayout->addWidget(intro);

    QHBoxLayout *previewLayout = new QHBoxLayout;
    previewLayout->setSpacing(16);
    previewLayout->addWidget(createPreview("Light palette", DGuiApplicationHelper::LightType, this));
    previewLayout->addWidget(createPreview("Dark palette", DGuiApplicationHelper::DarkType, this));
    mainLayout->addLayout(previewLayout);
}
