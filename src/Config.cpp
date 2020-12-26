//
// Created by lightquantum on 2020/12/23.
//

#include "Config.h"

std::optional<QVariant> Config::get(const QString &key) {
    muConfig.lockForRead();
    if (!configs.contains(key)) {
        muConfig.unlock();
        return {};
    }
    QVariant value = configs.value(key);
    muConfig.unlock();
    return value;
}

void Config::set(const QString& key, const QVariant& value) {
    muConfig.lockForWrite();
    configs.insert(key, value);
    muConfig.unlock();
}

void Config::unset(const QString& key) {
    muConfig.lockForWrite();
    configs.remove(key);
    muConfig.unlock();
}
