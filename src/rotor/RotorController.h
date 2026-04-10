#pragma once

#include <QObject>
#include <QString>
#include <QUdpSocket>

namespace AetherSDR {

// Controls an antenna rotator via the PstRotator UDP XML protocol.
//
// Usage:
//   RotorController rc;
//   rc.setTarget("127.0.0.1", 12000);
//   rc.pointTo(myLat, myLon, dxLat, dxLon);
//
// The PstRotator command format is:
//   <PST><CMD>AZ</CMD><PAR>42</PAR></PST>
class RotorController : public QObject {
    Q_OBJECT

public:
    explicit RotorController(QObject* parent = nullptr);

    void setTarget(const QString& host, quint16 port);
    QString host() const { return m_host; }
    quint16 port() const { return m_port; }

    // Compute great-circle bearing from operator QTH to DX station
    // and send the azimuth command to PstRotator.
    void pointTo(double myLat, double myLon, double dxLat, double dxLon);

    // Send a specific azimuth (0–360°). Used for test button ("AZ=0°").
    void sendAzimuth(double azimuth);

    // Compute forward bearing in [0, 360) without sending.
    static double bearing(double myLat, double myLon, double dxLat, double dxLon);

signals:
    void azimuthSent(double azimuth);
    void errorOccurred(const QString& message);

private:
    QUdpSocket m_socket;
    QString    m_host{"127.0.0.1"};
    quint16    m_port{12000};
};

} // namespace AetherSDR
