#include "RotorController.h"

#include <QtMath>
#include <QByteArray>
#include <QHostAddress>

namespace AetherSDR {

RotorController::RotorController(QObject* parent)
    : QObject(parent)
{
}

void RotorController::setTarget(const QString& host, quint16 port)
{
    m_host = host;
    m_port = port;
}

double RotorController::bearing(double myLat, double myLon, double dxLat, double dxLon)
{
    // Great-circle forward bearing formula
    const double phi1   = qDegreesToRadians(myLat);
    const double phi2   = qDegreesToRadians(dxLat);
    const double dLam   = qDegreesToRadians(dxLon - myLon);

    const double y = qSin(dLam) * qCos(phi2);
    const double x = qCos(phi1) * qSin(phi2) - qSin(phi1) * qCos(phi2) * qCos(dLam);

    double theta = qRadiansToDegrees(qAtan2(y, x));
    // Normalize to [0, 360)
    return std::fmod(theta + 360.0, 360.0);
}

void RotorController::sendAzimuth(double azimuth)
{
    // PstRotator XML-over-UDP protocol
    const int az = qRound(azimuth);
    const QByteArray cmd = QString("<PST><CMD>AZ</CMD><PAR>%1</PAR></PST>")
                               .arg(az)
                               .toUtf8();

    const qint64 sent = m_socket.writeDatagram(cmd, QHostAddress(m_host), m_port);
    if (sent != cmd.size()) {
        emit errorOccurred(QString("UDP send failed to %1:%2").arg(m_host).arg(m_port));
        return;
    }
    emit azimuthSent(static_cast<double>(az));
}

void RotorController::pointTo(double myLat, double myLon, double dxLat, double dxLon)
{
    sendAzimuth(bearing(myLat, myLon, dxLat, dxLon));
}

} // namespace AetherSDR
