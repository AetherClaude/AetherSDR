#pragma once

#include <QWidget>
#include <QMap>
#include <QSplitter>

namespace AetherSDR {

class FloatingAppletWindow;
class PanadapterApplet;
class SpectrumWidget;

// Vertical stack of N PanadapterApplet instances, each showing an
// independent FFT + waterfall for a different panadapter on the radio.
// Single-pan mode: one applet fills the stack (no visible divider).
class PanadapterStack : public QWidget {
    Q_OBJECT

public:
    explicit PanadapterStack(QWidget* parent = nullptr);

    // Add/remove panadapter displays
    PanadapterApplet* addPanadapter(const QString& panId);
    void removePanadapter(const QString& panId);
    void removeAll();  // remove all applets and reset splitter
    void rekey(const QString& oldId, const QString& newId);

    // Layout: rebuild splitter structure for a given layout ID
    // layoutId: "1", "2v", "2h", "2h1", "12h", "2x2"
    // panIds: the pan IDs to place in order (A, B, C, D)
    void applyLayout(const QString& layoutId, const QStringList& panIds);

    // Float/dock panadapters into standalone windows
    void floatPanadapter(const QString& panId);
    void dockPanadapter(const QString& panId);
    void dockAll();  // dock all floating pans (e.g. before layout change)
    bool isPanFloating(const QString& panId) const { return m_floatingWindows.contains(panId); }

    // Accessors
    PanadapterApplet* panadapter(const QString& panId) const;
    SpectrumWidget* spectrum(const QString& panId) const;
    int count() const { return m_pans.size(); }
    QList<PanadapterApplet*> allApplets() const { return m_pans.values(); }

    // Active pan (determines which pan the applet column shows controls for)
    QString activePanId() const { return m_activePanId; }
    PanadapterApplet* activeApplet() const;
    SpectrumWidget* activeSpectrum() const;
    void setActivePan(const QString& panId);
    void setSplitterOrientation(Qt::Orientation o) { m_splitter->setOrientation(o); }
    void equalizeSizes();
    void rearrangeLayout(const QString& layoutId);

signals:
    void activePanChanged(const QString& panId);

private:
    QSplitter* m_splitter{nullptr};
    QMap<QString, PanadapterApplet*> m_pans;
    QMap<QString, FloatingAppletWindow*> m_floatingWindows;
    QMap<QString, int> m_floatSplitterIndex;  // saved splitter position at pop-out time
    QString m_activePanId;
};

} // namespace AetherSDR
