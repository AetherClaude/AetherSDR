#include "CountryCoordinates.h"

namespace AetherSDR {

const CountryCoordinates& CountryCoordinates::instance()
{
    static CountryCoordinates inst;
    return inst;
}

CountryCoordinates::CountryCoordinates()
{
    // DXCC prefix → {lat, lon}, country name
    // Covers ~160 most-worked entities. Sorted by prefix for readability.
    struct Raw { const char* prefix; double lat; double lon; const char* name; };
    static constexpr Raw table[] = {
        // North America
        {"K",    38.0,  -97.0,  "United States"},
        {"W",    38.0,  -97.0,  "United States"},
        {"N",    38.0,  -97.0,  "United States"},
        {"AA",   38.0,  -97.0,  "United States"},
        {"VE",   56.0,  -96.0,  "Canada"},
        {"VA",   56.0,  -96.0,  "Canada"},
        {"XE",   23.0, -102.0,  "Mexico"},
        {"XF",   23.0, -102.0,  "Mexico"},
        {"TI",    9.9,  -84.1,  "Costa Rica"},
        {"HP",    8.0,  -80.0,  "Panama"},
        {"YN",   12.9,  -85.6,  "Nicaragua"},
        {"HQ",   15.2,  -86.2,  "Honduras"},
        {"YS",   13.7,  -88.9,  "El Salvador"},
        {"TG",   15.5,  -90.3,  "Guatemala"},
        {"V3",   17.2,  -88.5,  "Belize"},
        {"KP4",  18.2,  -66.5,  "Puerto Rico"},
        {"KP2",  18.3,  -65.0,  "U.S. Virgin Islands"},
        {"KP1",  18.1,  -64.9,  "Navassa Island"},
        {"KH6",  21.3, -157.8,  "Hawaii"},
        {"KL7",  64.2, -153.0,  "Alaska"},
        {"KG4",  19.9,  -75.1,  "Guantanamo Bay"},
        // Caribbean
        {"CO",   21.5,  -79.5,  "Cuba"},
        {"HH",   18.9,  -72.3,  "Haiti"},
        {"HI",   19.0,  -70.7,  "Dominican Republic"},
        {"J3",   12.1,  -61.7,  "Grenada"},
        {"J6",   13.9,  -60.9,  "St. Lucia"},
        {"J7",   15.4,  -61.4,  "Dominica"},
        {"J8",   13.3,  -61.2,  "St. Vincent"},
        {"V2",   17.1,  -61.8,  "Antigua and Barbuda"},
        {"V4",   17.3,  -62.7,  "St. Kitts and Nevis"},
        {"VP5",  21.8,  -71.9,  "Turks and Caicos"},
        {"VP9",  32.3,  -64.7,  "Bermuda"},
        {"FG",   16.3,  -61.6,  "Guadeloupe"},
        {"FM",   14.7,  -61.0,  "Martinique"},
        {"8P",   13.2,  -59.6,  "Barbados"},
        {"9Y",   10.7,  -61.3,  "Trinidad and Tobago"},
        {"PJ4",  12.2,  -68.3,  "Bonaire"},
        {"PJ2",  12.1,  -68.9,  "Curacao"},
        // South America
        {"LU",  -34.0,  -64.0,  "Argentina"},
        {"PY",  -15.0,  -53.0,  "Brazil"},
        {"CE",  -30.0,  -71.0,  "Chile"},
        {"HC",   -1.8,  -78.2,  "Ecuador"},
        {"OA",  -10.0,  -76.0,  "Peru"},
        {"YV",    8.0,  -66.0,  "Venezuela"},
        {"HK",    4.0,  -72.0,  "Colombia"},
        {"CP",  -17.0,  -65.0,  "Bolivia"},
        {"ZP",  -23.0,  -58.0,  "Paraguay"},
        {"CX",  -33.0,  -56.0,  "Uruguay"},
        {"GY",    4.9,  -58.9,  "Guyana"},
        {"FY",    4.0,  -53.0,  "French Guiana"},
        {"PZ",    4.0,  -56.0,  "Suriname"},
        // Europe
        {"G",    54.0,   -2.0,  "England"},
        {"GM",   57.0,   -4.0,  "Scotland"},
        {"GW",   52.0,   -3.5,  "Wales"},
        {"GI",   54.7,   -6.5,  "Northern Ireland"},
        {"EI",   53.0,   -8.0,  "Ireland"},
        {"PA",   52.4,    5.3,  "Netherlands"},
        {"DL",   51.5,   10.0,  "Germany"},
        {"F",    46.0,    2.0,  "France"},
        {"I",    42.8,   12.8,  "Italy"},
        {"EA",   40.0,   -4.0,  "Spain"},
        {"EA6",  39.6,    3.0,  "Balearic Islands"},
        {"EA8",  28.2,  -15.6,  "Canary Islands"},
        {"EA9",  35.9,   -5.3,  "Ceuta & Melilla"},
        {"ON",   50.5,    4.4,  "Belgium"},
        {"LX",   49.8,    6.1,  "Luxembourg"},
        {"OZ",   56.0,   10.0,  "Denmark"},
        {"OE",   47.5,   14.5,  "Austria"},
        {"HB",   47.0,    8.0,  "Switzerland"},
        {"HB0",  47.2,    9.6,  "Liechtenstein"},
        {"SM",   62.0,   15.0,  "Sweden"},
        {"OH",   64.0,   26.0,  "Finland"},
        {"LA",   65.0,   13.0,  "Norway"},
        {"TF",   65.0,  -18.0,  "Iceland"},
        {"OX",   72.0,  -40.0,  "Greenland"},
        {"SP",   52.0,   20.0,  "Poland"},
        {"OK",   50.0,   15.5,  "Czech Republic"},
        {"OM",   48.7,   19.7,  "Slovakia"},
        {"HA",   47.0,   19.0,  "Hungary"},
        {"YO",   45.0,   25.0,  "Romania"},
        {"LZ",   42.8,   25.2,  "Bulgaria"},
        {"SV",   39.0,   22.0,  "Greece"},
        {"SV5",  36.2,   28.1,  "Dodecanese"},
        {"SV9",  35.3,   25.0,  "Crete"},
        {"Z3",   41.6,   21.7,  "North Macedonia"},
        {"YU",   44.0,   21.0,  "Serbia"},
        {"9A",   45.1,   16.4,  "Croatia"},
        {"S5",   46.1,   14.9,  "Slovenia"},
        {"T9",   44.0,   17.5,  "Bosnia-Herzegovina"},
        {"Z6",   42.6,   21.1,  "Kosovo"},
        {"4O",   42.9,   19.5,  "Montenegro"},
        {"ZA",   41.0,   20.0,  "Albania"},
        {"UR",   49.0,   32.0,  "Ukraine"},
        {"UT",   49.0,   32.0,  "Ukraine"},
        {"UX",   49.0,   32.0,  "Ukraine"},
        {"UB",   49.0,   32.0,  "Ukraine"},
        {"EW",   53.0,   28.0,  "Belarus"},
        {"ES",   58.5,   25.0,  "Estonia"},
        {"YL",   57.0,   25.0,  "Latvia"},
        {"LY",   56.0,   24.0,  "Lithuania"},
        {"EU",   53.0,   27.0,  "Belarus"},
        {"RU",   60.0,   90.0,  "Russia"},
        {"UA",   60.0,   90.0,  "Russia"},
        {"RA",   60.0,   90.0,  "Russia"},
        {"RK",   60.0,   90.0,  "Russia"},
        {"RN",   60.0,   90.0,  "Russia"},
        {"UA9",  55.0,   65.0,  "Asiatic Russia"},
        {"UA0",  60.0,  105.0,  "Asiatic Russia"},
        // Middle East
        {"4X",   31.5,   35.0,  "Israel"},
        {"4Z",   31.5,   35.0,  "Israel"},
        {"YK",   35.0,   38.0,  "Syria"},
        {"OD",   33.9,   35.5,  "Lebanon"},
        {"JY",   31.0,   36.0,  "Jordan"},
        {"A4",   21.5,   57.0,  "Oman"},
        {"A6",   24.5,   54.5,  "United Arab Emirates"},
        {"A7",   25.3,   51.2,  "Qatar"},
        {"A9",   26.0,   50.5,  "Bahrain"},
        {"HZ",   24.0,   45.0,  "Saudi Arabia"},
        {"YI",   33.0,   44.0,  "Iraq"},
        {"EP",   32.0,   53.0,  "Iran"},
        {"9K",   29.3,   47.7,  "Kuwait"},
        {"7Z",   24.0,   45.0,  "Saudi Arabia"},
        {"TA",   39.0,   35.0,  "Turkey"},
        // Asia
        {"JA",   36.0,  138.0,  "Japan"},
        {"HL",   37.0,  127.5,  "South Korea"},
        {"DS",   37.0,  127.5,  "South Korea"},
        {"BY",   35.0,  105.0,  "China"},
        {"BA",   35.0,  105.0,  "China"},
        {"BG",   35.0,  105.0,  "China"},
        {"VU",   20.0,   77.0,  "India"},
        {"AP",   30.0,   70.0,  "Pakistan"},
        {"9N",   28.0,   84.0,  "Nepal"},
        {"S2",   23.0,   90.0,  "Bangladesh"},
        {"4S",    7.9,   80.8,  "Sri Lanka"},
        {"XV",   16.0,  108.0,  "Vietnam"},
        {"XW",   18.0,  103.0,  "Laos"},
        {"XU",   12.5,  104.9,  "Cambodia"},
        {"HS",   15.0,  101.0,  "Thailand"},
        {"9M2",   4.0,  109.0,  "West Malaysia"},
        {"9M6",   4.0,  117.0,  "East Malaysia"},
        {"YB",   -5.0,  120.0,  "Indonesia"},
        {"DU",   13.0,  122.0,  "Philippines"},
        {"VS6",  22.3,  114.2,  "Hong Kong"},
        {"VR",   22.3,  114.2,  "Hong Kong"},
        {"XX9",  22.2,  113.5,  "Macao"},
        {"BV",   23.5,  121.0,  "Taiwan"},
        {"JD1",  27.1,  142.2,  "Ogasawara"},
        {"7J",   27.1,  142.2,  "Ogasawara"},
        {"JT",   47.0,  105.0,  "Mongolia"},
        // Pacific / Oceania
        {"VK",  -25.0,  135.0,  "Australia"},
        {"ZL",  -41.0,  174.0,  "New Zealand"},
        {"ZK1",  -21.2, -159.8, "North Cook Islands"},
        {"ZK2",  -19.0, -169.9, "Niue"},
        {"KH0",  15.2,  145.8,  "Mariana Islands"},
        {"KH2",  13.5,  144.8,  "Guam"},
        {"KH8",  -14.3, -170.7, "American Samoa"},
        {"KH9",  19.3,  166.6,  "Wake Island"},
        {"V6",    7.0,  150.0,  "Micronesia"},
        {"YJ",  -17.7,  168.3,  "Vanuatu"},
        {"ZK3",  -9.4, -171.9,  "Tokelau"},
        {"A3",  -21.2, -175.2,  "Tonga"},
        {"3D2",  -18.1,  178.4, "Fiji"},
        {"H4",   -8.1,  157.0,  "Solomon Islands"},
        {"T2",    8.0,  179.2,  "Tuvalu"},
        {"T3",    1.9,  157.4,  "Kiribati"},
        {"FO",  -17.5, -149.6,  "French Polynesia"},
        {"FK",  -21.3,  165.5,  "New Caledonia"},
        {"P2",   -6.3,  147.1,  "Papua New Guinea"},
        {"YB0",  -6.2,  106.8,  "Indonesia"},
        // Africa
        {"ZS",  -29.0,   25.0,  "South Africa"},
        {"EA4",  40.4,   -3.7,  "Spain"},  // duplicate callsign area
        {"TZ",   17.0,   -4.0,  "Mali"},
        {"TU",    5.3,   -4.0,  "Ivory Coast"},
        {"5N",    9.0,    8.0,  "Nigeria"},
        {"5B",   35.1,   33.4,  "Cyprus"},
        {"9G",    7.5,   -1.5,  "Ghana"},
        {"9J",  -15.0,   28.0,  "Zambia"},
        {"9Q",   -4.0,   25.0,  "DR Congo"},
        {"7P",  -29.5,   28.5,  "Lesotho"},
        {"7Q",  -13.2,   34.3,  "Malawi"},
        {"3B8",  -20.3,   57.5, "Mauritius"},
        {"FR",  -21.1,   55.5,  "Reunion"},
        {"FT5",  -37.8,   77.5, "Amsterdam & St. Paul"},
        {"5R",  -20.0,   47.0,  "Madagascar"},
        {"C9",  -18.0,   35.0,  "Mozambique"},
        {"Z2",  -20.0,   30.0,  "Zimbabwe"},
        {"ZS8",  -46.9,   37.9, "Marion Island"},
        {"ET",    8.0,   38.0,  "Ethiopia"},
        {"ST",   15.0,   30.0,  "Sudan"},
        {"SU",   27.0,   30.0,  "Egypt"},
        {"CN",   32.0,   -5.0,  "Morocco"},
        {"7X",   28.0,    3.0,  "Algeria"},
        {"3V",   34.0,    9.0,  "Tunisia"},
        {"5A",   27.0,   17.0,  "Libya"},
        {"EA9",  35.9,   -5.3,  "Ceuta & Melilla"},
        {"D4",   16.9,  -24.0,  "Cape Verde"},
        {"J5",   11.9,  -15.6,  "Guinea-Bissau"},
        {"6W",   14.5,  -14.5,  "Senegal"},
        {"EL",    7.0,  -10.0,  "Liberia"},
        {"5V",    8.6,    0.8,  "Togo"},
        {"TY",   10.0,    2.0,  "Benin"},
        {"5T",   20.0,  -11.5,  "Mauritania"},
    };

    for (const auto& r : table) {
        m_table.insert(QString(r.prefix), Entry{{r.lat, r.lon}, QString(r.name)});
    }
}

QString CountryCoordinates::prefixFor(const QString& callsign) const
{
    // Try longest prefix first (up to 4 chars), descending
    const int maxLen = qMin(callsign.length(), 4);
    for (int len = maxLen; len >= 1; --len) {
        const QString candidate = callsign.left(len).toUpper();
        if (m_table.contains(candidate))
            return candidate;
    }
    return {};
}

bool CountryCoordinates::lookup(const QString& callsign, GeoCoord& out) const
{
    const QString prefix = prefixFor(callsign);
    if (prefix.isEmpty())
        return false;
    out = m_table.value(prefix).coord;
    return true;
}

QString CountryCoordinates::countryName(const QString& prefix) const
{
    return m_table.value(prefix).name;
}

} // namespace AetherSDR
