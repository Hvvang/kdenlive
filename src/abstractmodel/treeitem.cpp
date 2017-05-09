/***************************************************************************
 *   Copyright (C) 2017 by Nicolas Carion                                  *
 *   This file is part of Kdenlive. See www.kdenlive.org.                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) version 3 or any later version accepted by the       *
 *   membership of KDE e.V. (or its successor approved  by the membership  *
 *   of KDE e.V.), which shall act as a proxy defined in Section 14 of     *
 *   version 3 of the license.                                             *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "treeitem.hpp"
#include "abstracttreemodel.hpp"
#include <QDebug>

TreeItem::TreeItem(const QList<QVariant> &data, std::shared_ptr<AbstractTreeModel> model, std::shared_ptr<TreeItem> parent, int id)
    : m_itemData(data)
    , m_parentItem(parent)
    , m_model(model)
    , m_depth(0)
    , m_id(id == -1 ? AbstractTreeModel::getNextId() : id)
{
}

std::shared_ptr<TreeItem> TreeItem::construct(const QList<QVariant> &data, std::shared_ptr<AbstractTreeModel> model, std::shared_ptr<TreeItem> parent, int id)
{
    std::shared_ptr<TreeItem> self(new TreeItem(data,model, parent, id));
    id = self->m_id;
    baseFinishConstruct(self);
    return self;
}

// static
void TreeItem::baseFinishConstruct(std::shared_ptr<TreeItem> self)
{
    if (auto ptr = self->m_model.lock()) {
        ptr->registerItem(self);
    } else {
        qDebug() << "Error : construction of treeItem failed because parent model is not available anymore";
        Q_ASSERT(false);
    }
}

TreeItem::~TreeItem()
{
    if (auto ptr = m_model.lock()) {
        ptr->deregisterItem(m_id);
    } else {
        qDebug() << "ERROR: Something went wrong when deleting TreeItem. Model is not available anymore";
    }
}

std::shared_ptr<TreeItem> TreeItem::appendChild(const QList<QVariant> &data)
{
    if (auto ptr = m_model.lock()) {
        auto child = construct(data, ptr, shared_from_this());
        child->m_depth = m_depth + 1;
        int id = child->getId();
        m_childItems.push_back(child);
        auto it = std::prev(m_childItems.end());
        m_iteratorTable[id] = it;
        ptr->notifyRowAboutToAppend(shared_from_this());
        ptr->notifyRowAppended();
        return child;
    }
    qDebug() << "ERROR: Something went wrong when appending child in TreeItem. Model is not available anymore";
    Q_ASSERT(false);
    return std::shared_ptr<TreeItem>();
}

void TreeItem::appendChild(std::shared_ptr<TreeItem> child)
{
    if (auto ptr = m_model.lock()) {
        child->m_depth = m_depth + 1;
        child->m_parentItem = shared_from_this();
        int id = child->getId();
        auto it = m_childItems.insert(m_childItems.end(), std::move(child));
        m_iteratorTable[id] = it;
        ptr->notifyRowAboutToAppend(shared_from_this());
        ptr->notifyRowAppended();
    } else {
        qDebug() << "ERROR: Something went wrong when appending child in TreeItem. Model is not available anymore";
        Q_ASSERT(false);
    }
}

void TreeItem::removeChild(std::shared_ptr<TreeItem> child)
{
    if (auto ptr = m_model.lock()) {
        ptr->notifyRowAboutToDelete(shared_from_this(), child->row());
        // get iterator corresponding to child
        auto it = m_iteratorTable[child->getId()];
        // deletion of child
        m_childItems.erase(it);
        // clean iterator table
        m_iteratorTable.erase(child->getId());
        child->m_depth = 0;
        child->m_parentItem.reset();
        ptr->notifyRowDeleted();
    } else {
        qDebug() << "ERROR: Something went wrong when removing child in TreeItem. Model is not available anymore";
        Q_ASSERT(false);
    }
}

void TreeItem::changeParent(std::shared_ptr<TreeItem> newParent)
{
    if (auto ptr = m_parentItem.lock()) {
        ptr->removeChild(shared_from_this());
    }
    if (newParent) {
        newParent->appendChild(shared_from_this());
        m_parentItem = newParent;
    }
}

std::shared_ptr<TreeItem> TreeItem::child(int row) const
{
    Q_ASSERT(row >= 0 && row < (int)m_childItems.size());
    auto it = m_childItems.cbegin();
    std::advance(it, row);
    return (*it);
}

int TreeItem::childCount() const
{
    return (int)m_childItems.size();
}

int TreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant TreeItem::dataColumn(int column) const
{
    return m_itemData.value(column);
}

std::weak_ptr<TreeItem> TreeItem::parentItem() const
{
    return m_parentItem;
}

int TreeItem::row() const
{
    if (auto ptr = m_parentItem.lock()) {
        // we compute the distance in the parent's children list
        auto it = ptr->m_childItems.begin();
        return (int)std::distance(it, (decltype(it))ptr->m_iteratorTable.at(m_id));
    }
    return -1;
}

int TreeItem::depth() const
{
    return m_depth;
}

int TreeItem::getId() const
{
    return m_id;
}
