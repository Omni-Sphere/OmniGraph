#pragma once

#include <graphqlservice/GraphQLService.h>
#include "Generated/ItemBrandMutationObject.h"
#include "Resolvers/ItemBrand/Mutation.hpp"

#include "ServiceInyector.hpp"
#include "ItemBrand.hpp"

class ItemBrandMutation
{
public:
    explicit ItemBrandMutation(std::shared_ptr<ServiceInyector> _services)
        : m_services(std::move(_services)) {}

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::ItemBrand>> getCreateItemBrand(graphql::service::FieldParams&&, const graphql::omnisphere::CreateItemBrandInput& itemBrandInput) const
    {
        try
        {
            omnicore::model::ItemBrand itemBrandModel;

            if(m_services->ItemBrand->Add(omnicore::dto::CreateItemBrand{
                itemBrandInput.Code,
                itemBrandInput.Name,
                itemBrandInput.CreatedBy,
                itemBrandInput.CreateDate
            }))
            {
                itemBrandModel = m_services->ItemBrand->Get(omnicore::dto::GetItemBrand{
                    .Code = itemBrandInput.Code
                });
            }

            std::shared_ptr<ItemBrandResolver> resolver = std::make_shared<ItemBrandResolver>(std::make_shared<omnicore::model::ItemBrand>(itemBrandModel), m_services);

            co_return std::make_shared<graphql::omnisphere::object::ItemBrand>(std::move(resolver));
        }
        catch (const std::exception& e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"ItemBrand", "CreateItemBrand"}}});
        }
    }

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::ItemBrand>> getUpdateItemBrand(graphql::service::FieldParams&&, const graphql::omnisphere::UpdateItemBrandInput& itemBrandInput) const
    {
        try
        {
            omnicore::model::ItemBrand itemBrandModel;
            if(m_services->ItemBrand->Modify(omnicore::dto::UpdateItemBrand{
                itemBrandInput.Code,
                itemBrandInput.Name,
                itemBrandInput.LastUpdatedBy,
                itemBrandInput.UpdateDate
            }))
            {
                itemBrandModel = m_services->ItemBrand->Get(omnicore::dto::GetItemBrand{
                    .Code = itemBrandInput.Code
                });
            }

            std::shared_ptr<ItemBrandResolver> resolver = std::make_shared<ItemBrandResolver>(std::make_shared<omnicore::model::ItemBrand>(itemBrandModel), m_services);

            co_return std::make_shared<graphql::omnisphere::object::ItemBrand>(std::move(resolver));
        }
        catch (const std::exception& e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"ItemBrand", "UpdateItemBrand"}}});
        }
    }

private:
    std::shared_ptr<ServiceInyector> m_services;
};