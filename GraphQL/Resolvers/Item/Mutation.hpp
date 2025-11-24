#pragma once

#include <graphqlservice/GraphQLService.h>
#include "Generated/ItemMutationObject.h"
#include "Resolvers/Item/Resolver.hpp"

#include "ServiceInyector.hpp"
#include "Item.hpp"

class ItemMutation
{
public:
    explicit ItemMutation(std::shared_ptr<ServiceInyector> services)
        : m_services(std::move(services)) {}

    
    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::Item>> getCreateItem(graphql::service::FieldParams&&, const graphql::omnisphere::CreateItemInput& itemInput) const
    {
        try
        {
            omnicore::model::Item itemModel = m_services->Item->Add(omnicore::dto::CreateItem{
                itemInput.Code,
                itemInput.Name,
                itemInput.Description,
                itemInput.Image,
                itemInput.PurchaseItem.value_or(false),
                itemInput.SellItem.value_or(false),
                itemInput.InventoryItem.value_or(false),
                itemInput.Price,
                itemInput.Brand,
                itemInput.Group,
                itemInput.MinStock,
                itemInput.MaxStock,
                itemInput.MinOrder,
                itemInput.MaxOrder,
                itemInput.MinRequest,
                itemInput.MaxRequest,
                itemInput.CreatedBy,
                itemInput.CreateDate
            });

            std::shared_ptr<ItemResolver> resolver = std::make_shared<ItemResolver>(std::make_shared<omnicore::model::Item>(itemModel), m_services);
            
            co_return std::make_shared<graphql::omnisphere::object::Item>(std::move(resolver));
        }
        catch (const std::exception& e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"Item", "CreateItem"}}});
        }
    }

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::Item>> getUpdateItem(graphql::service::FieldParams&&, const graphql::omnisphere::UpdateItemInput& itemInput) const
    {
        try
        {            
            omnicore::model::Item itemModel = m_services->Item->Modify(omnicore::dto::UpdateItem{
                itemInput.Code,
                itemInput.Name,
                itemInput.Description,
                itemInput.Image,
                itemInput.IsActive,
                itemInput.PurchaseItem,
                itemInput.SellItem,
                itemInput.InventoryItem,
                itemInput.Price,
                itemInput.Brand,
                itemInput.Group,
                itemInput.MinStock,
                itemInput.MaxStock,
                itemInput.MinOrder,
                itemInput.MaxOrder,
                itemInput.MinRequest,
                itemInput.MaxRequest,
                itemInput.LastUpdatedBy,
                itemInput.UpdateDate
            });

            std::shared_ptr<ItemResolver> resolver = std::make_shared<ItemResolver>(std::make_shared<omnicore::model::Item>(itemModel), m_services);
            
            co_return std::make_shared<graphql::omnisphere::object::Item>(std::move(resolver));
        }
        catch (const std::exception& e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"Item", "UpdateItem"}}});
        }
    }
    
private:
    std::shared_ptr<ServiceInyector> m_services;
};