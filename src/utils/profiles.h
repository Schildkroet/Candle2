#ifndef PROFILES_H
#define PROFILES_H
#include <QStringList>
#include <QStandardPaths>
#include <QDir>

extern const QString default_profile_name;
QStringList getProfileNames();
QString configPathForProfile(QString profile_name);

#endif // PROFILES_H
