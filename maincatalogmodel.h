#ifndef MAINCATALOGMODEL_H
#define MAINCATALOGMODEL_H

#include <QAbstractListModel>
#include <QPixmap>

class MainCatalogModel : public QAbstractListModel
{
	Q_OBJECT
public:
	struct item {
		QString id;
		QString name;
		QString icon;
		QString description;
		int category_id = -1;
		int subcategory_id = -1;
	};
public:
	MainCatalogModel();

public:
	// QAbstractItemModel interface
	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	// QAbstractItemModel interface
	int columnCount(const QModelIndex &parent) const;

private:
	void fillItems();

private:
	QVector<item> m_items;
};

#endif // MAINCATALOGMODEL_H
