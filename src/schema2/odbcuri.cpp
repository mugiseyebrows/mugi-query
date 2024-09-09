#include "odbcuri.h"

OdbcUri::OdbcUri() {

}

OdbcUri::OdbcUri(const QString uri) {
    QStringList props = uri.split(";");
    QRegularExpression rx("(.*)=(.*)");
    for(const QString& prop: props) {
        auto m = rx.match(prop);
        if (m.hasMatch()) {
            QString key = m.captured(1).trimmed();
            QString value = m.captured(2).trimmed();
            mCase[key.toLower()] = key;
            mProps[key.toLower()] = value;
        } else {
            mPositional.append(prop);
        }
    }
}

QString OdbcUri::dbq() const {
    return mProps.value("dbq");
}

QString OdbcUri::driver() const {
    return mProps.value("driver");
}
