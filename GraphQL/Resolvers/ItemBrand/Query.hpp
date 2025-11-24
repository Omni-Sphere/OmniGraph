#pragma once

#include <graphqlservice/GraphQLService.h>

#include "Generated/ItemBrandQueryObject.h"
#include "Generated/ItemBrandObject.h"
#include "Resolvers/ItemBrand/Resolver.hpp"

#include "ServiceInyector.hpp"
#include "ItemBrand.hpp"

class ItemBrandQuery 
{
    public:
    explicit ItemBrandQuery(std::shared_ptr<ServiceInyector> _services)
        : m_services(std::move(_services)) {}

    graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::ItemBrand>> getGetItemBrand(graphql::service::FieldParams&& params, const graphql::omnisphere::GetItemBrandInput&& ItemBrandArg) const
    {
        try
        {
            omnicore::model::ItemBrand itemBrandModel;

            omnicore::dto::GetItemBrand getItemBrandDto{
                .Code = ItemBrandArg.Code,
                .Name = ItemBrandArg.Name
            };
            
            itemBrandModel = m_services->ItemBrand->Get(getItemBrandDto);

            std::shared_ptr<ItemBrandResolver> resolver = std::make_shared<ItemBrandResolver>(std::make_shared<omnicore::model::ItemBrand>(itemBrandModel), m_services);
            
            co_return std::make_shared<graphql::omnisphere::object::ItemBrand>(std::move(resolver));
        }
        catch (const std::exception& e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"ItemBrand", "GetItemBrand"}}});
        }
    }

    graphql::service::AwaitableObject<std::vector<std::shared_ptr<graphql::omnisphere::object::ItemBrand>>> getGetAllItemBrands(graphql::service::FieldParams&& params) const
    {
        try
        {
            std::vector<std::shared_ptr<graphql::omnisphere::object::ItemBrand>> ItemBrands;

            std::vector<omnicore::model::ItemBrand> model = m_services->ItemBrand->GetAll();

            
            for (const auto& itemBrand : model)
            {
                std::shared_ptr<ItemBrandResolver> resolver = std::make_shared<ItemBrandResolver>(std::make_shared<omnicore::model::ItemBrand>(itemBrand), m_services);
                ItemBrands.push_back(std::make_shared<graphql::omnisphere::object::ItemBrand>(std::move(resolver)));
            }

            co_return ItemBrands;
        }
        catch (const std::exception& e)
        {
            throw graphql::service::schema_exception({graphql::service::schema_error{
                .message = e.what(),
                .path = {"ItemBrand", "GetAllItemBrands"}}});
        }
    }

    private:
        std::shared_ptr<ServiceInyector> m_services;
};