#pragma once

#include "Generated/FloorObject.h"
#include "Generated/FloorQueryObject.h"
#include "Resolvers/Floor/Resolver.hpp"
#include <graphqlservice/GraphQLService.h>

#include "Floor/Floor.hpp"
#include "ServiceInyector.hpp"
#include <memory>
#include <vector>

class FloorQuery {
public:
  explicit FloorQuery(std::shared_ptr<ServiceInyector> services)
      : m_services(std::move(services)) {}

  graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::Floor>>
  getGetFloor(graphql::service::FieldParams &&params,
              const graphql::omnisphere::GetFloorInput &&FloorArg) const {
    try {
      omnisphere::dtos::GetFloor getFloorDto;
      getFloorDto.Entry = FloorArg.Entry;
      getFloorDto.Code = FloorArg.Code;

      omnisphere::models::Floor floorModel = m_services->Floor->Get(getFloorDto);

      std::shared_ptr<FloorResolver> resolver = std::make_shared<FloorResolver>(
          std::make_shared<omnisphere::models::Floor>(floorModel), m_services);

      co_return std::make_shared<graphql::omnisphere::object::Floor>(std::move(resolver));
    } catch (const std::exception &e) {
      throw graphql::service::schema_exception({graphql::service::schema_error{
          .message = e.what(), .path = {"Floor", "GetFloor"}}});
    }
  }

  graphql::service::AwaitableObject<std::vector<std::shared_ptr<graphql::omnisphere::object::Floor>>>
  getGetAllFloors(graphql::service::FieldParams &&params) const {
    try {
      std::vector<std::shared_ptr<graphql::omnisphere::object::Floor>> floors;
      std::vector<omnisphere::models::Floor> model = m_services->Floor->GetAll();

      for (const auto &floor : model) {
        std::shared_ptr<FloorResolver> resolver = std::make_shared<FloorResolver>(
            std::make_shared<omnisphere::models::Floor>(floor), m_services);
        floors.push_back(std::make_shared<graphql::omnisphere::object::Floor>(std::move(resolver)));
      }

      co_return floors;
    } catch (const std::exception &e) {
      throw graphql::service::schema_exception({graphql::service::schema_error{
          .message = e.what(), .path = {"Floor", "GetAllFloors"}}});
    }
  }

private:
  std::shared_ptr<ServiceInyector> m_services;
};
