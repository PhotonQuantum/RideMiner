//
// Created by lightquantum on 2020/12/23.
//

#ifndef PSPPROJ_CONFIG_H
#define PSPPROJ_CONFIG_H
#include <QMap>
#include <QVariant>
#include <QReadWriteLock>

class Config {
private:
    QReadWriteLock muConfig;
    QMap<QString, QVariant> configs;
public:
    static Config &getInstance() {
        static Config instance;
        return instance;
    }

    std::optional<QVariant> get(const QString& key);

    void set(const QString& key, const QVariant& value);

    void unset(const QString& key);

    Config() = default;

    ~Config() = default;

    Config(const Config &) = delete;

    Config &operator=(const Config &) = delete;
};

#endif //PSPPROJ_CONFIG_H
