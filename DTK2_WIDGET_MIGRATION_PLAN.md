# DTK2 Widget Migration Plan

## Goal

Collect reusable widgets and window building blocks that are currently duplicated in GXDE applications, move the low-level pieces into `dtk2widget`, and keep the visual language consistent: small radius, compact title bars, flat surfaces, clearer hierarchy, and more deliberate blur usage.

## Completed Ticks

- [x] Confirmed repository layout: `dtk2widget` is the qmake/libdtkwidget2 target; `dtk5widget` is the CMake reference implementation.
- [x] Scanned application-local widgets in control center, file manager, editor, image viewer, movie, and system assistant.
- [x] Picked the first low-risk migration batch: reusable visual containers with no business-model dependency.
- [x] Added `DFrame` to provide a rounded, palette-aware base frame plus horizontal/vertical line aliases.
- [x] Added `DCardWidget` to provide a reusable small-radius card with a restrained default shadow.
- [x] Added `DFloatingWidget` to provide a reusable elevated/blur-capable floating container inspired by dtk5.
- [x] Added `DBackgroundGroup` to provide dtk5-like grouped row backgrounds without depending on dtk5 `DStyle`.
- [x] Added `DTypographyLabel` and `DSectionTitle` to replace repeated app-local small/normal/large/section title label patterns.
- [x] Added `DHoverButton` and `DIconTextButton` for repeated file-manager/viewer icon button patterns.
- [x] Promoted dtk2 `DIconButton` as a reusable icon-only button while preserving deprecated `DButtonList` compatibility.
- [x] Added `DFloatingMessage` for repeated editor/movie/viewer in-window notification and tip patterns.
- [x] Added `DBlurSurface` for reusable control-center style translucent rounded panels backed by `DBlurEffectWidget`.
- [x] Added `DTabbedStackWidget` for system-assistant style top tab plus stacked page switching.
- [x] Added `DDocumentTabBar` for editor-style document tabs with stable document ids, modified markers, close helpers, and next/previous navigation.
- [x] Added compact/transparent/elevated titlebar presets and matching main-window presets for common app titlebar subclasses.
- [x] Exported the new widgets through qmake sources and forwarding headers.
- [x] Added a `Containers` tab to DTKExample/collections for `DFrame`, `DCardWidget`, `DFloatingWidget`, `DBackgroundGroup`, `DTypographyLabel`, and `DSectionTitle`.
- [x] Verified with `qmake ../dtkwidget.pro` and `make -j2` from `dtk2widget/build`.

## First-Batch API

- `DFrame`: base rounded frame, `frameRounded`, `radius`, `backgroundRole`.
- `DCardWidget`: card surface, `shadowEnabled`, `shadowBlurRadius`, `shadowColor`, `shadowOffset`.
- `DFloatingWidget`: elevated container, `setWidget()`, `blurBackgroundEnabled`, `blurBackground()`.
- `DBackgroundGroup`: grouped row/section background, `itemMargins`, `itemSpacing`, `radius`, `useWidgetBackground`, `backgroundRole`.
- `DTypographyLabel`: reusable text hierarchy, `LargeTitle`, `Title`, `Body`, `Caption`, `emphasis`, `secondary`.
- `DSectionTitle`: section header with compact title text and trailing separator line.
- `DHoverButton`: icon button with normal/hover/pressed icon states.
- `DIconTextButton`: push button with consistent centered icon/text spacing.
- `DIconButton`: icon-only button with optional circle mode and notification dot.
- `DFloatingMessage`: transient/resident floating message with icon, message text, optional action widget, blur background, and close handling.
- `DBlurSurface`: reusable blur-backed rounded panel, `blurEnabled`, `maskAlpha`, `maskColor`, `blendMode`, and `blurBackground()`.
- `DTabbedStackWidget`: compact `DTabBar` plus `QStackedWidget` wrapper, `addPage()`, `insertPage()`, `removePage()`, `currentIndex`, and direct access to both child widgets.
- `DDocumentTabBar`: `DTabBar` subclass for document tabs, `addDocument()`, `insertDocument()`, `updateDocument()`, `setDocumentModified()`, close-current/close-other helpers, and document id lookup.
- `DTitlebar` / `DMainWindow` presets: `DTitlebar::applyStylePreset()` and `DMainWindow::applyWindowPreset()` provide default, compact, transparent/blur, and elevated configurations.

## Candidate Inventory

- Control center: `TranslucentFrame`, `SettingsItem`, `BasicListView`, label variants, `ButtonTuple`, `DCCSliderAnnotated`, navigation/sidebar widgets.
- File manager: `DToolBar`, `DStatusBar`, `DHoverButton`, `DIconTextButton`, `DCompleterListView`, sidebar item primitives, preview toolbar frames.
- Editor: bottom bar, find/replace/jump bars, dropdown menu, toast variant, theme panel/list view.
- Image viewer: blur frame, tooltip, return/image buttons, title frame/button, shortcut frame/editor, module panels.
- Movie: notification/tip/progress indicators, toolbox proxy, compact titlebar derivatives.
- System assistant: monitor/progress/icon/dot widgets, title bar, list views, page containers.

## Next Ticks

- [x] Add a grouped-background/list-section container similar to dtk5 `DBackgroundGroup`, but implemented without depending on dtk5 `DStyle`.
- [x] Consolidate duplicate app label classes into reusable DTK2 typography helpers or documented presets.
- [x] Migrate reusable icon/text/hover buttons from file manager and image viewer into generic button primitives.
- [x] Add a compact notification/tip surface that can replace editor/movie/image-viewer local toast-like widgets.
- [x] Collect and generalize control-center blur widgets into reusable DTK2 blur surfaces and panels.
- [x] Absorb system-assistant multi-tab/page-switching patterns into DTK2 after the current widget migration batch.
- [x] Absorb editor multi-tab document/tabbar patterns into DTK2 after the current widget migration batch.
- [x] Review whether `DMainWindow`/`DTitlebar` should expose a compact/blur/elevated preset instead of each app subclassing title bars.
- [ ] Add an examples tab showing `DFrame`, `DCardWidget`, and `DFloatingWidget` in light/dark palettes.
- [ ] Start replacing one application-local widget usage after the new API settles.

## Deferred Optimization Tracks

- Control center blur widgets: first reusable surface is `DBlurSurface`; still review account avatar overlays and quick control panels for narrower reusable patterns rather than module-specific logic.
- System assistant tabs/pages: first reusable page switcher is `DTabbedStackWidget`; app-specific titlebar icons, tray actions, and monitor lifecycle stay in applications.
- Editor tabs: first reusable document tab helper is `DDocumentTabBar`; editor-specific drag previews, wrapper transfer, and window/titlebar background changes remain in the editor.

## Constraints

- Avoid moving business-specific widgets into dtk2.
- Keep new APIs source-compatible with Qt 5 and current qmake build.
- Prefer small, explicit properties over dtk5-only style hooks.
- Preserve the flat GXDE style while adding only enough depth to clarify focus and hierarchy.
