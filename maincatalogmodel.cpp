#include "maincatalogmodel.h"

#include "dbmanager.h"

MainCatalogModel::MainCatalogModel()
{
	fillItems();
}

int MainCatalogModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_items.size();
}

QVariant MainCatalogModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	int row = index.row();

	if (role == Qt::DisplayRole) {
		return QVariant(m_items[row].name);
	}

	if (role == Qt::WhatsThisRole) {
		return QVariant(m_items[row].description);
	}

	return QVariant();
}

int MainCatalogModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return 1;
}

void MainCatalogModel::fillItems()
{
	DBManager::instance()->fillItemsModel(m_items);
}


