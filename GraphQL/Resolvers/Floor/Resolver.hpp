#pragma once

#include "GraphQL/Generated/FloorObject.h"
#include "GraphQL/Generated/AreaObject.h"
#include "Floor/Models/Floor.hpp"
#include "Resolvers/BaseResolver.hpp"
#include "Resolvers/Area/Resolver.hpp"
#include "ServiceInyector.hpp"

class FloorResolver : public BaseResolver<omnisphere::models::Floor> {
public:
  using BaseResolver::getCode;
  using BaseResolver::getCreateDate;
  using BaseResolver::getCreatedBy;
  using BaseResolver::getEntry;
  using BaseResolver::getLastUpdatedBy;
  using BaseResolver::getName;
  using BaseResolver::getUpdateDate;

  explicit FloorResolver(std::shared_ptr<omnisphere::models::Floor> model,
                         std::shared_ptr<ServiceInyector> services)
      : BaseResolver(std::move(model), services), m_services(services) {}

  graphql::service::AwaitableObject<std::vector<std::shared_ptr<graphql::omnisphere::object::Area>>>
  getAreas(graphql::service::FieldParams &&) const {
    std::vector<std::shared_ptr<graphql::omnisphere::object::Area>> result;
    
    for (const auto& area : m_model->Areas) {
        std::shared_ptr<AreaResolver> resolver = std::make_shared<AreaResolver>(
            std::make_shared<omnisphere::models::Area>(area), m_services);
        result.push_back(std::make_shared<graphql::omnisphere::object::Area>(resolver));
    }
    
    co_return result;
  }

private:
  std::shared_ptr<ServiceInyector> m_services;
};
