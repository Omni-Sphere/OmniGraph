#pragma once

#include <graphqlservice/GraphQLService.h>

#include "Generated/ItemGroupQueryObject.h"
#include "Resolvers/ItemGroup/Resolver.hpp"
#include "ItemGroup.hpp"

#include "ServiceInyector.hpp"

class ItemGroupQuery
{
public:
    explicit ItemGroupQuery(std::shared_ptr<ServiceInyector> _services)
        : m_services(std::move(_services)) {}

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::ItemGroup>> getGetItemGroup(graphql::service::FieldParams &&params,
        const graphql::omnisphere::GetItemGroupInput &&ItemGroupArg) const
    {
        try
        {
            omnicore::dto::GetItemGroup getItemGroupDto{
                .Code = ItemGroupArg.Code,
                .Name = ItemGroupArg.Name};

            omnicore::model::ItemGroup itemGroupModel = m_services->ItemGroup->Get(getItemGroupDto);

            std::shared_ptr<ItemGroupResolver>  resolver = std::make_shared<ItemGroupResolver>(std::make_shared<omnicore::model::ItemGroup>(itemGroupModel), m_services);

            co_return std::make_shared<graphql::omnisphere::object::ItemGroup>(std::move(resolver));
        }
        catch (const std::exception &e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"ItemGroup", "GetItemGroup"}}});
        }
    }

    graphql::service::AwaitableObject<std::vector<std::shared_ptr<graphql::omnisphere::object::ItemGroup>>> getGetAllItemGroups(
        graphql::service::FieldParams &&params) const
    {
        try
        {
            std::vector<std::shared_ptr<graphql::omnisphere::object::ItemGroup>> ItemGroups;

            std::vector<omnicore::model::ItemGroup> itemGroupModel = m_services->ItemGroup->GetAll();

            for (const auto &itemGroup : itemGroupModel)
            {
                auto resolver = std::make_shared<ItemGroupResolver>(
                    std::make_shared<omnicore::model::ItemGroup>(itemGroup),
                    m_services);

                ItemGroups.push_back(std::make_shared<graphql::omnisphere::object::ItemGroup>(std::move(resolver)));
            }

            co_return ItemGroups;
        }
        catch (const std::exception &e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"ItemGroup", "GetAllItemGroups"}}});
        }
    }

private:
    std::shared_ptr<ServiceInyector> m_services;
};
