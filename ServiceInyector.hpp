#pragma once
#include "Database.hpp"
#include "Item/Item.hpp"
#include "ItemBrand/ItemBrand.hpp"
#include "ItemGroup/ItemGroup.hpp"
#include "Session/Session.hpp"
#include "User/User.hpp"
#include <memory>

class ServiceInyector {
private:
  std::shared_ptr<omnisphere::omnidata::services::Database> _db;

public:
  std::shared_ptr<omnisphere::omnicore::services::User> User;
  std::shared_ptr<omnisphere::omnierp::services::Item> Item;
  std::shared_ptr<omnisphere::omnicore::services::Session> Session;
  std::shared_ptr<omnisphere::omnierp::services::ItemGroup> ItemGroup;
  std::shared_ptr<omnisphere::omnierp::services::ItemBrand> ItemBrand;

  explicit ServiceInyector(
      std::shared_ptr<omnisphere::omnidata::services::Database> db)
      : _db(std::move(db)) {
    User = std::make_shared<omnisphere::omnicore::services::User>(_db);
    Item = std::make_shared<omnisphere::omnierp::services::Item>(_db);
    Session = std::make_shared<omnisphere::omnicore::services::Session>(_db);
    ItemGroup = std::make_shared<omnisphere::omnierp::services::ItemGroup>(_db);
    ItemBrand = std::make_shared<omnisphere::omnierp::services::ItemBrand>(_db);
  }
};
