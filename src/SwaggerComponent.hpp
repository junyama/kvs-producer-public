
#ifndef SwaggerComponent_hpp
#define SwaggerComponent_hpp

#include "oatpp-swagger/Model.hpp"
#include "oatpp-swagger/Resources.hpp"
#include "oatpp/core/macro/component.hpp"

/**
 *  Swagger ui is served at
 *  http://host:port
 */
class SwaggerComponent {
private:
    constexpr static const char* TAG = "SwaggerComponent";
public:
  
  /**
   *  General API docs info
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::DocumentInfo>, swaggerDocumentInfo)([] {
    
    oatpp::swagger::DocumentInfo::Builder builder;
      
      oatpp::String url = getenv("SWAGGER_SERVER_IP");
      if (url) {
          url = "http://" + url + ":8080";
      } else {
          OATPP_LOGD(TAG, "SWAGGER_SERVER_IP environment variable not defined");
          url = "http://192.168.0.142:8080";
      }
      OATPP_LOGD(TAG, "swagger server url: %s", url->c_str());
    
    builder
    .setTitle("KVS producer control APIs")
    //.setDescription("KVS producer project with swagger docs")
    .setVersion("1.0")
    //.setContactName("Ivan Ovsyanochka")
    //.setContactUrl("https://oatpp.io/")
    
    //.setLicenseName("Apache License, Version 2.0")
    //.setLicenseUrl("http://www.apache.org/licenses/LICENSE-2.0")
    
    //.addServer(SERVER_URL, "server on remote host")
    .addServer(url, "server on remote host")
    .addServer("http://localhost:8080", "server on localhost");
    
    return builder.build();
    
  }());
  
  
  /**
   *  Swagger-Ui Resources (<oatpp-examples>/lib/oatpp-swagger/res)
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::Resources>, swaggerResources)([] {
    // Make sure to specify correct full path to oatpp-swagger/res folder !!!
    return oatpp::swagger::Resources::loadResources(OATPP_SWAGGER_RES_PATH);
  }());
  
};

#endif /* SwaggerComponent_hpp */
