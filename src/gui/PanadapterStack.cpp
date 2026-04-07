#include "PanadapterStack.h"
#include "FloatingAppletWindow.h"
#include "PanadapterApplet.h"
#include "SpectrumWidget.h"
#include "core/AppSettings.h"

#include <QVBoxLayout>
#include <QTimer>

namespace AetherSDR {

PanadapterStack::PanadapterStack(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_splitter = new QSplitter(Qt::Vertical, this);
    m_splitter->setHandleWidth(3);
    m_splitter->setChildrenCollapsible(false);
    layout->addWidget(m_splitter);
}

PanadapterApplet* PanadapterStack::addPanadapter(const QString& panId)
{
    if (m_pans.contains(panId))
        return m_pans[panId];

    auto* applet = new PanadapterApplet(m_splitter);
    applet->setPanId(panId);
    applet->spectrumWidget()->setPanIndex(m_pans.size());
    applet->spectrumWidget()->loadSettings();
    m_splitter->addWidget(applet);

    // Equal stretch for all pans
    const int idx = m_splitter->indexOf(applet);
    m_splitter->setStretchFactor(idx, 1);

    m_pans[panId] = applet;

    // First pan becomes active
    if (m_activePanId.isEmpty())
        setActivePan(panId);

    // Equalize sizes whenever a pan is added
    if (m_pans.size() > 1)
        equalizeSizes();

    return applet;
}

void PanadapterStack::removePanadapter(const QString& panId)
{
    // Auto-dock if floating before removing
    if (m_floatingWindows.contains(panId))
        dockPanadapter(panId);

    auto* applet = m_pans.take(panId);
    if (!applet) return;

    delete applet;

    // If active was removed, switch to first remaining
    if (m_activePanId == panId) {
        if (!m_pans.isEmpty())
            setActivePan(m_pans.firstKey());
        else
            m_activePanId.clear();
    }
}

void PanadapterStack::rekey(const QString& oldId, const QString& newId)
{
    if (auto* applet = m_pans.take(oldId)) {
        m_pans[newId] = applet;
        if (m_activePanId == oldId)
            m_activePanId = newId;
    }
}

PanadapterApplet* PanadapterStack::panadapter(const QString& panId) const
{
    return m_pans.value(panId, nullptr);
}

SpectrumWidget* PanadapterStack::spectrum(const QString& panId) const
{
    auto* applet = m_pans.value(panId, nullptr);
    return applet ? applet->spectrumWidget() : nullptr;
}

PanadapterApplet* PanadapterStack::activeApplet() const
{
    return m_pans.value(m_activePanId, nullptr);
}

SpectrumWidget* PanadapterStack::activeSpectrum() const
{
    auto* applet = activeApplet();
    return applet ? applet->spectrumWidget() : nullptr;
}

void PanadapterStack::setActivePan(const QString& panId)
{
    if (m_activePanId == panId) return;
    m_activePanId = panId;

    // Visual indicator: update active border via property (no stylesheet churn)
    for (auto it = m_pans.begin(); it != m_pans.end(); ++it) {
        it.value()->setProperty("activePan", it.key() == panId);
        it.value()->update();
    }

    emit activePanChanged(panId);
}

static void equalizeSplitter(QSplitter* splitter)
{
    const int count = splitter->count();
    if (count < 2) return;
    const int total = (splitter->orientation() == Qt::Horizontal)
                        ? splitter->width() : splitter->height();
    const int each = total / count;
    QList<int> sizes;
    for (int i = 0; i < count; ++i) {
        sizes.append(each);
        // Recurse into nested splitters
        if (auto* nested = qobject_cast<QSplitter*>(splitter->widget(i)))
            equalizeSplitter(nested);
    }
    splitter->setSizes(sizes);
}

void PanadapterStack::equalizeSizes()
{
    equalizeSplitter(m_splitter);
}

void PanadapterStack::rearrangeLayout(const QString& layoutId)
{
    // Auto-dock all floating pans before rebuilding the splitter layout
    dockAll();

    // Collect applets in order
    QList<PanadapterApplet*> applets = m_pans.values();
    if (applets.isEmpty()) return;

    // Remove all applets from current splitter (don't delete them)
    for (auto* a : applets)
        a->setParent(nullptr);

    // Hide + remove old splitter from layout, defer deletion
    m_splitter->hide();
    layout()->removeWidget(m_splitter);
    m_splitter->deleteLater();
    m_splitter = new QSplitter(Qt::Vertical, this);
    m_splitter->setHandleWidth(3);
    m_splitter->setChildrenCollapsible(false);
    layout()->addWidget(m_splitter);

    if (layoutId == "2h" && applets.size() >= 2) {
        m_splitter->setOrientation(Qt::Horizontal);
        m_splitter->addWidget(applets[0]);
        m_splitter->addWidget(applets[1]);
    }
    else if (layoutId == "2h1" && applets.size() >= 3) {
        // A|B on top, C on bottom
        auto* topSplit = new QSplitter(Qt::Horizontal);
        topSplit->setHandleWidth(3);
        topSplit->setChildrenCollapsible(false);
        topSplit->addWidget(applets[0]);
        topSplit->addWidget(applets[1]);
        m_splitter->addWidget(topSplit);
        m_splitter->addWidget(applets[2]);
    }
    else if (layoutId == "12h" && applets.size() >= 3) {
        // A on top, B|C on bottom
        m_splitter->addWidget(applets[0]);
        auto* botSplit = new QSplitter(Qt::Horizontal);
        botSplit->setHandleWidth(3);
        botSplit->setChildrenCollapsible(false);
        botSplit->addWidget(applets[1]);
        botSplit->addWidget(applets[2]);
        m_splitter->addWidget(botSplit);
    }
    else if (layoutId == "3v" && applets.size() >= 3) {
        // A / B / C vertical stack
        m_splitter->addWidget(applets[0]);
        m_splitter->addWidget(applets[1]);
        m_splitter->addWidget(applets[2]);
    }
    else if (layoutId == "2x2" && applets.size() >= 4) {
        // A|B on top, C|D on bottom
        auto* topSplit = new QSplitter(Qt::Horizontal);
        topSplit->setHandleWidth(3);
        topSplit->setChildrenCollapsible(false);
        topSplit->addWidget(applets[0]);
        topSplit->addWidget(applets[1]);
        m_splitter->addWidget(topSplit);
        auto* botSplit = new QSplitter(Qt::Horizontal);
        botSplit->setHandleWidth(3);
        botSplit->setChildrenCollapsible(false);
        botSplit->addWidget(applets[2]);
        botSplit->addWidget(applets[3]);
        m_splitter->addWidget(botSplit);
    }
    else if (layoutId == "4v" && applets.size() >= 4) {
        // A / B / C / D vertical stack
        m_splitter->addWidget(applets[0]);
        m_splitter->addWidget(applets[1]);
        m_splitter->addWidget(applets[2]);
        m_splitter->addWidget(applets[3]);
    }
    else {
        // Default: vertical stack (2v, 1, or fallback)
        for (auto* a : applets)
            m_splitter->addWidget(a);
    }

    // Defer equalize until the new splitter has been laid out by Qt
    QTimer::singleShot(0, this, [this]() { equalizeSizes(); });
}

void PanadapterStack::removeAll()
{
    // Close all floating windows first
    for (auto* win : m_floatingWindows)
        delete win;
    m_floatingWindows.clear();
    m_floatSplitterIndex.clear();

    qDeleteAll(m_pans);
    m_pans.clear();
    m_activePanId.clear();

    // Delete the old splitter and create a fresh one
    delete m_splitter;
    m_splitter = new QSplitter(Qt::Vertical, this);
    m_splitter->setHandleWidth(3);
    m_splitter->setChildrenCollapsible(false);
    layout()->addWidget(m_splitter);
}

void PanadapterStack::applyLayout(const QString& layoutId, const QStringList& panIds)
{
    // Build structure based on layout ID.
    // Each layout adds applets to the correct splitter position.
    // panIds must have at least as many entries as the layout requires.

    if (layoutId == "1" && panIds.size() >= 1) {
        // Single pan — just add to vertical splitter
        addPanadapter(panIds[0]);
    }
    else if (layoutId == "2v" && panIds.size() >= 2) {
        // A / B — vertical stack
        addPanadapter(panIds[0]);
        addPanadapter(panIds[1]);
    }
    else if (layoutId == "2h" && panIds.size() >= 2) {
        // A | B — horizontal split
        // Replace the vertical splitter orientation
        m_splitter->setOrientation(Qt::Horizontal);
        addPanadapter(panIds[0]);
        addPanadapter(panIds[1]);
    }
    else if (layoutId == "2h1" && panIds.size() >= 3) {
        // A|B / C — horizontal top, single bottom
        auto* topSplit = new QSplitter(Qt::Horizontal);
        topSplit->setHandleWidth(3);
        topSplit->setChildrenCollapsible(false);
        m_splitter->addWidget(topSplit);

        auto* a = new PanadapterApplet(topSplit);
        a->setPanId(panIds[0]);
        a->spectrumWidget()->setPanIndex(0);
        a->spectrumWidget()->loadSettings();
        topSplit->addWidget(a);
        m_pans[panIds[0]] = a;

        auto* b = new PanadapterApplet(topSplit);
        b->setPanId(panIds[1]);
        b->spectrumWidget()->setPanIndex(1);
        b->spectrumWidget()->loadSettings();
        topSplit->addWidget(b);
        m_pans[panIds[1]] = b;

        auto* c = addPanadapter(panIds[2]);
        Q_UNUSED(c);

        // Equal row heights
        m_splitter->setStretchFactor(0, 1);
        m_splitter->setStretchFactor(1, 1);

        if (m_activePanId.isEmpty()) setActivePan(panIds[0]);
    }
    else if (layoutId == "12h" && panIds.size() >= 3) {
        // A / B|C — single top, horizontal bottom
        addPanadapter(panIds[0]);

        auto* botSplit = new QSplitter(Qt::Horizontal);
        botSplit->setHandleWidth(3);
        botSplit->setChildrenCollapsible(false);
        m_splitter->addWidget(botSplit);

        auto* b = new PanadapterApplet(botSplit);
        b->setPanId(panIds[1]);
        b->spectrumWidget()->setPanIndex(1);
        b->spectrumWidget()->loadSettings();
        botSplit->addWidget(b);
        m_pans[panIds[1]] = b;

        auto* c = new PanadapterApplet(botSplit);
        c->setPanId(panIds[2]);
        c->spectrumWidget()->setPanIndex(2);
        c->spectrumWidget()->loadSettings();
        botSplit->addWidget(c);
        m_pans[panIds[2]] = c;

        // Equal row heights
        m_splitter->setStretchFactor(0, 1);
        m_splitter->setStretchFactor(1, 1);

        if (m_activePanId.isEmpty()) setActivePan(panIds[0]);
    }
    else if (layoutId == "2x2" && panIds.size() >= 4) {
        // A|B / C|D — 2×2 grid
        auto* topSplit = new QSplitter(Qt::Horizontal);
        topSplit->setHandleWidth(3);
        topSplit->setChildrenCollapsible(false);
        m_splitter->addWidget(topSplit);

        auto* a = new PanadapterApplet(topSplit);
        a->setPanId(panIds[0]);
        a->spectrumWidget()->setPanIndex(0);
        a->spectrumWidget()->loadSettings();
        topSplit->addWidget(a);
        m_pans[panIds[0]] = a;

        auto* b = new PanadapterApplet(topSplit);
        b->setPanId(panIds[1]);
        b->spectrumWidget()->setPanIndex(1);
        b->spectrumWidget()->loadSettings();
        topSplit->addWidget(b);
        m_pans[panIds[1]] = b;

        auto* botSplit = new QSplitter(Qt::Horizontal);
        botSplit->setHandleWidth(3);
        botSplit->setChildrenCollapsible(false);
        m_splitter->addWidget(botSplit);

        auto* c = new PanadapterApplet(botSplit);
        c->setPanId(panIds[2]);
        c->spectrumWidget()->setPanIndex(2);
        c->spectrumWidget()->loadSettings();
        botSplit->addWidget(c);
        m_pans[panIds[2]] = c;

        auto* d = new PanadapterApplet(botSplit);
        d->setPanId(panIds[3]);
        d->spectrumWidget()->setPanIndex(3);
        d->spectrumWidget()->loadSettings();
        botSplit->addWidget(d);
        m_pans[panIds[3]] = d;

        m_splitter->setStretchFactor(0, 1);
        m_splitter->setStretchFactor(1, 1);

        if (m_activePanId.isEmpty()) setActivePan(panIds[0]);
    }
    else if (layoutId == "3v" && panIds.size() >= 3) {
        // A / B / C — vertical stack
        addPanadapter(panIds[0]);
        addPanadapter(panIds[1]);
        addPanadapter(panIds[2]);
    }
    else if (layoutId == "4v" && panIds.size() >= 4) {
        // A / B / C / D — vertical stack
        addPanadapter(panIds[0]);
        addPanadapter(panIds[1]);
        addPanadapter(panIds[2]);
        addPanadapter(panIds[3]);
    }
}

void PanadapterStack::floatPanadapter(const QString& panId)
{
    auto* applet = m_pans.value(panId);
    if (!applet || m_floatingWindows.contains(panId)) return;

    // Save the current splitter index for re-insertion on dock
    m_floatSplitterIndex[panId] = m_splitter->indexOf(applet);

    // Reparent applet out of the splitter
    applet->setParent(nullptr);

    // Build a human-readable title from the pan's title label
    QString title = applet->property("sliceTitle").toString();
    if (title.isEmpty()) title = QStringLiteral("Panadapter %1").arg(panId);

    // Create floating window (pass 'this' as parent → Qt::Tool window)
    auto* win = new FloatingAppletWindow(panId, title, applet, this);
    m_floatingWindows[panId] = win;
    applet->setFloating(true);

    connect(win, &FloatingAppletWindow::dockRequested,
            this, &PanadapterStack::dockPanadapter);

    win->show();
    QTimer::singleShot(50, win, [win]() { win->restoreGeometry(); });

    // Persist floating state
    auto& s = AppSettings::instance();
    s.setValue(QStringLiteral("FloatingPan_%1_IsFloating").arg(panId), "True");
    s.save();
}

void PanadapterStack::dockPanadapter(const QString& panId)
{
    if (!m_floatingWindows.contains(panId)) return;

    auto* applet = m_pans.value(panId);
    auto* win = m_floatingWindows.value(panId);

    if (applet) {
        // Reparent back into the splitter
        applet->setParent(m_splitter);

        // Re-insert at original position or append at end
        int targetIdx = m_floatSplitterIndex.value(panId, m_splitter->count());
        if (targetIdx > m_splitter->count()) targetIdx = m_splitter->count();
        m_splitter->insertWidget(targetIdx, applet);
        m_splitter->setStretchFactor(m_splitter->indexOf(applet), 1);

        applet->setFloating(false);
        applet->show();
    }

    // Clean up the floating window
    if (win) {
        win->saveGeometry();
        win->hide();
        win->deleteLater();
    }
    m_floatingWindows.remove(panId);
    m_floatSplitterIndex.remove(panId);

    // Persist floating state
    auto& s = AppSettings::instance();
    s.setValue(QStringLiteral("FloatingPan_%1_IsFloating").arg(panId), "False");
    s.save();

    // Re-equalize after docking
    if (m_splitter->count() > 1)
        QTimer::singleShot(0, this, [this]() { equalizeSizes(); });
}

void PanadapterStack::dockAll()
{
    const QStringList ids = m_floatingWindows.keys();
    for (const QString& id : ids)
        dockPanadapter(id);
}

} // namespace AetherSDR
