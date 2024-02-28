#include "profiles.h"
#include <QUrl>

// chosen for backward compatibility with pre-profiles versions, which always stored settings in `settings.ini`
const QString default_profile_name = "settings";

QStringList getProfileNames() {
    const QDir configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    const QStringList configFiles = configDir.entryList({"*.ini"}, QDir::Files);
    QStringList rv;
    for (QString configFile : configFiles) {
        rv << QUrl::fromPercentEncoding(configFile.replace(QRegExp("\\.ini$"), "").toUtf8());
    }
    return rv;
}

QString configPathForProfile(QString profile_name) {
    const QDir configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    return configDir.filePath(QUrl::toPercentEncoding(profile_name) + ".ini");
}
