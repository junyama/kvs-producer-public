#ifndef CRUD_STATICCONTROLLER_HPP
#define CRUD_STATICCONTROLLER_HPP

#define WWW_PATH "/home/xavier/github/kvs-producer/www/"

#include "oatpp/core/data/stream/FileStream.hpp"
#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

class StaticController : public oatpp::web::server::api::ApiController {
private:
    const std::string wwwPath = WWW_PATH;
public:
  StaticController(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper)
  {}
public:

  static std::shared_ptr<StaticController> createShared(
    OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper) // Inject objectMapper component here as default parameter
  ){
    return std::make_shared<StaticController>(objectMapper);
  }

  ENDPOINT("GET", "/", root) {
    auto filePath = wwwPath + "index.html";
    auto fileStream = std::make_shared<oatpp::data::stream::FileInputStream>(filePath.c_str());
    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>(fileStream);
    auto response = OutgoingResponse::createShared(Status::CODE_200, body);  
    response->putHeader(Header::CONTENT_TYPE, "text/html");
    return response;
  }
    
  ENDPOINT("GET", "/favicon.ico", favicon) { //does not work 
    auto filePath = wwwPath + "favicon.ico";
    auto fileStream = std::make_shared<oatpp::data::stream::FileInputStream>(filePath.c_str());
    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>(fileStream);
    auto response = OutgoingResponse::createShared(Status::CODE_200, body);   
    response->putHeader(Header::CONTENT_TYPE, "image/x-icon");
    return response;
  }
    
  ENDPOINT("GET", "/style.css", style) {
    auto filePath = wwwPath + "style.css";
    auto fileStream = std::make_shared<oatpp::data::stream::FileInputStream>(filePath.c_str());
    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>(fileStream);
    auto response = OutgoingResponse::createShared(Status::CODE_200, body);
    response->putHeader(Header::CONTENT_TYPE, "text/css");
    return response;
  }
    
  ENDPOINT("GET", "/functions.js", functions) {
    auto filePath = wwwPath + "functions.js";
    auto fileStream = std::make_shared<oatpp::data::stream::FileInputStream>(filePath.c_str());
    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>(fileStream);
    auto response = OutgoingResponse::createShared(Status::CODE_200, body);  
    response->putHeader(Header::CONTENT_TYPE, "text/javascript");
    return response;
  }

};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen

#endif //CRUD_STATICCONTROLLER_HPP
