#pragma once

// ThemePalette — centralised colour roles for AetherSDR's UI themes (#1160).
//
// Usage:
//   const ThemePalette& pal = ThemePalette::current();
//   painter.fillRect(rect, pal.background);
//
// The active palette is set once at startup (and again if the user toggles
// the theme at runtime) via ThemePalette::setLight(bool).

#include <QColor>

namespace AetherSDR {

struct ThemePalette {
    // Base surface colours
    QColor background;    // root widget / window background
    QColor widgetBg;      // controls, combo boxes, list views
    QColor altRowBg;      // alternate list-row background
    QColor deepBg;        // menu bar, status bar — slightly darker than background
    QColor border;        // widget borders and splitter handles

    // Text / foreground
    QColor text;          // primary text
    QColor dimText;       // secondary / muted text (drag-grip dots, etc.)
    QColor accentText;    // group-box titles, highlighted labels
    QColor headerText;    // applet title-bar label text

    // Accent
    QColor accent;        // selection highlight, pressed-button fill, slider handle

    // Applet panel header gradient stops (top → bottom)
    QColor headerTop;
    QColor headerMid;
    QColor headerBot;
    QColor headerBorderBot; // bottom border of the header band

    // ── Factory functions ──────────────────────────────────────────────────

    static ThemePalette dark()
    {
        ThemePalette p;
        p.background     = QColor(0x0f, 0x0f, 0x1a);
        p.widgetBg       = QColor(0x1a, 0x2a, 0x3a);
        p.altRowBg       = QColor(0x16, 0x16, 0x26);
        p.deepBg         = QColor(0x0a, 0x0a, 0x14);
        p.border         = QColor(0x20, 0x30, 0x40);
        p.text           = QColor(0xc8, 0xd8, 0xe8);
        p.dimText        = QColor(0x60, 0x70, 0x80);
        p.accentText     = QColor(0x00, 0xb4, 0xd8);
        p.headerText     = QColor(0x8a, 0xa8, 0xc0);
        p.accent         = QColor(0x00, 0xb4, 0xd8);
        p.headerTop      = QColor(0x3a, 0x4a, 0x5a);
        p.headerMid      = QColor(0x2a, 0x3a, 0x4a);
        p.headerBot      = QColor(0x1a, 0x2a, 0x38);
        p.headerBorderBot= QColor(0x0a, 0x1a, 0x28);
        return p;
    }

    static ThemePalette light()
    {
        ThemePalette p;
        p.background     = QColor(0xf0, 0xf4, 0xf8);
        p.widgetBg       = QColor(0xff, 0xff, 0xff);
        p.altRowBg       = QColor(0xe8, 0xee, 0xf4);
        p.deepBg         = QColor(0xe0, 0xe8, 0xf0);
        p.border         = QColor(0xb0, 0xc0, 0xd0);
        p.text           = QColor(0x1a, 0x2a, 0x3a);
        p.dimText        = QColor(0x70, 0x80, 0x90);
        p.accentText     = QColor(0x00, 0x6a, 0x9a);
        p.headerText     = QColor(0x2a, 0x4a, 0x6a);
        p.accent         = QColor(0x00, 0x7a, 0xaa);
        p.headerTop      = QColor(0xd0, 0xdc, 0xe8);
        p.headerMid      = QColor(0xc0, 0xcc, 0xd8);
        p.headerBot      = QColor(0xb0, 0xbc, 0xcc);
        p.headerBorderBot= QColor(0x90, 0xa0, 0xb4);
        return p;
    }

    // ── Global accessor ───────────────────────────────────────────────────

    static const ThemePalette& current()
    {
        return s_current;
    }

    static bool isLight()
    {
        return s_isLight;
    }

    // Call this before applyTheme() to switch palette; UI widgets that read
    // current() on each paint will pick up the new palette automatically.
    static void setLight(bool on)
    {
        s_isLight = on;
        s_current = on ? light() : dark();
    }

private:
    static ThemePalette s_current;
    static bool         s_isLight;
};

inline ThemePalette ThemePalette::s_current = ThemePalette::dark();
inline bool         ThemePalette::s_isLight  = false;

} // namespace AetherSDR
