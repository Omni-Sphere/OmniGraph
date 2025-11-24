#pragma once

#include <graphqlservice/GraphQLService.h>
#include "Generated/ItemQueryObject.h"
#include "Generated/ItemObject.h"
#include "Resolvers/Item/Resolver.hpp"

#include "ServiceInyector.hpp"
#include "Item.hpp"

class ItemQuery
{
    public:
    explicit ItemQuery(std::shared_ptr<ServiceInyector> _services)
        : m_services(std::move(_services)) 
    {}  

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::Item>> getGetItem(graphql::service::FieldParams&& params, const graphql::omnisphere::GetItemInput&& ItemArg) const
    {
        try
        {
            omnicore::dto::GetItem getItemDto{
                .Code = ItemArg.Code,
                .Name = ItemArg.Name
            };
            
            omnicore::model::Item itemModel = m_services->Item->Get(getItemDto);

            std::shared_ptr<ItemResolver> resolver = std::make_shared<ItemResolver>(std::make_shared<omnicore::model::Item>(itemModel), m_services);
            
            co_return std::make_shared<graphql::omnisphere::object::Item>(std::move(resolver));
        }
        catch (const std::exception& e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"Item", "GetItem"}}});
        }
    }

    graphql::service::AwaitableObject<std::vector<std::shared_ptr<graphql::omnisphere::object::Item>>> getGetAllItems(graphql::service::FieldParams&& params) const
    {
        try
        {
            std::vector<std::shared_ptr<graphql::omnisphere::object::Item>> Items;

            std::vector<omnicore::model::Item> model = m_services->Item->GetAll();

            for (const auto& item : model)
            {
                std::shared_ptr<ItemResolver> resolver = std::make_shared<ItemResolver>(std::make_shared<omnicore::model::Item>(item), m_services);
                Items.push_back(std::make_shared<graphql::omnisphere::object::Item>(std::move(resolver)));
            }

            co_return Items;
        }
        catch (const std::exception& e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"Item", "SearchItems"}}});
        }
    };

    graphql::service::AwaitableObject<std::vector<std::shared_ptr<graphql::omnisphere::object::Item>>> getSearchItems(graphql::service::FieldParams&& params, const graphql::omnisphere::SearchItemsInput&& FilterArg) const
    {
        try
        {
            std::vector<std::shared_ptr<graphql::omnisphere::object::Item>> Items;

            omnicore::dto::SearchItems filter;

            filter.CodeEqualsTo = FilterArg.CodeEqualsTo;

            std::vector<omnicore::model::Item> model = m_services->Item->Search(filter);

            for (const auto& item : model)
            {
                std::shared_ptr<ItemResolver> resolver = std::make_shared<ItemResolver>(std::make_shared<omnicore::model::Item>(item), m_services);
                Items.push_back(std::make_shared<graphql::omnisphere::object::Item>(std::move(resolver)));
            }

            co_return Items;
        }
        catch (const std::exception& e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"Item", "SearchItems"}}});
        }
    };

    private:
        std::shared_ptr<ServiceInyector> m_services;
};