#pragma once

#include "Generated/ItemObject.h"
#include "Generated/ItemQueryObject.h"
#include "Resolvers/Item/Resolver.hpp"
#include <graphqlservice/GraphQLService.h>

#include "Item/Item.hpp"
#include "ServiceInyector.hpp"
#include <memory>
#include <vector>


class ItemQuery {
public:
  explicit ItemQuery(std::shared_ptr<ServiceInyector> _services)
      : m_services(std::move(_services)) {}

  graphql::service::AwaitableObject<
      std::shared_ptr<graphql::omnisphere::object::Item>>
  getGetItem(graphql::service::FieldParams &&params,
             const graphql::omnisphere::GetItemInput &&ItemArg) const {
    try {
      omnisphere::omnierp::dtos::GetItem getItemDto{.Code = ItemArg.Code,
                                                    .Name = ItemArg.Name};

      omnisphere::omnierp::models::Item itemModel =
          m_services->Item->Get(getItemDto);

      std::shared_ptr<ItemResolver> resolver = std::make_shared<ItemResolver>(
          std::make_shared<omnisphere::omnierp::models::Item>(itemModel),
          m_services);

      co_return std::make_shared<graphql::omnisphere::object::Item>(
          std::move(resolver));
    } catch (const std::exception &e) {
      throw graphql::service::schema_exception({graphql::service::schema_error{
          .message = e.what(), .path = {"Item", "GetItem"}}});
    }
  }

  graphql::service::AwaitableObject<
      std::vector<std::shared_ptr<graphql::omnisphere::object::Item>>>
  getGetAllItems(graphql::service::FieldParams &&params) const {
    try {
      std::vector<std::shared_ptr<graphql::omnisphere::object::Item>> Items;

      std::vector<omnisphere::omnierp::models::Item> model =
          m_services->Item->GetAll();

      for (const auto &item : model) {
        std::shared_ptr<ItemResolver> resolver = std::make_shared<ItemResolver>(
            std::make_shared<omnisphere::omnierp::models::Item>(item),
            m_services);
        Items.push_back(std::make_shared<graphql::omnisphere::object::Item>(
            std::move(resolver)));
      }

      co_return Items;
    } catch (const std::exception &e) {
      throw graphql::service::schema_exception({graphql::service::schema_error{
          .message = e.what(), .path = {"Item", "SearchItems"}}});
    }
  };

  graphql::service::AwaitableObject<
      std::vector<std::shared_ptr<graphql::omnisphere::object::Item>>>
  getSearchItems(
      graphql::service::FieldParams &&params,
      const graphql::omnisphere::SearchItemsInput &&FilterArg) const {
    try {
      std::vector<std::shared_ptr<graphql::omnisphere::object::Item>> Items;

      omnisphere::omnierp::dtos::SearchItems filter;

      filter.CodeEqualsTo = FilterArg.CodeEqualsTo;

      std::vector<omnisphere::omnierp::models::Item> model =
          m_services->Item->Search(filter);

      for (const auto &item : model) {
        std::shared_ptr<ItemResolver> resolver = std::make_shared<ItemResolver>(
            std::make_shared<omnisphere::omnierp::models::Item>(item),
            m_services);
        Items.push_back(std::make_shared<graphql::omnisphere::object::Item>(
            std::move(resolver)));
      }

      co_return Items;
    } catch (const std::exception &e) {
      throw graphql::service::schema_exception({graphql::service::schema_error{
          .message = e.what(), .path = {"Item", "SearchItems"}}});
    }
  };

private:
  std::shared_ptr<ServiceInyector> m_services;
};