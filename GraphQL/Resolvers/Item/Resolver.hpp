#pragma once

#include "GraphQL/Generated/ItemObject.h"
#include "GraphQL/Generated/ItemGroupObject.h"
#include "GraphQL/Generated/ItemBrandObject.h"

#include "Models/Item.hpp"

#include "ServiceInyector.hpp"
#include "Resolvers/BaseResolver.hpp"
#include "Resolvers/ItemGroup/Resolver.hpp"
#include "Resolvers/ItemBrand/Resolver.hpp"


class ItemResolver : public BaseResolver<omnicore::model::Item>
{
public:
    using BaseResolver::getEntry;
    using BaseResolver::getCode;
    using BaseResolver::getName;
    using BaseResolver::getCreateDate;
    using BaseResolver::getCreatedBy;
    using BaseResolver::getLastUpdatedBy;
    using BaseResolver::getUpdateDate;

    explicit ItemResolver(std::shared_ptr<omnicore::model::Item> model, std::shared_ptr<ServiceInyector> services)
        : BaseResolver(std::move(model), services), m_services(services)
    {
    }

    graphql::service::AwaitableScalar<std::optional<std::string>> getDescription(graphql::service::FieldParams &&) const
    {
        co_return m_model->Description;
    }

    graphql::service::AwaitableScalar<std::optional<std::string>> getImage(graphql::service::FieldParams &&) const
    {
        co_return m_model->Image;
    }

    graphql::service::AwaitableScalar<double> getPrice(graphql::service::FieldParams &&) const
    {
        co_return m_model->Price;
    }

    graphql::service::AwaitableScalar<bool> getIsActive(graphql::service::FieldParams &&) const
    {
        co_return m_model->IsActive;
    }

    graphql::service::AwaitableScalar<bool> getPurchaseItem(graphql::service::FieldParams &&) const
    {
        co_return m_model->PurchaseItem;
    }

    graphql::service::AwaitableScalar<bool> getSellItem(graphql::service::FieldParams &&) const
    {
        co_return m_model->SellItem;
    }

    graphql::service::AwaitableScalar<bool> getInventoryItem(graphql::service::FieldParams &&) const
    {
        co_return m_model->InventoryItem;
    }

    graphql::service::AwaitableScalar<double> getOnHand(graphql::service::FieldParams &&) const
    {
        co_return m_model->OnHand;
    }

    graphql::service::AwaitableScalar<std::optional<double>> getOnOrder(graphql::service::FieldParams &&) const
    {
        co_return m_model->OnOrder;
    }

    graphql::service::AwaitableScalar<std::optional<double>> getOnRequest(graphql::service::FieldParams &&) const
    {
        co_return m_model->OnRequest;
    }

    graphql::service::AwaitableScalar<std::optional<double>> getMinStock(graphql::service::FieldParams &&) const
    {
        co_return m_model->MinStock;
    }

    graphql::service::AwaitableScalar<std::optional<double>> getMaxStock(graphql::service::FieldParams &&) const
    {
        co_return m_model->MaxStock;
    }

    graphql::service::AwaitableScalar<std::optional<double>> getMinOrder(graphql::service::FieldParams &&) const
    {
        co_return m_model->MinOrder;
    }

    graphql::service::AwaitableScalar<std::optional<double>> getMaxOrder(graphql::service::FieldParams &&) const
    {
        co_return m_model->MaxOrder;
    }

    graphql::service::AwaitableScalar<std::optional<double>> getMinRequest(graphql::service::FieldParams &&) const
    {
        co_return m_model->MinRequest;
    }

    graphql::service::AwaitableScalar<std::optional<double>> getMaxRequest(graphql::service::FieldParams &&) const
    {
        co_return m_model->MaxRequest;
    }

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::ItemGroup>> getGroup(graphql::service::FieldParams &&) const
    {
        if (!m_model->Group.has_value())
            co_return nullptr;

        co_return co_await ResolveGroup(m_model->Group.value());
    }

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::ItemBrand>> getBrand(graphql::service::FieldParams &&) const
    {
        if (!m_model->Brand.has_value())
            co_return nullptr;

        co_return co_await ResolveBrand(m_model->Brand.value());
    }

private:
    std::shared_ptr<ServiceInyector> m_services;

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::ItemGroup>> ResolveGroup(int entry) const
    {
        omnicore::model::ItemGroup itemGroupModel = m_services->ItemGroup->Get(omnicore::dto::GetItemGroup{
            .Entry = entry
        });

        std::shared_ptr<ItemGroupResolver> resolver = std::make_shared<ItemGroupResolver>(std::make_shared<omnicore::model::ItemGroup>(itemGroupModel), m_services);

        co_return std::make_shared<graphql::omnisphere::object::ItemGroup>(resolver);
    }

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::ItemBrand>> ResolveBrand(int entry) const
    {
        omnicore::model::ItemBrand itemBrandModel = m_services->ItemBrand->Get(omnicore::dto::GetItemBrand{
            .Entry = entry
        });

        std::shared_ptr<ItemBrandResolver> resolver = std::make_shared<ItemBrandResolver>(std::make_shared<omnicore::model::ItemBrand>(itemBrandModel), m_services);

        co_return std::make_shared<graphql::omnisphere::object::ItemBrand>(resolver);
    }
};
