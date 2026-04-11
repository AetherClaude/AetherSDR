#pragma once

// Shared combo box styling for consistent down-arrow appearance across all
// QComboBox instances in AetherSDR. Use applyComboStyle(combo) on any
// QComboBox to get the current-theme look with painted down-arrow.

#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QPixmap>
#include <QPainter>
#include "ThemePalette.h"

namespace AetherSDR {

// Generate a small down-arrow PNG whose colour matches the active palette.
// The arrow is re-generated whenever the theme changes (different cache path).
inline QString comboArrowPath()
{
    const ThemePalette& pal = ThemePalette::current();
    // Separate cache file per theme so switching themes repaints the arrow.
    const QString suffix = ThemePalette::isLight() ? "light" : "dark";
    QString path = QDir::temp().filePath(
        QString("aethersdr_combo_arrow_%1.png").arg(suffix));
    if (QFile::exists(path)) return path;
    QPixmap pm(8, 6);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(pal.headerText);
    const QPointF tri[] = {{0, 0}, {8, 0}, {4, 6}};
    p.drawPolygon(tri, 3);
    p.end();
    pm.save(path, "PNG");
    return path;
}

// Standard combo box stylesheet built from the active theme palette.
inline QString comboStyleSheet()
{
    const ThemePalette& pal = ThemePalette::current();
    return QString(
        "QComboBox { background: %1; color: %2; border: 1px solid %3;"
        " padding: 2px 2px 2px 4px; border-radius: 2px; }"
        "QComboBox::drop-down { border: none; width: 14px; }"
        "QComboBox::down-arrow { image: url(%4); width: 8px; height: 6px; }"
        "QComboBox QAbstractItemView { background: %1; color: %2;"
        " selection-background-color: %5; }")
        .arg(pal.widgetBg.name(),
             pal.text.name(),
             pal.border.name(),
             comboArrowPath(),
             pal.accent.name());
}

// Apply the current-theme style to a combo box.
inline void applyComboStyle(QComboBox* combo)
{
    combo->setStyleSheet(comboStyleSheet());
}

} // namespace AetherSDR
