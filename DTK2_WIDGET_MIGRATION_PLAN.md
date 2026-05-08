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
- [x] Exported the new widgets through qmake sources and forwarding headers.
- [x] Added a `Containers` tab to DTKExample/collections for `DFrame`, `DCardWidget`, `DFloatingWidget`, and `DBackgroundGroup`.
- [x] Verified with `qmake ../dtkwidget.pro` and `make -j2` from `dtk2widget/build`.

## First-Batch API

- `DFrame`: base rounded frame, `frameRounded`, `radius`, `backgroundRole`.
- `DCardWidget`: card surface, `shadowEnabled`, `shadowBlurRadius`, `shadowColor`, `shadowOffset`.
- `DFloatingWidget`: elevated container, `setWidget()`, `blurBackgroundEnabled`, `blurBackground()`.
- `DBackgroundGroup`: grouped row/section background, `itemMargins`, `itemSpacing`, `radius`, `useWidgetBackground`, `backgroundRole`.

## Candidate Inventory

- Control center: `TranslucentFrame`, `SettingsItem`, `BasicListView`, label variants, `ButtonTuple`, `DCCSliderAnnotated`, navigation/sidebar widgets.
- File manager: `DToolBar`, `DStatusBar`, `DHoverButton`, `DIconTextButton`, `DCompleterListView`, sidebar item primitives, preview toolbar frames.
- Editor: bottom bar, find/replace/jump bars, dropdown menu, toast variant, theme panel/list view.
- Image viewer: blur frame, tooltip, return/image buttons, title frame/button, shortcut frame/editor, module panels.
- Movie: notification/tip/progress indicators, toolbox proxy, compact titlebar derivatives.
- System assistant: monitor/progress/icon/dot widgets, title bar, list views, page containers.

## Next Ticks

- [x] Add a grouped-background/list-section container similar to dtk5 `DBackgroundGroup`, but implemented without depending on dtk5 `DStyle`.
- [ ] Consolidate duplicate app label classes into reusable DTK2 typography helpers or documented presets.
- [ ] Migrate reusable icon/text/hover buttons from file manager and image viewer into generic button primitives.
- [ ] Add a compact notification/tip surface that can replace editor/movie/image-viewer local toast-like widgets.
- [ ] Review whether `DMainWindow`/`DTitlebar` should expose a compact/blur/elevated preset instead of each app subclassing title bars.
- [ ] Add an examples tab showing `DFrame`, `DCardWidget`, and `DFloatingWidget` in light/dark palettes.
- [ ] Start replacing one application-local widget usage after the new API settles.

## Constraints

- Avoid moving business-specific widgets into dtk2.
- Keep new APIs source-compatible with Qt 5 and current qmake build.
- Prefer small, explicit properties over dtk5-only style hooks.
- Preserve the flat GXDE style while adding only enough depth to clarify focus and hierarchy.
