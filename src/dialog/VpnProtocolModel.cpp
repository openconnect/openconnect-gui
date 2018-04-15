#include "VpnProtocolModel.h"

// FIXME: this include should to into <openconnect.h>
#ifdef _WIN32
#include <winsock2.h>
#endif
#include <openconnect.h>

VpnProtocolModel::VpnProtocolModel(QObject* parent)
    : QAbstractListModel(parent)
{
    loadProtocols();
}

int VpnProtocolModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return m_protocols.size();
}

QVariant VpnProtocolModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const VpnProtocol protocol = m_protocols.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return QVariant{ protocol.prettyName };
    case Qt::ToolTipRole:
        return QVariant{ protocol.description };
    case Qt::UserRole + 1:
        return QVariant{ protocol.name };
    }

    return QVariant();
}

#if OPENCONNECT_CHECK_VER(5, 5)
void VpnProtocolModel::loadProtocols()
{
    struct oc_vpn_proto* protos = nullptr;

    if (openconnect_get_supported_protocols(&protos) >= 0) {
        for (oc_vpn_proto* p = protos; p->name; ++p) {
            m_protocols.append({ p->name, p->pretty_name, p->description });
        }
        openconnect_free_supported_protocols(protos);
    }
}
#else
void VpnProtocolModel::loadProtocols()
{
    m_protocols.append({ "anyconnect", "Cisco AnyConnect", "" });
    m_protocols.append({ "nc", "Juniper Network Connect", "" });
}
#endif
