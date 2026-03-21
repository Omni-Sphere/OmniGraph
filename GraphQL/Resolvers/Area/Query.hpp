#pragma once

#include "Generated/AreaObject.h"
#include "Generated/AreaQueryObject.h"
#include "Resolvers/Area/Resolver.hpp"
#include <graphqlservice/GraphQLService.h>

#include "Area/Area.hpp"
#include "ServiceInyector.hpp"
#include <memory>
#include <vector>

class AreaQuery {
public:
  explicit AreaQuery(std::shared_ptr<ServiceInyector> services)
      : m_services(std::move(services)) {}

  graphql::service::AwaitableObject<std::shared_ptr<graphql::omnisphere::object::Area>>
  getGetArea(graphql::service::FieldParams &&params,
             const graphql::omnisphere::GetAreaInput &&AreaArg) const {
    try {
      omnisphere::dtos::GetArea getAreaDto;
      getAreaDto.Entry = AreaArg.Entry;
      getAreaDto.Code = AreaArg.Code;

      omnisphere::models::Area areaModel = m_services->Area->Get(getAreaDto);

      std::shared_ptr<AreaResolver> resolver = std::make_shared<AreaResolver>(
          std::make_shared<omnisphere::models::Area>(areaModel), m_services);

      co_return std::make_shared<graphql::omnisphere::object::Area>(std::move(resolver));
    } catch (const std::exception &e) {
      throw graphql::service::schema_exception({graphql::service::schema_error{
          .message = e.what(), .path = {"Area", "GetArea"}}});
    }
  }

  graphql::service::AwaitableObject<std::vector<std::shared_ptr<graphql::omnisphere::object::Area>>>
  getGetAllAreas(graphql::service::FieldParams &&params) const {
    try {
      std::vector<std::shared_ptr<graphql::omnisphere::object::Area>> areas;
      std::vector<omnisphere::models::Area> model = m_services->Area->GetAll();

      for (const auto &area : model) {
        std::shared_ptr<AreaResolver> resolver = std::make_shared<AreaResolver>(
            std::make_shared<omnisphere::models::Area>(area), m_services);
        areas.push_back(std::make_shared<graphql::omnisphere::object::Area>(std::move(resolver)));
      }

      co_return areas;
    } catch (const std::exception &e) {
      throw graphql::service::schema_exception({graphql::service::schema_error{
          .message = e.what(), .path = {"Area", "GetAllAreas"}}});
    }
  }

private:
  std::shared_ptr<ServiceInyector> m_services;
};
