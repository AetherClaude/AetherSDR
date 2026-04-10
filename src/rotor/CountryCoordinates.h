#pragma once

#include <QString>
#include <QHash>

namespace AetherSDR {

struct GeoCoord {
    double lat{0.0};
    double lon{0.0};
};

// Maps DXCC prefixes to approximate country-center coordinates.
// Lookup via prefixFor(callsign) does longest-prefix matching so
// "JA1ZZZ" resolves to "JA" without a full callsign database.
class CountryCoordinates {
public:
    static const CountryCoordinates& instance();

    // Returns the DXCC prefix key for a callsign (longest prefix match).
    // Returns empty string if no match found.
    QString prefixFor(const QString& callsign) const;

    // Looks up coordinates for a callsign. Returns false if not found.
    bool lookup(const QString& callsign, GeoCoord& out) const;

    // Returns the country name for a prefix, or empty string.
    QString countryName(const QString& prefix) const;

private:
    CountryCoordinates();

    struct Entry {
        GeoCoord coord;
        QString  name;
    };

    QHash<QString, Entry> m_table;   // prefix → entry
};

} // namespace AetherSDR
