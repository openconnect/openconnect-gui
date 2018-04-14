#pragma once

#include "VpnProtocol.h"

#include <QAbstractListModel>

class VpnProtocolModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit VpnProtocolModel(QObject* parent = nullptr);

    // Basic functionality:
    //    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
    void loadProtocols();

    QList<VpnProtocol> m_protocols;
};
