#pragma once

#include "Generated/ItemGroupMutationObject.h"
#include "ItemGroup/ItemGroup.hpp"
#include "Resolvers/ItemGroup/Resolver.hpp"
#include <graphqlservice/GraphQLService.h>


#include "ServiceInyector.hpp"

class ItemGroupMutation {
public:
  explicit ItemGroupMutation(std::shared_ptr<ServiceInyector> _services)
      : m_services(std::move(_services)) {}

  graphql::service::AwaitableObject<
      std::shared_ptr<graphql::omnisphere::object::ItemGroup>>
  getCreateItemGroup(
      graphql::service::FieldParams &&,
      const graphql::omnisphere::CreateItemGroupInput &itemGroupInput) const {
    try {
      omnisphere::omnierp::dtos::CreateItemGroup createItemGroup =
          omnisphere::omnierp::dtos::CreateItemGroup(
              itemGroupInput.Code, itemGroupInput.Name,
              itemGroupInput.CreatedBy, itemGroupInput.CreateDate);

      if (m_services->ItemGroup->Add(createItemGroup)) {

        omnisphere::omnierp::models::ItemGroup itemGroupModel = m_services->ItemGroup->Get(
            omnisphere::omnierp::dtos::GetItemGroup{.Code = itemGroupInput.Code});

        std::shared_ptr<ItemGroupResolver> resolver =
            std::make_shared<ItemGroupResolver>(
                std::make_shared<omnisphere::omnierp::models::ItemGroup>(itemGroupModel),
                m_services);

        co_return std::make_shared<graphql::omnisphere::object::ItemGroup>(
            std::move(resolver));
      } else {
        throw std::runtime_error("Failed to create ItemGroup");
        co_return nullptr;
      }
    } catch (const std::exception &e) {
      throw graphql::service::schema_exception({graphql::service::schema_error{
          .message = e.what(), .path = {"ItemGroup", "CreateItemGroup"}}});
    }
  }

  graphql::service::AwaitableObject<
      std::shared_ptr<graphql::omnisphere::object::ItemGroup>>
  getUpdateItemGroup(
      graphql::service::FieldParams &&,
      const graphql::omnisphere::UpdateItemGroupInput &itemGroupInput) const {
    try {
      omnisphere::omnierp::dtos::UpdateItemGroup updateItemGroup =
          omnisphere::omnierp::dtos::UpdateItemGroup(
              itemGroupInput.Code, itemGroupInput.Name,
              itemGroupInput.LastUpdatedBy, itemGroupInput.UpdateDate);

      if (m_services->ItemGroup->Modify(updateItemGroup)) {

        omnisphere::omnierp::models::ItemGroup itemGroupModel = m_services->ItemGroup->Get(
            omnisphere::omnierp::dtos::GetItemGroup{.Code = itemGroupInput.Code});

        auto resolver = std::make_shared<ItemGroupResolver>(
            std::make_shared<omnisphere::omnierp::models::ItemGroup>(itemGroupModel),
            m_services);

        co_return std::make_shared<graphql::omnisphere::object::ItemGroup>(
            std::move(resolver));
      } else {
        throw std::runtime_error("Failed to update ItemGroup");
        co_return nullptr;
      }
    } catch (const std::exception &e) {
      throw graphql::service::schema_exception({graphql::service::schema_error{
          .message = e.what(), .path = {"ItemGroup", "UpdateItemGroup"}}});
    }
  }

private:
  std::shared_ptr<ServiceInyector> m_services;
};