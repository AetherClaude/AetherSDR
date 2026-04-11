#pragma once

#include <QSlider>
#include <QComboBox>
#include <QAbstractItemView>
#include <QLabel>
#include <QMouseEvent>
#include <QWheelEvent>

// Global lock for sidebar controls — when locked, sliders, combo boxes,
// and scrollable labels ignore wheel/mouse events so the user can scroll
// the applet panel without accidentally changing values. (#745)
class ControlsLock {
public:
    static bool isLocked() { return s_locked; }
    static void setLocked(bool locked) { s_locked = locked; }
private:
    static inline bool s_locked = false;
};

// QSlider subclass that always consumes wheel events, even at min/max
// boundaries. Prevents scroll from propagating to parent widgets (e.g.
// SpectrumWidget tuning the VFO when a slider bottoms out). (#570)
// When controls are locked (#745), ignores wheel events and lets the
// parent scroll area handle them.
class GuardedSlider : public QSlider {
public:
    using QSlider::QSlider;
    // On Linux with Qt's Fusion style, the slider handle hit-test rect is
    // computed from the groove geometry only, ignoring negative CSS margins.
    // The shared stylesheet uses margin:-3px on a 4px groove, leaving only a
    // 4px clickable zone for a 10px handle. Guarantee at least 14px height so
    // the full handle always falls within the interactive area. (#1145)
    QSize sizeHint() const override {
        QSize s = QSlider::sizeHint();
        if (orientation() == Qt::Horizontal && s.height() < 14)
            s.setHeight(14);
        return s;
    }
    QSize minimumSizeHint() const override {
        QSize s = QSlider::minimumSizeHint();
        if (orientation() == Qt::Horizontal && s.height() < 14)
            s.setHeight(14);
        return s;
    }
    void mousePressEvent(QMouseEvent* ev) override {
        if (ControlsLock::isLocked()) {
            ev->ignore();
            return;
        }
        QSlider::mousePressEvent(ev);
    }
    void wheelEvent(QWheelEvent* ev) override {
        if (ControlsLock::isLocked()) {
            ev->ignore();
            return;
        }
        // Use singleStep (default 1) instead of pageStep (default 10) so
        // that mouse-wheel adjustments are fine-grained (#1026).
        int delta = ev->angleDelta().y();
        if (delta != 0)
            setValue(value() + (delta > 0 ? singleStep() : -singleStep()));
        ev->accept();
    }
};

// QComboBox subclass that only responds to wheel events when the dropdown
// popup is open. Prevents accidental value changes when scrolling the applet
// panel, but allows normal wheel scrolling through the list when the user
// has clicked to open the dropdown. (#570, #676)
// When controls are locked (#745), also blocks mouse press to prevent
// opening the dropdown.
class GuardedComboBox : public QComboBox {
public:
    using QComboBox::QComboBox;
    void wheelEvent(QWheelEvent* ev) override {
        if (ControlsLock::isLocked()) {
            ev->ignore();
            return;
        }
        if (view() && view()->isVisible())
            QComboBox::wheelEvent(ev);  // popup open — scroll the list
        else
            ev->ignore();  // popup closed — let parent handle scroll
    }
    void mousePressEvent(QMouseEvent* ev) override {
        if (ControlsLock::isLocked()) {
            ev->ignore();
            return;
        }
        QComboBox::mousePressEvent(ev);
    }
};

// QLabel subclass that emits scrolled(int steps) on wheel events and
// always consumes them. Used for RIT/XIT/pitch numeric displays. (#619)
// When controls are locked (#745), ignores wheel events.
class ScrollableLabel : public QLabel {
    Q_OBJECT
public:
    using QLabel::QLabel;
    void wheelEvent(QWheelEvent* ev) override {
        if (ControlsLock::isLocked()) {
            ev->ignore();
            return;
        }
        int delta = ev->angleDelta().y();
        if (delta > 0) emit scrolled(1);
        else if (delta < 0) emit scrolled(-1);
        ev->accept();
    }
signals:
    void scrolled(int direction);
};
